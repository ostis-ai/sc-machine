/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <unordered_set>
#include <string>

class ScRepoPathCollector
{
public:
  ScRepoPathCollector();

  using Sources = std::unordered_set<std::string>;

  bool IsSkipText(std::string const & string) const;
  bool IsExcludedPath(std::string const & path) const;

  std::string GetFileDirectory(std::string const & path) const;
  std::string GetFileExtension(std::string const & path) const;

  bool IsSourceFile(std::string const & filePath) const;
  bool IsRepoPathFile(std::string const & filePath) const;

  void ParseRepoPath(std::string const & repoPath, Sources & excludedSources, Sources & checkSources) const;

  void CollectBuildSources(
      std::string const & path,
      Sources const & excludedSources,
      Sources const & checkSources,
      Sources & buildSources);

private:
  void CollectBuildSources(std::string const & path, Sources const & excludedSources, Sources & buildSources);

  static std::unordered_set<std::string> const m_supportedSourcesFormats;
  static std::unordered_set<std::string> const m_supportedRepoPathFormats;
};
