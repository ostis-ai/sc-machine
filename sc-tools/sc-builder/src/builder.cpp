/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder.hpp"
#include "scs_translator.hpp"

#include <memory>
#include <unordered_set>

#include <fstream>
#include <filesystem>

#include <regex>

std::unordered_set<std::string> const Builder::m_supportedSourcesFormats = {"scs"};
std::unordered_set<std::string> const Builder::m_supportedRepoPathFormats = {"path"};

namespace impl
{
void NormalizeExt(std::string & ext)
{
  utils::StringUtils::ToLowerCase(ext);
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
  return std::regex_match(string, std::regex("^\\s*$")) || utils::StringUtils::StartsWith(string, "#", true);
}

bool IsExcludedPath(std::string const & path)
{
  return utils::StringUtils::StartsWith(path, "!", true);
}

bool IsFile(std::string const & path)
{
  return std::filesystem::is_regular_file(path);
}

bool IsDirectory(std::string const & path)
{
  return std::filesystem::is_directory(path);
}

std::string GetFileName(std::string const & path)
{
  return path.substr(0, path.rfind('/') + 1);
}

std::string GetFileExtension(std::string const & path)
{
  return utils::StringUtils::GetFileExtension(path);
}
}  // namespace impl

Builder::Builder() = default;

using Sources = std::unordered_set<std::string>;

bool Builder::Run(BuilderParams const & params, sc_memory_params const & memoryParams)
{
  m_params = params;

  // initialize sc-memory
  ScMemory::Initialize(memoryParams);

  Sources excludedSources, checkSources;
  if (impl::IsFile(m_params.m_inputPath))
  {
    ScConsole::PrintLine() << ScConsole::Color::Blue << "Parse repo path file... ";
    ParseRepoPath(m_params.m_inputPath, excludedSources, checkSources);
  }
  Sources buildSources;
  ScConsole::PrintLine() << ScConsole::Color::Blue << "Collect all sources... ";
  CollectBuildSources(excludedSources, checkSources, buildSources);

  m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "Builder");
  m_translators = {{"scs", std::make_shared<SCsTranslator>(*m_ctx)}};
  ScAddr const & outputStructure = m_params.m_resultStructureUpload ? ResolveOutputStructure() : ScAddr::Empty;

  ScConsole::PrintLine() << ScConsole::Color::Blue << "Build knowledge base from sources... ";
  bool const status = BuildSources(buildSources, outputStructure);

  m_ctx.reset();
  ScMemory::Shutdown(true);

  return status;
}

bool Builder::BuildSources(std::unordered_set<std::string> const & buildSources, ScAddr const & outputStructure)
{
  // process founded files
  bool status = true;

  size_t done = 0;
  for (auto const & fileName : buildSources)
  {
    ScConsole::SetColor(ScConsole::Color::White);
    std::cout << "[" << (++done) << "/" << buildSources.size() << "]: ";
    ScConsole::SetColor(ScConsole::Color::Grey);
    std::cout << fileName << " - ";

    try
    {
      ProcessFile(fileName, outputStructure);

      ScConsole::SetColor(ScConsole::Color::Green);
      std::cout << "ok" << std::endl;
    }
    catch (utils::ScException const & e)
    {
      ScConsole::SetColor(ScConsole::Color::Red);
      std::cout << "failed" << std::endl;
      ScConsole::ResetColor();
      std::cout << e.Message() << std::endl;
      status = false;
      break;
    }
  }

  ScConsole::PrintLine() << ScConsole::Color::Green << "Clean state...";
  Translator::Clean(*m_ctx);

  if (status)
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

  return status;
}

ScAddr Builder::ResolveOutputStructure()
{
  ScSystemIdentifierFiver fiver;
  m_ctx->HelperResolveSystemIdtf(m_params.m_resultStructureSystemIdtf, ScType::NodeConstStruct, fiver);
  ScAddr const & outputStructure = fiver.addr1;

  auto const & AddElementToStructure = [this, &outputStructure](ScAddr const & addr) {
    if (!m_ctx->HelperCheckEdge(outputStructure, addr, ScType::EdgeAccessConstPosPerm))
    {
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, outputStructure, addr);
    }
  };

  AddElementToStructure(fiver.addr2);
  AddElementToStructure(fiver.addr3);
  AddElementToStructure(fiver.addr4);
  AddElementToStructure(fiver.addr5);

  return outputStructure;
}

