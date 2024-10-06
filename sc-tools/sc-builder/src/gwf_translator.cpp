/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "gwf_translator.hpp"

#include <filesystem>

#include <sc-memory/utils/sc_exec.hpp>

#include "builder_defines.hpp"

#include "gwf_parser.hpp"
#include "sc_scs_writer.hpp"
#include "gwf_translator_constants.hpp"

using namespace Constants;

GWFTranslator::GWFTranslator(ScMemoryContext & context)
  : Translator(context)
  , m_scsTranslator(context)
{
}

std::string GWFTranslator::TranslateGWFToSCs(std::string const & xmlStr, std::string const & filePath)
{
  SCgElements elementWithoutParents;

  GWFParser parser;
  parser.Parse(xmlStr, elementWithoutParents);

  if (elementWithoutParents.empty())
    SC_THROW_EXCEPTION(
        utils::ExceptionParseError,
        "GWFTranslator::TranslateGWFToSCs: There are no elements in file `" << filePath << "`.");

  SCsWriter writer;
  Buffer scsBuffer;
  std::unordered_set<SCgElementPtr> writtenElements;
  writer.Write(elementWithoutParents, filePath, scsBuffer, 0, writtenElements);

  return scsBuffer.GetValue();
}

std::string GWFTranslator::WriteStringToFile(std::string const & scsText, std::string const & fileName)
{
  std::string const & filePath = fileName + GENERATED_EXTENTION + SCS_EXTENTION;
  std::ofstream outputFile(filePath, std::ios::binary);

  if (!outputFile.is_open())
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical,
        "GWFTranslator::WriteStringToFile: Error creating file for writing `" << filePath << "`.");

  outputFile << scsText;

  if (outputFile.fail())
  {
    outputFile.close();
    std::remove(filePath.c_str());
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical, "GWFTranslator::WriteStringToFile: Error writing to file `" << filePath << "`.");
  }

  outputFile.close();

  if (outputFile.fail())
  {
    std::remove(filePath.c_str());
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical, "GWFTranslator::WriteStringToFile: Error closing the file `" << filePath << "`.");
  }

  return filePath;
}

std::string GWFTranslator::GetXMLFileContent(std::string const & fileName)
{
  xmlInitParser();

  xmlDocPtr const & document = xmlReadFile(fileName.c_str(), nullptr, 0);
  if (document == nullptr)
    SC_THROW_EXCEPTION(
        utils::ExceptionParseError, "GWFTranslator::GetXMLFileContent: Failed to parse XML file `" << fileName << "`.");

  xmlChar * xmlBuffer;
  int bufferSize;
  xmlDocDumpFormatMemory(document, &xmlBuffer, &bufferSize, 0);

  std::string xmlString((char *)xmlBuffer, bufferSize);

  xmlFree(xmlBuffer);
  xmlFreeDoc(document);
  xmlCleanupParser();

  return xmlString;
}

bool GWFTranslator::TranslateImpl(Params const & params)
{
  std::string const & gwfText = GetXMLFileContent(params.m_fileName);
  if (gwfText.empty())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "GWFTranslator::TranslateImpl: .gwf file `" << params.m_fileName << "` is empty.");

  std::string const & scsText = TranslateGWFToSCs(gwfText, params.m_fileName);
  std::string const & scsSource = WriteStringToFile(scsText, params.m_fileName);

  Params newParams;
  newParams.m_fileName = scsSource;
  newParams.m_autoFormatInfo = params.m_autoFormatInfo;
  newParams.m_outputStructure = params.m_outputStructure;
  bool status = m_scsTranslator.Translate(newParams);

  std::filesystem::remove(scsSource.c_str());

  return status;
}
