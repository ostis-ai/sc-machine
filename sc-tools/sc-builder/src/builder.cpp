/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder.hpp"
#include "keynodes.hpp"
#include "scs_translator.hpp"

#include <memory>
#include <unordered_set>

#include <fstream>
#include <filesystem>

#include <regex>

namespace impl
{
std::unordered_set<std::string> gSupportedFormats = {"scs"};

void NormalizeExt(std::string & ext)
{
  utils::StringUtils::ToLowerCase(ext);
}

bool IsSourceFile(std::string const & filePath)
{
  if (!std::filesystem::is_regular_file(filePath))
    return false;

  std::string ext = utils::StringUtils::GetFileExtension(filePath);
  if (!ext.empty())
    impl::NormalizeExt(ext);

  return gSupportedFormats.find(ext) != gSupportedFormats.cend();
}

void NormalizePath(std::string const & repoDirectoryPath, std::string & path)
{
  utils::StringUtils::Trim(path);
  std::stringstream stream;
  stream << repoDirectoryPath << path;
  path = stream.str();
}

bool IsSkipText(std::string const & string)
{
  return std::regex_match(string, std::regex("^\\s*$"))
         || utils::StringUtils::StartsWith(string, "#", true);
}

bool IsExcludedPath(std::string const & path)
{
  return utils::StringUtils::StartsWith(path, "!", true);
}

std::string GetFileName(std::string const & path)
{
  return path.substr(0, path.rfind('/') + 1);
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

private:
  std::vector<const sc_char *> m_params{};
  std::vector<std::string> m_names{};
};

}  // namespace impl

Builder::Builder() = default;

bool Builder::Run(BuilderParams const & params, sc_memory_params const & memoryParams)
{
  m_params = params;

  CollectExcludedPaths();
  CollectFiles();

  // initialize sc-memory
  bool noErrors = true;
  impl::ExtParser extParser;
  extParser(m_params.m_enabledExtPath);

  ScMemory::Initialize(memoryParams);
  m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "Builder");

  Keynodes::InitGlobal();

  std::cout << "Build knowledge base from sources... " << std::endl;

  if (m_params.m_resultStructureUpload)
  {
    ResolveOutputStructure();
  }

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

void Builder::ResolveOutputStructure()
{
  ScSystemIdentifierFiver fiver;
  m_ctx->HelperResolveSystemIdtf(m_params.m_resultStructureSystemIdtf, ScType::NodeConstStruct, fiver);
  m_outputStructure = fiver.addr1;

  auto const & AddElementToStructure = [this](ScAddr const & addr)
  {
    if (!m_ctx->HelperCheckEdge(m_outputStructure, addr, ScType::EdgeAccessConstPosPerm))
    {
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_outputStructure, addr);
    }
  };

  AddElementToStructure(fiver.addr2);
  AddElementToStructure(fiver.addr3);
  AddElementToStructure(fiver.addr4);
  AddElementToStructure(fiver.addr5);
}

bool Builder::ProcessFile(std::string const & fileName)
{
  std::shared_ptr<Translator> translator = CreateTranslator(fileName);
  if (!translator)
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't create translator for a file " << fileName);

  Translator::Params translateParams;
  translateParams.m_fileName = fileName;
  translateParams.m_autoFormatInfo = m_params.m_autoFormatInfo;
  translateParams.m_resultStructureUpload = m_params.m_resultStructureUpload;
  translateParams.m_outputStructure = m_outputStructure;

  return translator->Translate(translateParams);
}

void Builder::CollectExcludedPaths()
{
  m_excludedPaths.clear();

  if (!std::filesystem::is_regular_file(m_params.m_inputPath))
    return;

  std::string const & repoDirectoryPath = impl::GetFileName(m_params.m_inputPath);

  std::ifstream infile;
  infile.open(m_params.m_inputPath.c_str());
  if (!infile.is_open())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't open file: " << m_params.m_inputPath);

  std::string path;
  while (std::getline(infile, path))
  {
    if (!impl::IsExcludedPath(path))
      continue;

    path = path.substr(1);
    impl::NormalizePath(repoDirectoryPath, path);

    m_excludedPaths.insert(path);
  }
}

void Builder::CollectFiles(std::string const & path)
{
  auto const & IsExcludedPath = [this](std::string const & filePath) -> bool {
    return m_excludedPaths.find(filePath) != m_excludedPaths.cend();
  };

  if (IsExcludedPath(path))
    return;

  if (impl::IsSourceFile(path))
  {
    m_files.insert(path);
    return;
  }

  auto const & items = std::filesystem::directory_iterator{path};
  for (auto const & item : items)
  {
    std::string const sourcePath = item.path();
    if (impl::IsSourceFile(sourcePath) && !IsExcludedPath(sourcePath))
    {
      m_files.insert(sourcePath);
    }
    else if (std::filesystem::is_directory(sourcePath))
    {
      CollectFiles(sourcePath);
    }
  }
}

void Builder::CollectFiles()
{
  m_files.clear();

  if (std::filesystem::is_directory(m_params.m_inputPath))
  {
    CollectFiles(m_params.m_inputPath);
  }
  else if (std::filesystem::is_regular_file(m_params.m_inputPath))
  {
    std::string const & repoDirectoryPath = impl::GetFileName(m_params.m_inputPath);

    std::ifstream infile;
    infile.open(m_params.m_inputPath.c_str());
    if (!infile.is_open())
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't open file: " << m_params.m_inputPath);

    std::string path;
    while (std::getline(infile, path))
    {
      if (impl::IsSkipText(path) || impl::IsExcludedPath(path))
        continue;

      impl::NormalizePath(repoDirectoryPath, path);

      CollectFiles(path);
    }
  }
}

std::shared_ptr<Translator> Builder::CreateTranslator(std::string const & filePath)
{
  return impl::IsSourceFile(filePath) ? std::make_shared<SCsTranslator>(*m_ctx) : std::shared_ptr<Translator>();
}
