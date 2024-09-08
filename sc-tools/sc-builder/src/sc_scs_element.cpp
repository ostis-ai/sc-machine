/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scs_element.hpp"

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
        "SCsNode::ConvertFromSCgElement: No matching scs node type for scg node: " + scgNodeType);

  type = scsNodeType;
}

std::string SCsNode::Dump(std::string const & filepath) const
{
  Buffer buffer;
  buffer.Write(
      m_systemIdentifier + NEWLINE + SPACE + SPACE + SC_EDGE_MAIN_L + SPACE + type + ELEMENT_END + NEWLINE + NEWLINE);
  return buffer.GetValue();
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
        utils::ExceptionInvalidType, "SCsLink::ConvertFromSCgElement: Content type not supported: " + link->GetId());
  }
}

std::string SCsLink::Dump(std::string const & filePath) const
{
  Buffer buffer;

  if (m_isUrl)
  {
    bool isImage = false;
    std::string imageFormat;

    std::filesystem::path const basePath = std::filesystem::path(filePath).parent_path();
    std::filesystem::path const fullPath = basePath / m_fileName;

    std::string content = FILE_PREFIX + fullPath.filename().string();

    std::string fileExtension = fullPath.extension().string();
    auto it = IMAGE_FORMATS.find(fileExtension);

    if (it != IMAGE_FORMATS.end())
    {
      imageFormat = it->second;
      isImage = true;
    }
    else
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "SCsLink::Dump: File extension not found " + fileExtension);

    std::ofstream file(fullPath, std::ios::binary);
    if (!file)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams, "SCsLink::Dump: Failed to open file for writing: " + fullPath.string());

    file.write(m_urlContent.data(), m_urlContent.size());
    file.close();

    buffer.Write(
        m_systemIdentifier + SPACE + EQUAL + SPACE + DOUBLE_QUOTE + content + DOUBLE_QUOTE + ELEMENT_END + NEWLINE);
    if (isImage)
    {
      buffer.Write(
          FORMAT_EDGE + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + m_systemIdentifier + SPACE + SC_EDGE_DCOMMON_R
          + SPACE + imageFormat + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
      buffer.Write(
          NREL_FORMAT_EDGE + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + NREL_FORMAT + SPACE + SC_EDGE_MAIN_R + SPACE
          + FORMAT_EDGE + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
    }
  }
  else
  {
    std::string isVar = SCsWriter::IsVariable(m_type) ? UNDERSCORE : "";
    buffer.Write(
        m_systemIdentifier + SPACE + EQUAL + SPACE + isVar + OPEN_BRACKET + m_content + CLOSE_BRACKET + ELEMENT_END
        + NEWLINE);
  }

  buffer.Write(NEWLINE);

  return buffer.GetValue();
}

// SCsConnector
void SCsConnector::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(scgElement);
  if (connector == nullptr)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsConnector::ConvertFromSCgElement: Invalid SCgElement passed to SCsConnector.");

  std::string const & edgeType = connector->GetType();
  std::string const & id = connector->GetId();

  m_isUnsupported = SCgToSCsTypes::ConvertSCgEdgeTypeToSCsElementType(edgeType, m_symbol);

  if (m_symbol.empty())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "SCsConnector::ConvertFromSCgElement: No matching scs node type for scg node: " + edgeType);

  m_alias = SCsWriter::MakeAlias(EDGE, id);

  m_sourceIdentifier = GetSourceTargetIdentifier(connector->GetSource());
  m_targetIdentifier = GetSourceTargetIdentifier(connector->GetTarget());
}

std::string SCsConnector::GetSourceTargetIdentifier(std::shared_ptr<SCgElement> const & element) const
{
  auto scsElement = SCsElementFactory::CreateAndConvertElementFromSCgElement(element);
  scsElement->ConvertFromSCgElement(element);
  SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(element, scsElement);
  return scsElement->GetSystemIdentifier();
}

