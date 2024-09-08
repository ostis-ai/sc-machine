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

// BUFFER CLASS

Buffer::Buffer()
  : m_value("")
{
}

void Buffer::Write(std::string const & s)
{
  m_value += s;
}

void Buffer::AddTabs(std::size_t const & count)
{
  std::istringstream iss{m_value};
  std::ostringstream new_value;
  std::string line;
  std::string tabs(count * 4, SPACE[0]);

  while (std::getline(iss, line))
    new_value << tabs << line << NEWLINE;

  m_value = new_value.str();
}

std::string Buffer::GetValue() const
{
  return m_value;
}

// UTILS

std::string SCsWriter::MakeAlias(std::string const & prefix, std::string const & element_id)
{
  return ALIAS_PREFIX + prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(element_id, DASH, UNDERSCORE);
}

bool SCsWriter::IsVariable(std::string const & el_type)
{
  return el_type.find(VAR) != std::string::npos;
}

// SCS WRITER CLASS

bool SCsWriter::SCgIdentifierCorrector::IsRussianIdentifier(std::string const & identifier)
{
  std::regex identifierPatternRus("^[0-9a-zA-Z_\\xD0\\x80-\\xD1\\x8F\\xD1\\x90-\\xD1\\x8F\\xD1\\x91\\xD0\\x81*' ]*$");
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
    return CorrectIdentifierForVariable(systemIdentifier);

  if (!isVar && systemIdentifier[0] == UNDERSCORE[0])
    return CorrectIdentifierForNonVariable(systemIdentifier);

  return systemIdentifier;
}

std::string SCsWriter::SCgIdentifierCorrector::GenerateIdentifierForUnresolvedCharacters(
    std::string & systemIdentifier,
    std::string const & elementId,
    bool isVar)
{
  std::string prefix = isVar ? EL_VAR_PREFIX : EL_PREFIX;
  systemIdentifier = prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(elementId, DASH, UNDERSCORE);
  return systemIdentifier;
}

std::string SCsWriter::SCgIdentifierCorrector::CorrectIdentifierForVariable(std::string & systemIdentifier)
{
  return UNDERSCORE + utils::StringUtils::ReplaceAll(systemIdentifier, DASH, UNDERSCORE);
}

std::string SCsWriter::SCgIdentifierCorrector::CorrectIdentifierForNonVariable(std::string & systemIdentifier)
{
  return utils::StringUtils::ReplaceAll(systemIdentifier.substr(1), DASH, UNDERSCORE);
}

void SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(
    std::shared_ptr<SCgElement> const & scgElement,
    std::shared_ptr<SCsElement> & scsElement)
{
  scsElement->SetSystemIdentifier(scgElement->GetIdentifier());
  bool isVar = SCsWriter::IsVariable(scgElement->GetType());

  std::string const & systemIdentifier = scsElement->GetSystemIdentifier();
  if (!IsEnglishIdentifier(systemIdentifier))
  {
    if (IsRussianIdentifier(systemIdentifier))
      scsElement->SetMainIdentifier(systemIdentifier);

    scsElement->SetSystemIdentifier("");
  }

  std::string id = scgElement->GetId();
  std::string correctedIdentifier = scsElement->GetSystemIdentifier();
  correctedIdentifier = GenerateCorrectedIdentifier(correctedIdentifier, id, isVar);
  scsElement->SetSystemIdentifier(correctedIdentifier);

  auto const & tag = scgElement->GetTag();
  if (tag == PAIR || tag == ARC)
  {
    auto const & id = scgElement->GetId();
    scsElement->SetSystemIdentifier(SCsWriter::MakeAlias(EDGE, id));
  }
}

std::string SCsWriter::Write(SCgElements const & scgElementsMap, std::string const & filePath) const
{
  try
  {
    Buffer buffer;

    auto scgElements = ConvertMapToList(scgElementsMap);

    for (auto const & scgElement : scgElements)
    {
      auto scsElement = SCsElementFactory::CreateAndConvertElementFromSCgElement(scgElement);

      SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(scgElement, scsElement);

      if (!scsElement->GetMainIdentifier().empty())
        SCsWriter::WriteMainIdentifier(buffer, scsElement->GetSystemIdentifier(), scsElement->GetMainIdentifier());

      // If an element has a parent, there must be a terminal ancestor element that has no parent
      // By starting with ancestors, we ensure that all elements are covered

      if (scsElement)
      {
        std::string const & scgParent = scgElement->GetParent();
        std::string const & scgTag = scgElement->GetTag();

        if (scgParent == NO_PARENT)
        {
          if (scgTag == CONTOUR)
          {
            std::shared_ptr<SCsContour> contour = std::dynamic_pointer_cast<SCsContour>(scsElement);
            contour->SetSCgElements(scgElements);
            scsElement = contour;
          }
          scsElement->ConvertFromSCgElement(scgElement);
          buffer.Write(scsElement->Dump(filePath));
        }
        else if (scgTag == NODE || scgTag == BUS)
        {
          // All single nodes need to be written
          scsElement->ConvertFromSCgElement(scgElement);
          buffer.Write(scsElement->Dump(filePath));
        }
      }
    }
    return buffer.GetValue();
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("Exception in process elements: " << e.Message());
    return "";
  }
}

