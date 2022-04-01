/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"

#include <list>
#include <string>

struct BuilderParams
{
  //! Input directory path
  std::string m_inputPath;
  //! Output directory path
  std::string m_outputPath;
  //! Path to memory extensions
  std::string m_extensionsPath;
  //! Path to file with a list of enabled extensions
  std::string m_enabledExtPath;
  //! Flag to clear output
  bool m_clearOutput : 1;
  //! Flag to generate format information based on file extensions
  bool m_autoFormatInfo : 1;
  //! Path to configuration file
  std::string m_configFile;
};

class Builder
{
public:
  Builder();

  bool Run(BuilderParams const & options);

protected:
  bool ProcessFile(std::string const & filename);

  void CollectFiles(std::string const & path);
  void CollectFiles();

  std::shared_ptr<class Translator> CreateTranslator(std::string const & fileExt);

private:
  std::list<std::string> m_files;

  BuilderParams m_params;
  std::unique_ptr<ScMemoryContext> m_ctx;
};
