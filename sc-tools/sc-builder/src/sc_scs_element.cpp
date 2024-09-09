/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scs_element.hpp"

#include <filesystem>
#include <unordered_set>

#include <sc-memory/sc_debug.hpp>

#include "gwf_translator_const.hpp"

#include "sc_scg_element.hpp"
#include "sc_scs_writer.hpp"

using namespace Constants;

// SCsElement
void SCsElement::SetSystemIdentifier(std::string const & identifier)
{
  m_systemIdentifier = identifier;
}

void SCsElement::SetMainIdentifier(std::string const & identifier)
{
  m_mainIdentifier = identifier;
}

std::string SCsElement::GetSystemIdentifier() const
{
  return m_systemIdentifier;
}

std::string SCsElement::GetMainIdentifier() const
{
  return m_mainIdentifier;
}

// SCsNode
void SCsNode::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::string const & scgNodeType = scgElement->GetType();
  std::string scsNodeType = "";

  SCgToSCsTypes::ConvertSCgNodeTypeToSCsElementType(scgNodeType, scsNodeType);

  if (scsNodeType.empty())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "SCsNode::ConvertFromSCgElement: No matching sc.s-node type for sc.g-node `" << scgNodeType << "`.");

  type = scsNodeType;
}

void SCsNode::Dump(
    std::string const & filePath,
    Buffer & buffer,
    size_t depth,
    std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const
{
  buffer << NEWLINE;

  buffer.AddTabs(depth) << GetSystemIdentifier() << NEWLINE;
  buffer.AddTabs(depth) << SPACE << SPACE << SC_CONNECTOR_MAIN_L << SPACE << type << ELEMENT_END << NEWLINE;

  if (!this->GetMainIdentifier().empty())
    SCsWriter::WriteMainIdentifier(buffer, depth, this->GetSystemIdentifier(), this->GetMainIdentifier());
}

// SCsLink
enum class ContentType
{
  String = 1,
  Integer = 2,
  Float = 3,
  Image = 4
};

void SCsLink::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  auto link = std::dynamic_pointer_cast<SCgLink>(scgElement);
  if (link == nullptr)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsLink::ConvertFromSCgElement: Invalid SCgElement passed to SCsLink.");

  ContentType contentType = static_cast<ContentType>(std::stoi(link->GetContentType()));
  m_type = link->GetType();
  m_fileName = link->GetFileName();

  switch (contentType)
  {
  case ContentType::String:
    m_content = link->GetContentData();
    break;
  case ContentType::Integer:
    m_content = DOUBLE_QUOTE + INT64 + link->GetContentData() + DOUBLE_QUOTE;
    break;
  case ContentType::Float:
    m_content = DOUBLE_QUOTE + FLOAT + link->GetContentData() + DOUBLE_QUOTE;
    break;
  case ContentType::Image:
    m_urlContent = link->GetContentData();
    m_isUrl = true;
    break;
  default:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType,
        "SCsLink::ConvertFromSCgElement: Content type for link `" << link->GetId() << "` is not supported.");
  }
}

void SCsLink::Dump(
    std::string const & filePath,
    Buffer & buffer,
    size_t depth,
    std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const
{
  buffer << NEWLINE;

  if (m_isUrl)
  {
    bool isImage = false;
    std::string imageFormat;

    std::filesystem::path const & basePath = std::filesystem::path(filePath).parent_path();
    std::filesystem::path const & fullPath = basePath / m_fileName;

    std::string const & content = FILE_PREFIX + fullPath.filename().string();
    std::string const & fileExtension = fullPath.extension().string();
    auto it = IMAGE_FORMATS.find(fileExtension);
    if (it == IMAGE_FORMATS.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionItemNotFound, "SCsLink::Dump: File extension `" << fileExtension << "` is not supported.");

    imageFormat = it->second;
    isImage = true;

    std::ofstream file(fullPath, std::ios::binary);
    if (!file)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "SCsLink::Dump: Failed to open file for writing `" << fullPath.string() << "`.");

    file.write(m_urlContent.data(), m_urlContent.size());
    file.close();

    buffer.AddTabs(depth) << m_systemIdentifier << SPACE << EQUAL << SPACE << DOUBLE_QUOTE << content << DOUBLE_QUOTE
                          << ELEMENT_END << NEWLINE;
    if (isImage)
    {
      buffer.AddTabs(depth) << FORMAT_ARC << SPACE << EQUAL << SPACE << OPEN_PARENTHESIS << m_systemIdentifier << SPACE
                            << SC_CONNECTOR_DCOMMON_R << SPACE << imageFormat << CLOSE_PARENTHESIS << ELEMENT_END
                            << NEWLINE;
      buffer.AddTabs(depth) << NREL_FORMAT_ARC << SPACE << EQUAL << SPACE << OPEN_PARENTHESIS << NREL_FORMAT << SPACE
                            << SC_CONNECTOR_MAIN_R << SPACE << FORMAT_ARC << CLOSE_PARENTHESIS << ELEMENT_END
                            << NEWLINE;
    }
  }
  else
  {
    std::string const & isVar = SCsWriter::IsVariable(m_type) ? UNDERSCORE : "";
    buffer.AddTabs(depth) << m_systemIdentifier << SPACE << EQUAL << SPACE << isVar << OPEN_BRACKET << m_content
                          << CLOSE_BRACKET << ELEMENT_END << NEWLINE;
  }
}

