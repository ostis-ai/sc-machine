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

Buffer & Buffer::operator<<(std::string const & string)
{
  m_value << string;
  return *this;
}

Buffer & Buffer::AddTabs(std::size_t const & count)
{
  std::string tabs(count * 4, SPACE[0]);
  m_value << tabs;
  return *this;
}

std::string Buffer::GetValue() const
{
  return m_value.str();
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
    scsElement->SetSystemIdentifier(SCsWriter::MakeAlias(CONNECTOR, id));
  }
}

void SCsWriter::Write(SCgElements const & scgElements, std::string const & filePath, Buffer & buffer) const
{
  try
  {
    for (auto [_, scgElement] : scgElements)
    {
      auto const & scgTag = scgElement->GetTag();
      if (scgElement->GetParent() != NO_PARENT && (scgTag != NODE && scgTag != BUS))
        continue;

      if (scgElement->GetTag() == BUS)
      {
        if (auto const & bus = std::dynamic_pointer_cast<SCgBus>(scgElement))
          scgElement = scgElements.at(bus->GetNodeId());
      }

      auto scsElement = SCsElementFactory::ConvertElementFromSCgElement(scgElement);
      SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(scgElement, scsElement);

      if (!scsElement->GetMainIdentifier().empty())
        SCsWriter::WriteMainIdentifier(buffer, scsElement->GetSystemIdentifier(), scsElement->GetMainIdentifier());

      scsElement->ConvertFromSCgElement(scgElement);
      scsElement->Dump(filePath, buffer, 0);
    }
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("SCsWriter::Write: Exception in process elements: " << e.Message());
  }
}

void SCsWriter::WriteMainIdentifier(
    Buffer & buffer,
    std::string const & systemIdentifier,
    std::string const & mainIdentifier)
{
  if (mainIdentifier[0] == OPEN_BRACKET[0])
    buffer << NEWLINE << systemIdentifier << NEWLINE << SPACE << SC_CONNECTOR_DCOMMON_R << SPACE << N_REL_MAIN_IDTF
           << COLON << SPACE << mainIdentifier << ELEMENT_END << NEWLINE;
  else
    buffer << NEWLINE << systemIdentifier << NEWLINE << SPACE << SC_CONNECTOR_DCOMMON_R << SPACE << N_REL_MAIN_IDTF
           << COLON << SPACE << OPEN_BRACKET << mainIdentifier << CLOSE_BRACKET << ELEMENT_END << NEWLINE;
}

// SCsFactory

std::shared_ptr<SCsElement> SCsElementFactory::ConvertElementFromSCgElement(
    std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCsElement> scsElement;
  std::string const & tag = scgElement->GetTag();

  if (tag == NODE || tag == BUS)
  {
    std::shared_ptr<SCgLink> link = std::dynamic_pointer_cast<SCgLink>(scgElement);
    if (link != nullptr && link->GetContentType() != NO_CONTENT)
      scsElement = std::make_shared<SCsLink>();
    else
      scsElement = std::make_shared<SCsNode>();
  }
  else if (tag == PAIR || tag == ARC)
    scsElement = std::make_shared<SCsConnector>();
  else if (tag == CONTOUR)
    scsElement = std::make_shared<SCsContour>();
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsWriter::ConvertElementFromSCgElement: Unsupported SCgElement type.");

  SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(scgElement, scsElement);
  return scsElement;
}
