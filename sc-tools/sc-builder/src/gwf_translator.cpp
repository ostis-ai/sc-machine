/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "gwf_translator.hpp"

#include <filesystem>

#include <sc-memory/utils/sc_exec.hpp>

#define PYTHON_INTERPRETER "python3"
#define GWF_TRANSLATOR_SCRIPT SC_PREPARE_BUILD_SCRIPTS_PATH "/kb-scripts/gwf_to_scs.py"
#define GWF_TRANSLATOR_ERRORS_LOG SC_PREPARE_BUILD_SCRIPTS_PATH "/prepare_kb.log"
#define GWF_TRANSLATOR_INPUT_FILE_PARAM "--input="
#define GWF_TRANSLATOR_ERRORS_LOG_PARAM "--errors_file="

GWFTranslator::GWFTranslator(ScMemoryContext & context)
  : Translator(context)
  , m_scsTranslator(context)
{
}

std::string GWFTranslator::ConvertToSCsPath(std::string const & path) const
{
  return path + ".scs";
}

bool GWFTranslator::ErrorsExist(std::string const & path) const
{
  return std::filesystem::is_regular_file(path) && std::filesystem::file_size(path) > 0;
}

std::string GWFTranslator::GetError(std::string const & path) const
{
  std::string error;
  GetFileContent(path, error);
  return error;
}

bool GWFTranslator::TranslateImpl(Params const & params)
{
  std::string result;
  ScExec exec{
      {PYTHON_INTERPRETER,
       GWF_TRANSLATOR_SCRIPT,
       GWF_TRANSLATOR_INPUT_FILE_PARAM + params.m_fileName,
       GWF_TRANSLATOR_ERRORS_LOG_PARAM GWF_TRANSLATOR_ERRORS_LOG}};
  std::string const & scsSource = ConvertToSCsPath(params.m_fileName);

  if (ErrorsExist(GWF_TRANSLATOR_ERRORS_LOG))
  {
    std::string const & error = GetError(GWF_TRANSLATOR_ERRORS_LOG);
    std::filesystem::remove(scsSource);
    std::filesystem::remove(GWF_TRANSLATOR_ERRORS_LOG);

    SC_THROW_EXCEPTION(utils::ExceptionParseError, error);
  }

  Params newParams;
  newParams.m_fileName = scsSource;
  newParams.m_autoFormatInfo = params.m_autoFormatInfo;
  newParams.m_outputStructure = params.m_outputStructure;

  bool status = m_scsTranslator.Translate(newParams);
  std::filesystem::remove(scsSource);
  return status;
}