bool Builder::ProcessFile(std::string const & fileName, ScAddr const & outputStructure)
{
  Translator::Params translateParams;
  translateParams.m_fileName = fileName;
  translateParams.m_autoFormatInfo = m_params.m_autoFormatInfo;
  translateParams.m_outputStructure = outputStructure;

  std::string const & fileExt = impl::GetFileExtension(fileName);

  auto const & it = m_translators.find(fileExt);
  if (it == m_translators.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not found translators for sources with extension \"" << fileExt << "\"");

  return it->second->Translate(translateParams);
}

bool Builder::IsSourceFile(std::string const & filePath) const
{
  if (!impl::IsFile(filePath))
    return false;

  std::string ext = impl::GetFileExtension(filePath);
  if (!ext.empty())
    impl::NormalizeExt(ext);

  return Builder::m_supportedSourcesFormats.find(ext) != m_supportedSourcesFormats.cend();
}

bool Builder::IsRepoPathFile(std::string const & filePath) const
{
  if (!impl::IsFile(filePath))
    return false;

  std::string ext = impl::GetFileExtension(filePath);
  if (ext.empty())
    impl::NormalizeExt(ext);

  return Builder::m_supportedRepoPathFormats.find(ext) != m_supportedRepoPathFormats.cend();
}

void Builder::ParseRepoPath(
    std::string const & repoPath,
    std::unordered_set<std::string> & excludedSources,
    std::unordered_set<std::string> & checkSources) const
{
  if (!IsRepoPathFile(repoPath))
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Repo path file \"" << repoPath << "\" has invalid extension");

  std::ifstream infile;
  infile.open(repoPath);
  if (!infile.is_open())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't open repo path file \"" << repoPath << "\"");

  std::string path;
  std::string const & repoDirectoryPath = impl::GetFileName(repoPath);
  while (std::getline(infile, path))
  {
    if (impl::IsSkipText(path))
      continue;

    if (impl::IsExcludedPath(path))
    {
      path = path.substr(1);
      impl::NormalizePath(repoDirectoryPath, path);

      excludedSources.insert(path);
    }
    else
    {
      impl::NormalizePath(repoDirectoryPath, path);

      if (IsRepoPathFile(path))
        ParseRepoPath(path, excludedSources, checkSources);
      else
        checkSources.insert(path);
    }
  }
}

void Builder::CollectBuildSources(
    std::string const & path,
    std::unordered_set<std::string> const & excludedSources,
    std::unordered_set<std::string> & buildSources)
{
  auto const & IsExcludedPath = [&excludedSources](std::string const & filePath) -> bool {
    return excludedSources.find(filePath) != excludedSources.cend();
  };

  if (IsExcludedPath(path))
    return;

  if (IsSourceFile(path))
  {
    buildSources.insert(path);
    return;
  }

  if (!impl::IsDirectory(path))
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Sources path \"" << path << "\" is invalid");

  auto const & items = std::filesystem::directory_iterator{path};
  for (auto const & item : items)
  {
    std::string const sourcePath = item.path();

    if (IsExcludedPath(sourcePath))
      continue;

    if (IsSourceFile(sourcePath))
      buildSources.insert(sourcePath);
    else if (impl::IsDirectory(sourcePath))
      CollectBuildSources(sourcePath, excludedSources, buildSources);
  }
}

void Builder::CollectBuildSources(
    std::unordered_set<std::string> const & excludedSources,
    std::unordered_set<std::string> const & checkSources,
    std::unordered_set<std::string> & buildSources)
{
  buildSources.clear();

  if (impl::IsDirectory(m_params.m_inputPath))
    CollectBuildSources(m_params.m_inputPath, excludedSources, buildSources);
  else
  {
    for (std::string const & sourcesPath : checkSources)
    {
      CollectBuildSources(sourcesPath, excludedSources, buildSources);
    }
  }
}