std::list<std::shared_ptr<SCgElement>> SCsWriter::ConvertMapToList(SCgElements const & scgElements)
{
  std::list<std::shared_ptr<SCgElement>> elementList;

  for (auto const & pair : scgElements)
    elementList.push_back(pair.second);

  return elementList;
}

void SCsWriter::WriteMainIdentifier(
    Buffer & buffer,
    std::string const & systemIdentifier,
    std::string const & mainIdentifier)
{
  std::string output;
  if (mainIdentifier[0] == OPEN_BRACKET[0])
    output = NEWLINE + systemIdentifier + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON + SPACE
             + mainIdentifier + ELEMENT_END + NEWLINE;
  else
    output = NEWLINE + systemIdentifier + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON + SPACE
             + OPEN_BRACKET + mainIdentifier + CLOSE_BRACKET + ELEMENT_END + NEWLINE;
  buffer.Write(output + NEWLINE);
}

// SCsFactory

std::shared_ptr<SCsElement> SCsElementFactory::CreateAndConvertElementFromSCgElement(
    std::shared_ptr<SCgElement> const & scgElement)
{
  return CreateAndConvertElementFromSCgElement(scgElement, {}, nullptr, nullptr);
}

std::shared_ptr<SCsElement> SCsElementFactory::CreateAndConvertElementFromSCgElement(
    std::shared_ptr<SCgElement> const & scgElement,
    std::list<std::shared_ptr<SCgElement>> const & scgElements,
    std::shared_ptr<SCgContour> const & scgContour,
    std::shared_ptr<SCsContour> const & scsContour)
{
  std::shared_ptr<SCsElement> scsElement = nullptr;
  std::string const & tag = scgElement->GetTag();

  if (tag == NODE || tag == BUS)
  {
    std::shared_ptr<SCgLink> link = std::dynamic_pointer_cast<SCgLink>(scgElement);
    if (link && link->GetContentType() != NO_CONTENT)
      scsElement = std::make_shared<SCsLink>();
    else if (scgContour)
    {
      // SCs cannot record individual nodes in a contour, so it is necessary to create
      // an explicit connection between the contour and the node
      scsElement = std::make_shared<SCsNodeInContour>(
          scgElements, scgElement->GetId(), scgContour->GetId(), scsContour->GetSystemIdentifier());
    }
    else
      scsElement = std::make_shared<SCsNode>();
  }
  else if (tag == PAIR || tag == ARC)
  {
    std::shared_ptr<SCgConnector> scgConnector = std::dynamic_pointer_cast<SCgConnector>(scgElement);
    if (scgConnector && scgContour && scgConnector->GetSource() == scgContour
        && scgConnector->GetTarget()->GetTag() == NODE)
    {
      // If there is an arc to a node in a contour,
      // then this node is in the contour, but not vice versa,
      // so we count multiple arcs from the NodeInContour

      auto scsConnector = std::make_shared<SCsConnector>();
      auto source = scgConnector->GetSource();
      scsElement = std::make_shared<SCsEdgeFromContourToNode>(
          scgElements,
          scgConnector->GetTarget()->GetId(),
          scgConnector->GetSource()->GetId(),
          scsConnector->GetSourceTargetIdentifier(source));
    }
    else
      scsElement = std::make_shared<SCsConnector>();
  }
  else if (tag == CONTOUR)
    scsElement = std::make_shared<SCsContour>();
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsWriter::CreateAndConvertElementFromSCgElement: Unsupported SCgElement type.");

  if (scsElement != nullptr)
  {
    SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(scgElement, scsElement);
    scsElement->ConvertFromSCgElement(scgElement);
    if (scsContour != nullptr)
      scsContour->AddElement(scsElement);
  }

  return scsElement;
}
