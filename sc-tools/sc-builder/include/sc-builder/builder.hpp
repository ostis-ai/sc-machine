/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"

#include <string>

#include "translator.hpp"
#include "sc_repo_path_collector.hpp"

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
  Builder();

  bool Run(BuilderParams const & params, sc_memory_params const & memoryParams);

protected:
  BuilderParams m_params;
  std::unique_ptr<ScMemoryContext> m_ctx;
  ScRepoPathCollector m_collector;
  std::unordered_map<std::string, std::shared_ptr<Translator>> m_translators;

  ScAddr ResolveOutputStructure();

  bool BuildSources(ScRepoPathCollector::Sources const & buildSources, ScAddr const & outputStructure);

  bool ProcessFile(std::string const & filename, ScAddr const & outputStructure);

  void DumpStatistics();
};
