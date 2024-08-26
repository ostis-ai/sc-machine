#include "sc_scs_writer.hpp"

using namespace Constants;

// BUFFER CLASS

Buffer::Buffer()
  : value("")
{
}

void Buffer::Write(std::string const & s)
{
  value += s;
}

void Buffer::AddTabs(std::size_t const & count)
{
  std::istringstream iss(value);
  std::ostringstream new_value;
  std::string line;
  std::string tabs(count * 4, SPACE[0]);

  while (std::getline(iss, line))
  {
    new_value << tabs << line << NEWLINE;
  }

  value = new_value.str();
}

std::string Buffer::GetValue() const
{
  return value;
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

std::unordered_map<std::string, std::string> const IMAGE_FORMATS = {{".png", "format_png"}};

bool SCsWriter::CorrectorOfSCgIdtf::IsRussianIdtf(std::string const & idtf)
{
  std::regex idtfPatternRus("^[0-9a-zA-Z_\\xD0\\x80-\\xD1\\x8F\\xD1\\x90-\\xD1\\x8F\\xD1\\x91\\xD0\\x81*' ]*$");
  return std::regex_match(idtf, idtfPatternRus);
}

bool SCsWriter::CorrectorOfSCgIdtf::IsEnglishIdtf(std::string const & idtf)
{
  std::regex idtfPatternEng("^[0-9a-zA-Z_]*$");
  return std::regex_match(idtf, idtfPatternEng);
}

std::string SCsWriter::CorrectorOfSCgIdtf::GenerateCorrectedIdtf(
    std::string & systemIdtf,
    std::string const & elementId,
    bool isVar)
{
  if (systemIdtf.empty())
    return GenerateIdtfForUnresolvedCharacters(systemIdtf, elementId, isVar);

  if (isVar && systemIdtf[0] != UNDERSCORE[0])
    return CorrectIdtfForVariable(systemIdtf);

  if (!isVar && systemIdtf[0] == UNDERSCORE[0])
    return CorrectIdtfForNonVariable(systemIdtf);

  return systemIdtf;
}

std::string SCsWriter::CorrectorOfSCgIdtf::GenerateIdtfForUnresolvedCharacters(
    std::string & systemIdtf,
    std::string const & elementId,
    bool isVar)
{
  std::string prefix = isVar ? EL_VAR_PREFIX : EL_PREFIX;
  systemIdtf = prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(elementId, DASH, UNDERSCORE);
  return systemIdtf;
}

std::string SCsWriter::CorrectorOfSCgIdtf::CorrectIdtfForVariable(std::string & systemIdtf)
{
  return UNDERSCORE + utils::StringUtils::ReplaceAll(systemIdtf, DASH, UNDERSCORE);
}

std::string SCsWriter::CorrectorOfSCgIdtf::CorrectIdtfForNonVariable(std::string & systemIdtf)
{
  return utils::StringUtils::ReplaceAll(systemIdtf.substr(1), DASH, UNDERSCORE);
}

void SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(
    std::shared_ptr<SCgElement> const & scgElement,
    std::shared_ptr<SCsElement> & scsElement)
{
  scsElement->SetSystemIdtf(scgElement->GetIdtf());
  bool isVar = SCsWriter::IsVariable(scgElement->GetType());

  std::string const & systemIdtf = scsElement->GetSystemIdtf();

  if (!IsEnglishIdtf(systemIdtf))
  {
    if (IsRussianIdtf(systemIdtf))
    {
      scsElement->SetMainIdtf(systemIdtf);
    }
    scsElement->SetSystemIdtf("");
  }

  std::string id = scgElement->GetId();
  std::string correctedIdtf = scsElement->GetSystemIdtf();

  correctedIdtf = GenerateCorrectedIdtf(correctedIdtf, id, isVar);
  scsElement->SetSystemIdtf(correctedIdtf);

  auto const & tag = scgElement->GetTag();
  if (tag == PAIR || tag == ARC)
  {
    auto const & id = scgElement->GetId();
    scsElement->SetSystemIdtf(SCsWriter::MakeAlias(EDGE, id));
  }
}

std::string SCsWriter::Write(
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & scgElementsMap,
    std::string const & filePath) const
{
  try
  {
    Buffer buffer;

    auto scgElements = ConvertMapToList(scgElementsMap);

    for (auto const & scgElement : scgElements)
    {
      auto scsElement = SCsElementFactory::CreateAndConvertElementFromSCgElement(scgElement);

      SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(scgElement, scsElement);

      if (!scsElement->GetMainIdtf().empty())
      {
        SCsWriter::WriteMainIdtf(buffer, scsElement->GetSystemIdtf(), scsElement->GetMainIdtf());
      }

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

std::list<std::shared_ptr<SCgElement>> SCsWriter::ConvertMapToList(
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & scgElements)
{
  std::list<std::shared_ptr<SCgElement>> elementList;

  for (auto const & pair : scgElements)
  {
    elementList.push_back(pair.second);
  }

  return elementList;
}

void SCsWriter::WriteMainIdtf(Buffer & buffer, std::string const & systemIdtf, std::string const & mainIdtf)
{
  std::string output;
  if (mainIdtf[0] == OPEN_BRACKET[0])
  {
    output = NEWLINE + systemIdtf + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON + SPACE
             + mainIdtf + ELEMENT_END + NEWLINE;
  }
  else
  {
    output = NEWLINE + systemIdtf + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON + SPACE
             + OPEN_BRACKET + mainIdtf + CLOSE_BRACKET + ELEMENT_END + NEWLINE;
  }
  buffer.Write(output + NEWLINE);
}

// Converter

// SCsNode

void SCsNode::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::string const & scgNodeType = scgElement->GetType();
  std::string scsNodeType = "";

  SCgToSCsElement::ConvertSCgNodeTypeToSCsElementType(scgNodeType, scsNodeType);

  if (scsNodeType.empty())
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "SCsNode::ConvertFromSCgElement: No matching scs node type for scg node: " + scgNodeType);
  }

  type = scsNodeType;
}

std::string SCsNode::Dump(std::string const & filepath) const
{
  Buffer buffer;

  buffer.Write(systemIdtf + NEWLINE + SPACE + SPACE + SC_EDGE_MAIN_L + SPACE + type + ELEMENT_END + NEWLINE + NEWLINE);

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
  if (!link)
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsLink::ConvertFromSCgElement: Invalid SCgElement passed to SCsLink.");
  }

  ContentType contentType = static_cast<ContentType>(std::stoi(link->GetContentType()));
  type = link->GetType();
  fileName = link->GetFileName();

  switch (contentType)
  {
  case ContentType::String:
    content = link->GetContentData();
    break;
  case ContentType::Integer:
    content = DOUBLE_QUOTE + INT64 + link->GetContentData() + DOUBLE_QUOTE;
    break;
  case ContentType::Float:
    content = DOUBLE_QUOTE + FLOAT + link->GetContentData() + DOUBLE_QUOTE;
    break;
  case ContentType::Image:
    urlContent = link->GetContentData();
    isUrl = true;
    break;
  default:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsLink::ConvertFromSCgElement: Content type not supported: " + link->GetId());
  }
}