std::string SCsConnector::Dump(std::string const & filepath) const
{
  Buffer buffer;
  if (m_isUnsupported)
  {
    buffer.Write(
        m_alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + m_sourceIdentifier + SPACE + SC_EDGE_DCOMMON_R + SPACE
        + m_targetIdentifier + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);

    buffer.Write(m_symbol + SPACE + SC_EDGE_MAIN_R + SPACE + m_alias + ELEMENT_END + NEWLINE);
  }
  else
  {
    buffer.Write(
        m_alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + m_sourceIdentifier + SPACE + m_symbol + SPACE
        + m_targetIdentifier + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
  }

  buffer.Write(NEWLINE);

  return buffer.GetValue();
}

// SCsContour
std::list<std::shared_ptr<SCsElement>> & SCsContour::GetSCsElements()
{
  return m_scsElements;
}

std::list<std::shared_ptr<SCgElement>> & SCsContour::GetSCgElements()
{
  return m_scgElements;
}

void SCsContour::SetSCgElements(std::list<std::shared_ptr<SCgElement>> elements)
{
  m_scgElements = elements;
}

void SCsContour::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCgContour> contour = std::dynamic_pointer_cast<SCgContour>(scgElement);
  if (contour == nullptr)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsContour::ConvertFromSCgElement Invalid SCgElement passed to SCsContour.");

  auto const & contourSCgElements = contour->GetElements();

  for (auto const & scgElement : contourSCgElements)
  {
    std::shared_ptr<SCsElement> scsElement = nullptr;
    std::string const & tag = scgElement->GetTag();

    if (tag == NODE || tag == BUS)
    {
      // SCs cannot record individual nodes in a contour, so it is necessary to create
      // an explicit connection between the contour and the node

      scsElement = std::make_shared<SCsNodeInContour>(
          m_scgElements, scgElement->GetId(), contour->GetId(), this->GetSystemIdentifier());
    }
    else if (tag == PAIR || tag == ARC)
    {
      std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(scgElement);

      // If there is an arc to a node in a contour,
      // then this node is in the contour, but not vice versa,
      // so we count multiple arcs from the NodeInContour

      if (connector->GetSource() == contour && connector->GetTarget()->GetTag() == NODE)
        scsElement = std::make_shared<SCsEdgeFromContourToNode>(
            m_scgElements, connector->GetTarget()->GetId(), contour->GetId(), this->GetSystemIdentifier());
      else
        scsElement = std::make_shared<SCsConnector>();
    }
    else if (tag == CONTOUR)
      scsElement = std::make_shared<SCsContour>();
    else
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidType, "SCsContour::ConvertFromSCgElement: Unsupported SCgElement type.");

    if (scsElement)
    {
      SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(scgElement, scsElement);
      scsElement->ConvertFromSCgElement(scgElement);
      AddElement(scsElement);
    }
  }
}

void SCsContour::AddElement(std::shared_ptr<SCsElement> const & element)
{
  m_scsElements.push_back(element);
}

std::string SCsContour::Dump(std::string const & filepath) const
{
  Buffer buffer;
  Buffer contourBuffer;

  for (auto const & element : m_scsElements)
  {
    if (auto nodeInContour = std::dynamic_pointer_cast<SCsNodeInContour>(element))
      buffer.Write(nodeInContour->Dump(filepath));
    else if (auto edgeFromContourToNode = std::dynamic_pointer_cast<SCsEdgeFromContourToNode>(element))
      buffer.Write(nodeInContour->Dump(filepath));
    else
      contourBuffer.Write(element->Dump(filepath));
  }

  buffer.Write(
      GetSystemIdentifier() + SPACE + EQUAL + SPACE + OPEN_CONTOUR + NEWLINE + contourBuffer.GetValue() + NEWLINE
      + CLOSE_CONTOUR + ELEMENT_END + NEWLINE);

  return buffer.GetValue();
}

// SCsMultipleElement
SCsMultipleElement::SCsMultipleElement(
    std::list<std::shared_ptr<SCgElement>> const & scgElements,
    std::string const & nodeId,
    std::string const & contourId,
    std::string const & contourIdentifier)
  : m_scgElements(scgElements)
  , m_nodeId(nodeId)
  , m_contourId(contourId)
  , m_contourIdentifier(contourIdentifier)
  , m_multipleArcCounter(0)
{
}

std::list<std::shared_ptr<SCgElement>> SCsMultipleElement::GetSCgElements()
{
  return m_scgElements;
}

void SCsMultipleElement::SetMultipleArcCounter(size_t counter)
{
  m_multipleArcCounter = counter;
}

std::string SCsMultipleElement::GetMultipleArcCounter() const
{
  return std::to_string(m_multipleArcCounter);
}

void SCsMultipleElement::SetNodeId(std::string const & id)
{
  m_nodeId = id;
}

std::string const & SCsMultipleElement::GetNodeId() const
{
  return m_nodeId;
}

void SCsMultipleElement::SetNodeIdentifier(std::string const & identifier)
{
  m_nodeIdentifier = identifier;
}

