/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scs_writer.hpp"

#include <regex>

#include <sc-memory/sc_utils.hpp>

#include "sc_scg_element.hpp"
#include "sc_scs_element.hpp"

using namespace Constants;

std::string SCsWriter::MakeAlias(std::string const & prefix, std::string const & elementId)
{
  return ALIAS_PREFIX + prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(elementId, DASH, UNDERSCORE);
}

bool SCsWriter::IsVariable(std::string const & elementType)
{
  return elementType.find(VAR) != std::string::npos;
}

bool SCsWriter::SCgIdentifierCorrector::IsRussianIdentifier(std::string const & identifier)
{
  std::regex identifierPatternRus(R"(^[0-9a-zA-Z_\xD0\x80-\xD1\x8F\xD1\x90-\xD1\x8F\xD1\x91\xD0\x81*' ]*$)");
  return std::regex_match(identifier, identifierPatternRus);
}

bool SCsWriter::SCgIdentifierCorrector::IsEnglishIdentifier(std::string const & identifier)
{
  std::regex identifierPatternEng("^[0-9a-zA-Z_]*$");
  return std::regex_match(identifier, identifierPatternEng);
}

std::string SCsWriter::SCgIdentifierCorrector::GenerateCorrectedIdentifier(
    std::string & systemIdentifier,
    std::string const & elementId,
    bool isVar)
{
  if (systemIdentifier.empty())
    return GenerateIdentifierForUnresolvedCharacters(systemIdentifier, elementId, isVar);

  if (isVar && systemIdentifier[0] != UNDERSCORE[0])
    return GenerateSCsIdentifierForVariable(systemIdentifier);

  return systemIdentifier;
}

std::string SCsWriter::SCgIdentifierCorrector::GenerateIdentifierForUnresolvedCharacters(
    std::string & systemIdentifier,
    std::string const & elementId,
    bool isVar)
{
  std::string const & prefix = isVar ? EL_VAR_PREFIX : EL_PREFIX;
  systemIdentifier = prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(elementId, DASH, UNDERSCORE);
  return systemIdentifier;
}

std::string SCsWriter::SCgIdentifierCorrector::GenerateSCsIdentifierForVariable(std::string & systemIdentifier)
{
  return UNDERSCORE + systemIdentifier;
}

void SCsWriter::SCgIdentifierCorrector::GenerateSCsIdentifier(
    SCgElementPtr const & scgElement,
    SCsElementPtr & scsElement)
{
  scsElement->SetIdentifierForSCs(scgElement->GetIdentifier());
  bool isVar = SCsWriter::IsVariable(scgElement->GetType());

  std::string const & systemIdentifier = scsElement->GetIdentifierForSCs();
  if (!IsEnglishIdentifier(systemIdentifier))
  {
    if (IsRussianIdentifier(systemIdentifier))
      scsElement->SetMainIdentifier(systemIdentifier);

    scsElement->SetIdentifierForSCs("");
  }

  std::string id = scgElement->GetId();
  std::string correctedIdentifier = scsElement->GetIdentifierForSCs();
  correctedIdentifier = GenerateCorrectedIdentifier(correctedIdentifier, id, isVar);
  scsElement->SetIdentifierForSCs(correctedIdentifier);

  auto const & tag = scgElement->GetTag();
  if (tag == PAIR || tag == ARC)
    scsElement->SetIdentifierForSCs(SCsWriter::MakeAlias(CONNECTOR, id));
}

void SCsWriter::Write(
    SCgElements const & elements,
    std::string const & filePath,
    Buffer & buffer,
    size_t depth,
    std::unordered_set<SCgElementPtr> & writtenElements)
{
  std::list<SCgElementPtr> dependedConnectors;

  auto it = elements.cbegin();
  while (it != elements.cend())
  {
    SCgElementPtr scgElement = it->second;

    bool isElementWritable = true;
    std::string const & scgTag = scgElement->GetTag();
    if (scgTag == BUS)
      isElementWritable = false;
    else if (scgTag == PAIR || scgTag == ARC)
    {
      auto scgConnector = std::dynamic_pointer_cast<SCgConnector>(scgElement);
      auto const & source = scgConnector->GetSource();
      auto const & target = scgConnector->GetTarget();

      if (writtenElements.find(source) == writtenElements.cend()
          || writtenElements.find(target) == writtenElements.cend())
      {
        dependedConnectors.push_back(scgElement);
        isElementWritable = false;
      }
    }

    if (isElementWritable)
    {
      auto const & scsElement = SCsElementFactory::CreateSCsElementForSCgElement(scgElement);
      scsElement->ConvertFromSCgElement(scgElement);
      scsElement->Dump(filePath, buffer, depth, writtenElements);
      writtenElements.insert(scgElement);
    }

    ++it;
  }
  if (!dependedConnectors.empty())
  {
    size_t connectorsWrittenOnPreviousIteration = dependedConnectors.size();
    while (connectorsWrittenOnPreviousIteration != 0)
    {
      connectorsWrittenOnPreviousIteration = 0;
      for (auto dependedConnector = dependedConnectors.cbegin(); dependedConnector != dependedConnectors.cend();)
      {
        auto scgConnector = std::dynamic_pointer_cast<SCgConnector>(*dependedConnector);
        auto const & source = scgConnector->GetSource();
        auto const & target = scgConnector->GetTarget();
        bool isElementWritable =
            (writtenElements.find(source) != writtenElements.cend()
             && writtenElements.find(target) != writtenElements.cend());
        if (isElementWritable)
        {
          auto const & scsElement = SCsElementFactory::CreateSCsElementForSCgElement(*dependedConnector);
          scsElement->ConvertFromSCgElement(*dependedConnector);
          scsElement->Dump(filePath, buffer, depth, writtenElements);
          writtenElements.insert(*dependedConnector);
          ++connectorsWrittenOnPreviousIteration;
          dependedConnector = dependedConnectors.erase(dependedConnector);
        }
        else
          ++dependedConnector;
      }
    }
  }
  if (!dependedConnectors.empty())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "SCsWriter: unknown incident elements for " << dependedConnectors.size() << " connectors at `" << filePath
                                                    << "`.");
}

void SCsWriter::WriteMainIdentifier(
    Buffer & buffer,
    size_t depth,
    std::string const & systemIdentifier,
    std::string const & mainIdentifier)
{
  buffer << NEWLINE;
  buffer.AddTabs(depth) << systemIdentifier << NEWLINE;
  buffer.AddTabs(depth) << SPACE << SC_CONNECTOR_DCOMMON_R << SPACE << NREL_MAIN_IDTF << COLON << SPACE << OPEN_BRACKET
                        << mainIdentifier << CLOSE_BRACKET << ELEMENT_END << NEWLINE;
}