std::string SCsLink::Dump(std::string const & filePath) const
{
  Buffer buffer;

  if (isUrl)
  {
    bool isImage = false;
    std::string imageFormat;

    std::filesystem::path const basePath = std::filesystem::path(filePath).parent_path();
    std::filesystem::path const fullPath = basePath / fileName;

    std::string content = FILE_PREFIX + fullPath.filename().string();

    std::string fileExtension = fullPath.extension().string();
    auto it = IMAGE_FORMATS.find(fileExtension);

    if (it != IMAGE_FORMATS.end())
    {
      imageFormat = it->second;
      isImage = true;
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "SCsLink::Dump: File extension not found " + fileExtension);
    }

    std::ofstream file(fullPath, std::ios::binary);
    if (!file)
    {
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams, "SCsLink::Dump: Failed to open file for writing: " + fullPath.string());
    }
    file.write(urlContent.data(), urlContent.size());
    file.close();

    buffer.Write(systemIdtf + SPACE + EQUAL + SPACE + DOUBLE_QUOTE + content + DOUBLE_QUOTE + ELEMENT_END + NEWLINE);
    if (isImage)
    {
      buffer.Write(
          FORMAT_EDGE + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + systemIdtf + SPACE + SC_EDGE_DCOMMON_R + SPACE
          + imageFormat + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
      buffer.Write(
          NREL_FORMAT_EDGE + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + NREL_FORMAT + SPACE + SC_EDGE_MAIN_R + SPACE
          + FORMAT_EDGE + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
    }
  }
  else
  {
    std::string isVar = SCsWriter::IsVariable(type) ? UNDERSCORE : "";
    buffer.Write(
        systemIdtf + SPACE + EQUAL + SPACE + isVar + OPEN_BRACKET + content + CLOSE_BRACKET + ELEMENT_END + NEWLINE);
  }

  buffer.Write(NEWLINE);

  return buffer.GetValue();
}

