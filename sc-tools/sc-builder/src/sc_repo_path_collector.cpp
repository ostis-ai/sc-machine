/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_repo_path_collector.hpp"

#include <regex>
#include <filesystem>

#include <sc-memory/sc_utils.hpp>

std::unordered_set<std::string> const ScRepoPathCollector::m_supportedSourcesFormats = {"scs", "gwf"};
std::unordered_set<std::string> const ScRepoPathCollector::m_supportedRepoPathFormats = {"path"};

namespace impl
{
void NormalizeExt(std::string & ext)
{
  utils::StringUtils::ToLowerCase(ext);
}

void NormalizePath(std::string const & repoDirectoryPath, std::string & path)
{
  std::stringstream stream;
  stream << repoDirectoryPath << path;
  path = stream.str();
}

bool IsFile(std::string const & path)
{
  return std::filesystem::is_regular_file(path);
}

bool IsDirectory(std::string const & path)
{
  return std::filesystem::is_directory(path);
}
}  // namespace impl

ScRepoPathCollector::ScRepoPathCollector() = default;

bool ScRepoPathCollector::IsSkipText(std::string const & string) const
{
  return std::regex_match(string, std::regex("^\\s*$")) || utils::StringUtils::StartsWith(string, "#", true);
}

bool ScRepoPathCollector::IsExcludedPath(std::string const & path) const
{
  return utils::StringUtils::StartsWith(path, "!", true);
}

std::string ScRepoPathCollector::GetFileDirectory(std::string const & path) const
{
  return path.substr(0, path.rfind('/') + 1);
}

std::string ScRepoPathCollector::GetFileExtension(std::string const & path) const
{
  return utils::StringUtils::GetFileExtension(path);
}

bool ScRepoPathCollector::IsSourceFile(std::string const & filePath) const
{
  if (!impl::IsFile(filePath))
    return false;

  std::string ext = GetFileExtension(filePath);
  if (!ext.empty())
    impl::NormalizeExt(ext);

  return ScRepoPathCollector::m_supportedSourcesFormats.find(ext) != m_supportedSourcesFormats.cend();
}

bool ScRepoPathCollector::IsRepoPathFile(std::string const & filePath) const
{
  if (!impl::IsFile(filePath))
    return false;

  std::string ext = GetFileExtension(filePath);
  return ScRepoPathCollector::m_supportedRepoPathFormats.find(ext) != m_supportedRepoPathFormats.cend();
}

void ScRepoPathCollector::ParseRepoPath(std::string const & repoPath, Sources & excludedSources, Sources & checkSources)
    const
{
  if (!IsRepoPathFile(repoPath))
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Repo path file \"" << repoPath << "\" has invalid extension");

  std::ifstream infile;
  infile.open(repoPath);
  if (!infile.is_open())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't open repo path file \"" << repoPath << "\"");

  std::string path;
  std::string const & repoDirectoryPath = GetFileDirectory(repoPath);
  while (std::getline(infile, path))
  {
    if (IsSkipText(path))
      continue;

    utils::StringUtils::Trim(path);

    if (IsExcludedPath(path))
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

  for (std::string const & source : excludedSources)
    checkSources.erase(source);
}

void ScRepoPathCollector::CollectBuildSources(
    std::string const & path,
    Sources const & excludedSources,
    Sources & buildSources)
{
  auto const & IsExcludedPath = [&excludedSources](std::string const & filePath) -> bool
  {
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

void ScRepoPathCollector::CollectBuildSources(
    std::string const & path,
    Sources const & excludedSources,
    Sources const & checkSources,
    Sources & buildSources)
{
  buildSources.clear();

  if (impl::IsDirectory(path))
    CollectBuildSources(path, excludedSources, buildSources);
  else
  {
    for (std::string const & sourcesPath : checkSources)
    {
      CollectBuildSources(sourcesPath, excludedSources, buildSources);
    }
  }
}