std::string const & SCsMultipleElement::GetNodeIdentifier() const
{
  return m_nodeIdentifier;
}

void SCsMultipleElement::SetContourId(std::string const & id)
{
  m_contourId = id;
}

std::string const & SCsMultipleElement::GetContourId() const
{
  return m_contourId;
}

void SCsMultipleElement::SetContourIdentifier(std::string const & identifier)
{
  m_contourIdentifier = identifier;
}

std::string const & SCsMultipleElement::GetContourIdentifier() const
{
  return m_contourIdentifier;
}

void SCsMultipleElement::AddWrittenPair(std::string const & contourId, std::string const & nodeId)
{
  m_writtenPairs[contourId] = nodeId;
}

bool SCsMultipleElement::IsPairWritten(std::string const & contourId, std::string const & nodeId) const
{
  auto it = m_writtenPairs.find(contourId);
  return it != m_writtenPairs.end() && it->second == nodeId;
}

void SCsMultipleElement::CalculateMultipleArcs(size_t & multipleArcCounter)
{
  for (auto const & el : GetSCgElements())
  {
    auto const & tag = el->GetTag();
    if (tag == NODE || tag == BUS)
    {
      auto const & parent = el->GetParent();
      if (parent == GetContourId())
      {
        multipleArcCounter++;
      }
    }
    else if (tag == PAIR || tag == ARC)
    {
      std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(el);
      auto const & sourceId = connector->GetSource()->GetId();
      auto const & targetId = connector->GetTarget()->GetId();
      if (sourceId == GetContourId() && targetId == GetNodeId())
        multipleArcCounter++;
    }
    else
      continue;
  }
}

// SCsNodeInContour
SCsNodeInContour::SCsNodeInContour(
    std::list<std::shared_ptr<SCgElement>> const & scgElements,
    std::string const & nodeId,
    std::string const & contourId,
    std::string const & contourIdentifier)
  : SCsMultipleElement(scgElements, nodeId, contourId, contourIdentifier)
{
}

void SCsNodeInContour::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element)
{
  if (IsPairWritten(GetContourId(), GetNodeId()))
    return;

  size_t multipleArcCounter = 1;
  CalculateMultipleArcs(multipleArcCounter);
  SetMultipleArcCounter(multipleArcCounter);
  AddWrittenPair(GetContourId(), GetNodeId());
}

std::string SCsNodeInContour::Dump(std::string const & filepath) const
{
  Buffer buffer;

  auto const edgeName = EDGE_FROM_CONTOUR + UNDERSCORE + GetContourId() + UNDERSCORE + EDGE_TO_NODE + UNDERSCORE
                        + GetNodeId() + UNDERSCORE + GetMultipleArcCounter();

  buffer.Write(
      edgeName + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + GetContourIdentifier() + SPACE + SC_EDGE_MAIN_R + SPACE
      + m_systemIdentifier + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE + NEWLINE);

  return buffer.GetValue();
}

// SCsEdgeFromContourToNode
SCsEdgeFromContourToNode::SCsEdgeFromContourToNode(
    std::list<std::shared_ptr<SCgElement>> const & scgElements,
    std::string const & nodeId,
    std::string const & contourId,
    std::string const & contourIdentifier)
  : SCsMultipleElement(scgElements, nodeId, contourId, contourIdentifier)
{
}

void SCsEdgeFromContourToNode::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element)
{
  if (IsPairWritten(GetContourId(), GetNodeId()))
    return;

  auto scsConnector = SCsElementFactory::CreateAndConvertElementFromSCgElement(element);
  SCsWriter::SCgIdentifierCorrector::CorrectIdentifier(element, scsConnector);

  SetNodeIdentifier(scsConnector->GetSystemIdentifier());

  size_t multipleArcCounter = 1;
  CalculateMultipleArcs(multipleArcCounter);
  SetMultipleArcCounter(multipleArcCounter);
  AddWrittenPair(GetContourId(), GetNodeId());
}

std::string SCsEdgeFromContourToNode::Dump(std::string const & filepath) const
{
  Buffer buffer;

  auto const edgeName = EDGE_FROM_CONTOUR + UNDERSCORE + GetContourId() + UNDERSCORE + EDGE_TO_NODE + UNDERSCORE
                        + GetNodeId() + UNDERSCORE + GetMultipleArcCounter();

  buffer.Write(
      edgeName + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + GetContourIdentifier() + SPACE + SC_EDGE_MAIN_R + SPACE
      + GetNodeIdentifier() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE + NEWLINE);

  return buffer.GetValue();
}