// SCsConnector

void SCsConnector::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(scgElement);
  if (!connector)
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsConnector::ConvertFromSCgElement: Invalid SCgElement passed to SCsConnector.");
  }

  std::string const & edgeType = connector->GetType();
  std::string const & id = connector->GetId();

  isUnsupported = SCgToSCsElement::ConvertSCgEdgeTypeToSCsElementType(edgeType, symbol);

  if (symbol.empty())
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "SCsConnector::ConvertFromSCgElement: No matching scs node type for scg node: " + edgeType);
  }

  alias = SCsWriter::MakeAlias(EDGE, id);

  sourceIdtf = GetSourceTargetIdtf(connector->GetSource());
  targetIdtf = GetSourceTargetIdtf(connector->GetTarget());
}

std::string SCsConnector::GetSourceTargetIdtf(std::shared_ptr<SCgElement> const & element) const
{
  auto scsElement = SCsElementFactory::CreateAndConvertElementFromSCgElement(element);
  scsElement->ConvertFromSCgElement(element);
  SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(element, scsElement);
  return scsElement->GetSystemIdtf();
}

std::string SCsConnector::Dump(std::string const & filepath) const
{
  Buffer buffer;

  if (isUnsupported)
  {
    buffer.Write(
        alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + sourceIdtf + SPACE + SC_EDGE_DCOMMON_R + SPACE + targetIdtf
        + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);

    buffer.Write(symbol + SPACE + SC_EDGE_MAIN_R + SPACE + alias + ELEMENT_END + NEWLINE);
  }
  else
  {
    buffer.Write(
        alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + sourceIdtf + SPACE + symbol + SPACE + targetIdtf
        + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
  }

  buffer.Write(NEWLINE);

  return buffer.GetValue();
}

// SCsContour

void SCsContour::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCgContour> contour = std::dynamic_pointer_cast<SCgContour>(scgElement);
  if (!contour)
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsContour::ConvertFromSCgElement Invalid SCgElement passed to SCsContour.");
  }

  auto const & contourSCgElements = contour->GetElements();

  for (auto const & scgElement : contourSCgElements)
  {
    std::shared_ptr<SCsElement> scsElement = nullptr;
    std::string const & tag = scgElement->GetTag();

    if (tag == NODE || tag == BUS)
    {
      // SCs cannot record individual nodes in a contour, so it is necessary to create
      // an explicit connection between the contour and the node

      scsElement =
          std::make_shared<SCsNodeInContour>(scgElements, scgElement->GetId(), contour->GetId(), this->GetSystemIdtf());
    }
    else if (tag == PAIR || tag == ARC)
    {
      std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(scgElement);

      // If there is an arc to a node in a contour,
      // then this node is in the contour, but not vice versa,
      // so we count multiple arcs from the NodeInContour

      if (connector->GetSource() == contour && connector->GetTarget()->GetTag() == NODE)
      {
        scsElement = std::make_shared<SCsEdgeFromContourToNode>(
            scgElements, connector->GetTarget()->GetId(), contour->GetId(), this->GetSystemIdtf());
      }
      else
      {
        scsElement = std::make_shared<SCsConnector>();
      }
    }
    else if (tag == CONTOUR)
    {
      scsElement = std::make_shared<SCsContour>();
    }
    else
    {
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidType, "SCsContour::ConvertFromSCgElement: Unsupported SCgElement type.");
    }

    if (scsElement)
    {
      SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(scgElement, scsElement);
      scsElement->ConvertFromSCgElement(scgElement);
      AddElement(scsElement);
    }
  }
}