// SCsConnector
void SCsConnector::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(scgElement);
  if (connector == nullptr)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsConnector::ConvertFromSCgElement: Invalid SCgElement passed to SCsConnector.");

  std::string const & connectorType = connector->GetType();
  std::string const & id = connector->GetId();

  m_isUnsupported = SCgToSCsTypes::ConvertSCgEdgeTypeToSCsElementType(connectorType, m_type);
  if (m_type.empty())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "SCsConnector::ConvertFromSCgElement: No matching sc.s-node type for sc.g-connector type `" << connectorType
                                                                                                    << "`.");

  m_alias = SCsWriter::MakeAlias(CONNECTOR, id);

  m_sourceIdentifier = GetIncidentElementIdentifier(connector->GetSource());
  m_targetIdentifier = GetIncidentElementIdentifier(connector->GetTarget());
}

std::string SCsConnector::GetIncidentElementIdentifier(std::shared_ptr<SCgElement> const & element) const
{
  auto scsElement = SCsElementFactory::CreateSCsElementForSCgElement(element);
  scsElement->ConvertFromSCgElement(element);
  SCsWriter::SCgIdentifierCorrector::GenerateSCsIdentifier(element, scsElement);
  return scsElement->GetSystemIdentifier();
}

void SCsConnector::Dump(
    std::string const & filePath,
    Buffer & buffer,
    size_t depth,
    std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const
{
  buffer << NEWLINE;

  if (m_isUnsupported)
  {
    buffer.AddTabs(depth) << m_alias << SPACE << EQUAL << SPACE << OPEN_PARENTHESIS << m_sourceIdentifier << SPACE
                          << SC_CONNECTOR_DCOMMON_R << SPACE << m_targetIdentifier << CLOSE_PARENTHESIS << ELEMENT_END
                          << NEWLINE;

    buffer.AddTabs(depth) << m_type << SPACE << SC_CONNECTOR_MAIN_R << SPACE << m_alias << ELEMENT_END << NEWLINE;
  }
  else
  {
    buffer.AddTabs(depth) << m_alias << SPACE << EQUAL << SPACE << OPEN_PARENTHESIS << m_sourceIdentifier << SPACE
                          << m_type << SPACE << m_targetIdentifier << CLOSE_PARENTHESIS << ELEMENT_END << NEWLINE;
  }
}

// SCsContour
void SCsContour::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCgContour> contour = std::dynamic_pointer_cast<SCgContour>(scgElement);
  if (contour == nullptr)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsContour::ConvertFromSCgElement Invalid SCgElement passed to SCsContour.");

  m_scgElements = contour->GetElements();
}

void SCsContour::Dump(
    std::string const & filePath,
    Buffer & buffer,
    size_t depth,
    std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const
{
  buffer << NEWLINE;

  Buffer contourBuffer;
  SCsWriter writer;
  writer.Write(m_scgElements, filePath, contourBuffer, depth + 1, writtenElements);

  buffer.AddTabs(depth) << GetSystemIdentifier() << SPACE << EQUAL << SPACE << OPEN_CONTOUR << NEWLINE;
  buffer << contourBuffer.GetValue() << NEWLINE;
  buffer.AddTabs(depth) << CLOSE_CONTOUR << ELEMENT_END << NEWLINE;
}
