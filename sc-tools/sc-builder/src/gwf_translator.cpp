/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "gwf_translator.hpp"

#include <filesystem>

#include <sc-memory/utils/sc_exec.hpp>

#include "gwf_parser.hpp"
#include "sc_scs_writer.hpp"

GWFTranslator::GWFTranslator(ScMemoryContext & context)
  : Translator(context)
  , m_scsTranslator(context)
{
}

std::string GWFTranslator::GwfToScs(const std::string xmlStr, const std::string filePath)
{
  GwfParser parser;

  std::vector<std::unordered_map<std::string, std::string>> elements = parser.Parse(xmlStr);

  if (elements.empty())
  {
    throw std::runtime_error("There are no elements in this file " + filePath);
  }

  ScsWriter writer;
  const std::string scsStr = writer.Write(elements, filePath);

  return scsStr;
}

std::string GWFTranslator::WriteStringToFile(std::string const & scsStr, std::string const & filePath)
{
  const std::string scsSource = filePath + ".scs";

  std::ofstream outputFile(scsSource, std::ios::binary);

  if (!outputFile.is_open())
  {
    throw std::runtime_error("Error creating file for writing: " + scsSource);
  }

  outputFile << scsStr;

  if (outputFile.fail())
  {
    throw std::runtime_error("Error writing to file: " + scsSource);
  }

  outputFile.close();

  if (outputFile.fail())
  {
    throw std::runtime_error("Error closing the file: " + scsSource);
  }

  return scsSource;
}

std::string GWFTranslator::XmlFileToString(std::string const & filename)
{
  xmlInitParser();

  xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
  if (doc == nullptr)
  {
    throw std::runtime_error("Failed to parse XML file");
  }

  xmlChar * xmlBuffer;
  int bufferSize;
  xmlDocDumpFormatMemory(doc, &xmlBuffer, &bufferSize, 1);

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
    const std::string xmlStr = XmlFileToString(params.m_fileName);
    if (xmlStr.empty())
    {
      throw std::runtime_error("Gwf file is empty: " + params.m_fileName);
    }

    const std::string scsStr = GwfToScs(xmlStr, params.m_fileName);

    const std::string scsSource = WriteStringToFile(scsStr, params.m_fileName);

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
    std::cerr << "Error: " << e.what() << '\n';

    return false;
  }
}
