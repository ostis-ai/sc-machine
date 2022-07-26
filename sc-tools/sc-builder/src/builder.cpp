/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder.hpp"
#include "keynodes.hpp"
#include "scs_translator.hpp"

#include <boost/filesystem.hpp>
#include <memory>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <unordered_set>

namespace impl
{
std::unordered_set<std::string> gSupportedFormats = {"scs", "scsi"};

std::string NormalizeExt(std::string ext)
{
  utils::StringUtils::ToLowerCase(ext);
  return ext;
}

bool IsSupportedFormat(std::string const & fileExt)
{
  return gSupportedFormats.find(NormalizeExt(fileExt)) != gSupportedFormats.end();
}

class ExtParser
{
public:
  void operator()(std::string const & file_path)
  {
    if (!file_path.empty())
    {
      std::ifstream infile(file_path);
      std::string line;
      while (std::getline(infile, line))
      {
        if (line.empty())
          continue;
        m_names.emplace_back(line);
      }

      m_params.clear();
      m_params.reserve(m_names.size() + 1);
      for (auto const & v : m_names)
        m_params.push_back(v.c_str());
      m_params.push_back(nullptr);
    }

    m_params.push_back(nullptr);
  }

  std::vector<const sc_char *> & GetParams()
  {
    return m_params;
  }

private:
  std::vector<const sc_char *> m_params;
  std::vector<std::string> m_names;
};

}  // namespace impl

Builder::Builder() = default;

bool Builder::Run(BuilderParams const & params, sc_memory_params const & memoryParams)
{
  m_params = params;

  CollectFiles();

  // initialize sc-memory
  bool noErrors = true;
  impl::ExtParser extParser;
  extParser(m_params.m_enabledExtPath);

  ScMemory::Initialize(memoryParams);
  m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "Builder");

  Keynodes::InitGlobal();

  std::cout << "Build knowledge base from sources... " << std::endl;

  // process founded files
  size_t done = 0;
  for (auto const & fileName : m_files)
  {
    ScConsole::SetColor(ScConsole::Color::White);
    std::cout << "[" << (++done) << "/" << m_files.size() << "]: ";
    ScConsole::SetColor(ScConsole::Color::Grey);
    std::cout << fileName << " - ";

    try
    {
      ProcessFile(fileName);

      ScConsole::SetColor(ScConsole::Color::Green);
      std::cout << "ok" << std::endl;
    }
    catch (utils::ScException const & e)
    {
      ScConsole::SetColor(ScConsole::Color::Red);
      std::cout << "failed" << std::endl;
      ScConsole::ResetColor();
      std::cout << e.Message() << std::endl;
      noErrors = false;
      break;
    }
  }

  ScConsole::PrintLine() << ScConsole::Color::Green << "Clean state...";
  Translator::Clean(*m_ctx);

  if (noErrors)
  {
    // print statistics
    ScMemoryContext::Stat const stats = m_ctx->CalculateStat();

    auto const allCount = stats.GetAllNum();

    auto const printLine = [](std::string const & name, uint32_t num, float percent) {
      ScConsole::PrintLine() << ScConsole::Color::LightBlue << name << ": " << ScConsole::Color::White << num << "("
                             << percent << "%)";
    };

    ScConsole::PrintLine() << ScConsole::Color::White << "Statistics";
    printLine("Nodes", stats.m_nodesNum, float(stats.m_nodesNum) / float(allCount) * 100);
    printLine("Edges", stats.m_edgesNum, float(stats.m_edgesNum) / float(allCount) * 100);
    printLine("Links", stats.m_linksNum, float(stats.m_linksNum) / float(allCount) * 100);
    ScConsole::PrintLine() << ScConsole::Color::LightBlue << "Total: " << ScConsole::Color::White << stats.GetAllNum();
  }

  m_ctx.reset();

  ScMemory::Shutdown(true);

  return noErrors;
}

bool Builder::ProcessFile(std::string const & fileName)
{
  // get file extension
  std::string const ext = utils::StringUtils::GetFileExtension(fileName);
  if (ext.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't determine file extension " << fileName);

    return false;
  }

  std::shared_ptr<Translator> translator = CreateTranslator(ext);
  if (!translator)
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't create translator for a file " << fileName);

    return false;
  }

  Translator::Params translateParams;
  translateParams.m_fileName = fileName;
  translateParams.m_autoFormatInfo = m_params.m_autoFormatInfo;

  return translator->Translate(translateParams);
}

void Builder::CollectFiles(std::string const & path)
{
  boost::filesystem::recursive_directory_iterator itEnd, it(path);
  while (it != itEnd)
  {
    if (!boost::filesystem::is_directory(*it))
    {
      boost::filesystem::path path = *it;
      std::string const fileName = path.normalize().string();
      std::string ext = utils::StringUtils::GetFileExtension(fileName);
      utils::StringUtils::ToLowerCase(ext);

      if (impl::IsSupportedFormat(ext))
        m_files.push_back(fileName);
    }

    try
    {
      ++it;
    }
    catch (std::exception & ex)
    {
      std::cout << ex.what() << std::endl;
      it.no_push();
      try
      {
        ++it;
      }
      catch (...)
      {
        std::cout << ex.what() << std::endl;
        return;
      }
    }
  }
}

void Builder::CollectFiles()
{
  m_files.clear();
  if (boost::filesystem::is_directory(m_params.m_inputPath))
  {
    CollectFiles(m_params.m_inputPath);
  }
  else if (boost::filesystem::is_regular_file(m_params.m_inputPath))
  {
    std::ifstream infile;
    infile.open(m_params.m_inputPath.c_str());
    if (infile.is_open())
    {
      std::string path;
      while (std::getline(infile, path))
      {
        boost::trim(path);
        if (utils::StringUtils::StartsWith(path, "#", true))
          continue;

        if (!path.empty())
        {
          if (boost::filesystem::is_directory(path))
          {
            CollectFiles(path);
          }
          else
          {
            SC_THROW_EXCEPTION(utils::ExceptionInvalidState, path << " isn't a directory");
          }
        }
      }
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't open file: " << m_params.m_inputPath);
    }
  }
}

std::shared_ptr<Translator> Builder::CreateTranslator(std::string const & fileExt)
{
  std::string const ext = impl::NormalizeExt(fileExt);

  if (ext == "scs" || ext == "scsi")
    return std::make_shared<SCsTranslator>(*m_ctx);

  return {};
}