void SCsContour::AddElement(std::shared_ptr<SCsElement> const & element)
{
  scsElements.push_back(element);
}

std::string SCsContour::Dump(std::string const & filepath) const
{
  Buffer buffer;
  Buffer contourBuffer;

  for (auto const & element : scsElements)
  {
    if (auto nodeInContour = std::dynamic_pointer_cast<SCsNodeInContour>(element))
    {
      buffer.Write(nodeInContour->Dump(filepath));
    }
    else if (auto edgeFromContourToNode = std::dynamic_pointer_cast<SCsEdgeFromContourToNode>(element))
    {
      buffer.Write(nodeInContour->Dump(filepath));
    }
    else
    {
      contourBuffer.Write(element->Dump(filepath));
    }
  }

  buffer.Write(
      systemIdtf + SPACE + EQUAL + SPACE + OPEN_CONTOUR + NEWLINE + contourBuffer.GetValue() + NEWLINE + CLOSE_CONTOUR
      + ELEMENT_END + NEWLINE);

  return buffer.GetValue();
}

// MultipluElements

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
      {
        multipleArcCounter++;
      }
    }
    else
      continue;
  }
}

// SCsNode in contour

void SCsNodeInContour::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element)
{
  if (IsPairWritten(GetContourId(), GetNodeId()))
  {
    return;
  }

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
      edgeName + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + GetContourIdtf() + SPACE + SC_EDGE_MAIN_R + SPACE
      + systemIdtf + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE + NEWLINE);

  return buffer.GetValue();
}

// SCsEdge from contour to node

void SCsEdgeFromContourToNode::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element)
{
  if (IsPairWritten(GetContourId(), GetNodeId()))
  {
    return;
  }

  auto scsConnector = SCsElementFactory::CreateAndConvertElementFromSCgElement(element);
  SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(element, scsConnector);

  SetNodeIdtf(scsConnector->GetSystemIdtf());

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
      edgeName + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + GetContourIdtf() + SPACE + SC_EDGE_MAIN_R + SPACE
      + GetNodeIdtf() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE + NEWLINE);

  return buffer.GetValue();
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
    {
      scsElement = std::make_shared<SCsLink>();
    }
    else if (scgContour)
    {
      // SCs cannot record individual nodes in a contour, so it is necessary to create
      // an explicit connection between the contour and the node
      scsElement = std::make_shared<SCsNodeInContour>(
          scgElements, scgElement->GetId(), scgContour->GetId(), scsContour->GetSystemIdtf());
    }
    else
    {
      scsElement = std::make_shared<SCsNode>();
    }
  }
  else if (tag == PAIR || tag == ARC)
  {
    std::shared_ptr<SCgConnector> scgConnector= std::dynamic_pointer_cast<SCgConnector>(scgElement);
    if (scgConnector && scgContour && scgConnector->GetSource() == scgContour && scgConnector->GetTarget()->GetTag() == NODE)
    {
      // If there is an arc to a node in a contour,
      // then this node is in the contour, but not vice versa,
      // so we count multiple arcs from the NodeInContour

      auto scsConnector = std::make_shared<SCsConnector>();
      auto source = scgConnector->GetSource();
      scsElement = std::make_shared<SCsEdgeFromContourToNode>(
          scgElements, scgConnector->GetTarget()->GetId(), scgConnector->GetSource()->GetId(), scsConnector->GetSourceTargetIdtf(source));
    }
    else
    {
      scsElement = std::make_shared<SCsConnector>();
    }
  }
  else if (tag == CONTOUR)
  {
    scsElement = std::make_shared<SCsContour>();
  }
  else
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType, "SCsWriter::CreateAndConvertElementFromSCgElement: Unsupported SCgElement type.");
  }

  if (scsElement)
  {
    SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(scgElement, scsElement);
    scsElement->ConvertFromSCgElement(scgElement);
    if (scsContour)
    {
      scsContour->AddElement(scsElement);
    }
  }

  return scsElement;
}
