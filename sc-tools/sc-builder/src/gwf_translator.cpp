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
#include "gwf_translator_const.hpp"

using namespace Constants;

GWFTranslator::GWFTranslator(ScMemoryContext & context)
  : Translator(context)
  , m_scsTranslator(context)
{
}

std::string GWFTranslator::GWFToScs(std::string const & xmlStr, std::string const & filePath)
{
  GWFParser parser;
  auto const elements = parser.Parse(xmlStr);

  if (elements.empty())
    SC_THROW_EXCEPTION(
        utils::ExceptionParseError, "GWFTranslator::GWFToScs: There are no elements in this file " + filePath);

  SCsWriter writer;
  std::string const & scsText = writer.Write(elements, filePath);
  return scsText;
}

std::string GWFTranslator::WriteStringToFile(std::string const & scsStr, std::string const & filePath)
{
  std::string const & scsSource = filePath + GENERATED_EXTENTION + SCS_EXTENTION;

  std::ofstream outputFile(scsSource, std::ios::binary);

  if (!outputFile.is_open())
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical, "GWFTranslator::WriteStringToFile: Error creating file for writing: " + scsSource);

  outputFile << scsStr;

  if (outputFile.fail())
  {
    outputFile.close();
    std::remove(scsSource.c_str());
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical, "GWFTranslator::WriteStringToFile: Error writing to file: " + scsSource);
  }

  outputFile.close();

  if (outputFile.fail())
  {
    std::remove(scsSource.c_str());
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical, "GWFTranslator::WriteStringToFile: Error closing the file: " + scsSource);
  }

  return scsSource;
}

std::string GWFTranslator::XmlFileToString(std::string const & filename)
{
  xmlInitParser();

  xmlDocPtr const & doc = xmlReadFile(filename.c_str(), nullptr, 0);
  if (doc == nullptr)
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "GWFTranslator::XmlFileToString: Failed to parse XML file");
  }

  xmlChar * xmlBuffer;
  int bufferSize;
  xmlDocDumpFormatMemory(doc, &xmlBuffer, &bufferSize, 0);

  std::string xmlString((char *)xmlBuffer, bufferSize);

  xmlFree(xmlBuffer);
  xmlFreeDoc(doc);
  xmlCleanupParser();

  return xmlString;
}

bool GWFTranslator::TranslateImpl(Params const & params)
{
  try
  {
    std::string const xmlStr = XmlFileToString(params.m_fileName);
    if (xmlStr.empty())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams, "GWFTranslator::TranslateImpl: Gwf file is empty: " + params.m_fileName);

    std::string const scsStr = GWFToScs(xmlStr, params.m_fileName);

    std::string const scsSource = WriteStringToFile(scsStr, params.m_fileName);

    Params newParams;
    newParams.m_fileName = scsSource;
    newParams.m_autoFormatInfo = params.m_autoFormatInfo;
    newParams.m_outputStructure = params.m_outputStructure;
    bool status = m_scsTranslator.Translate(newParams);

    std::filesystem::remove(scsSource.c_str());

    return status;
  }
  catch (std::exception const & e)
  {
    SC_LOG_ERROR("Error: " + std::string(e.what()));
    return false;
  }
}
