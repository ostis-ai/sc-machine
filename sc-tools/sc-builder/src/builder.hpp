/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"
#include "translator.hpp"

#include <list>
#include <string>

struct BuilderParams
{
  //! Input directory path
  std::string m_inputPath;
  //! Output directory path
  std::string m_outputPath;
  //! Path to file with a list of enabled extensions
  std::string m_enabledExtPath;
  //! Flag to generate format information based on file extensions
  bool m_autoFormatInfo : 1;
  //! Result structure system identifier
  std::string m_resultStructureSystemIdtf;
  //! Flag to create result structure
  sc_bool m_resultStructureUpload = SC_FALSE;
};

class Builder
{
public:
  using Sources = std::unordered_set<std::string>;

  static std::unordered_set<std::string> const m_supportedSourcesFormats;
  static std::unordered_set<std::string> const m_supportedRepoPathFormats;

  Builder();

  bool Run(BuilderParams const & params, sc_memory_params const & memoryParams);

protected:
  BuilderParams m_params;
  std::unique_ptr<ScMemoryContext> m_ctx;
  std::unordered_map<std::string, std::shared_ptr<Translator>> m_translators;

  ScAddr ResolveOutputStructure();

  bool BuildSources(Sources const & buildSources, ScAddr const & outputStructure);

  bool ProcessFile(std::string const & filename, ScAddr const & outputStructure);

  bool IsSourceFile(std::string const & filePath) const;
  bool IsRepoPathFile(std::string const & filePath) const;

  void ParseRepoPath(std::string const & repoPath, Sources & excludedSources, Sources & checkSources) const;

  void CollectBuildSources(std::string const & path, Sources const & excludedSources, Sources & buildSources);
  void CollectBuildSources(
      std::string const & path,
      Sources const & excludedSources,
      Sources const & checkSources,
      Sources & buildSources);
};
