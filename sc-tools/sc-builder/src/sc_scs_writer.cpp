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
  std::string line;
  std::string new_value;
  std::string tabs(count * 4, SPACE[0]);

  while (std::getline(iss, line))
  {
    new_value += tabs + line + NEWLINE;
  }

  value = new_value;
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

// void SCsWriter::ProcessElementsList(
//     std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elementsList,
//     Buffer & buffer,
//     std::string const & filePath)
// {
//   try
//   {
//     for (auto const & element : elementsList)
//     {
//       auto const & tag = element.second->GetTag();

//       if (tag == NODE || tag == BUS)
//       {
//         // The type of each node must be specified
//         WriteNode(buffer, element.second, filePath);
//         continue;
//       }

//       auto const & parent = element.second->GetParent();

//       // If an element has a parent, there must be a terminal ancestor element that has no parent
//       // By starting with ancestors, we ensure that all elements are covered

//       if (parent == NO_PARENT)
//       {
//         if (tag == PAIR || tag == ARC)
//         {
//           std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(element.second);
//           WriteConnector(buffer, connector);
//         }
//         else if (tag == CONTOUR)
//         {
//           std::shared_ptr<SCgContour> contour = std::dynamic_pointer_cast<SCgContour>(element.second);
//           WriteContour(buffer, contour, 1);
//         }
//       }
//     }
//   }
//   catch (utils::ScException const & e)
//   {
//     SC_LOG_ERROR("SCSWriter: exception in process elements " << e.Message());
//   }
// }

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
  {
    return GenerateIdtfForUnresolvedCharacters(systemIdtf, elementId, isVar);
  }
  else if (isVar)
  {
    if (systemIdtf[0] != UNDERSCORE[0])
    {
      return CorrectIdtfForVariable(systemIdtf);
    }
  }
  else if (systemIdtf[0] == UNDERSCORE[0])
  {
    return CorrectIdtfForNonVariable(systemIdtf);
  }
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
  std::string correctedIdtf;

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
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & scgElements,
    std::string const & filePath)
{
  try
  {
    Buffer buffer;

    for (auto const & scgElementPair : scgElements)
    {
      auto const & scgElement = scgElementPair.second;

      auto scsElement = SCsElementFactory::CreateElementFromSCgElement(scgElement);

      SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(scgElement, scsElement);

      // If an element has a parent, there must be a terminal ancestor element that has no parent
      // By starting with ancestors, we ensure that all elements are covered

      if (scsElement)
      {
        if (scgElement->GetParent() == NO_PARENT)
        {
          scsElement->ConvertFromSCgElement(scgElement);
          buffer.Write(scsElement->Dump(filePath));
        }
      }
    }

    return buffer.GetValue();
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("SCSWriter: exception in process elements " << e.Message());

    return "";
  }
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

bool SCsWriter::CheckForNode(
    std::shared_ptr<SCgElement> const refElement,
    std::vector<std::shared_ptr<SCgElement>> const & contourElements)
{
  for (auto const & element : contourElements)
  {
    if (element == refElement)
    {
      return true;
    }
  }

  return false;
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
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + scgNodeType);
  }

  type = scsNodeType;
}

std::string SCsNode::Dump(std::string const & filepath) const
{
  Buffer buffer;

  if (!mainIdtf.empty())
  {
    SCsWriter::WriteMainIdtf(buffer, systemIdtf, mainIdtf);
  }

  buffer.Write(systemIdtf + NEWLINE + SPACE + SPACE + SC_EDGE_MAIN_L + SPACE + type + ELEMENT_END + NEWLINE + NEWLINE);

  return buffer.GetValue();
}

// SCsLink

void SCsLink::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  std::shared_ptr<SCgLink> link = std::dynamic_pointer_cast<SCgLink>(scgElement);

  int const contentType = std::stoi(link->GetContentType());

  systemIdtf = link->GetIdtf();
  type = link->GetType();
  fileName = link->GetFileName();

  std::string contentRes;
  std::string const & contentData = link->GetContentData();

  switch (contentType)
  {
  // Content is string
  case 1:
    content = contentData;
    break;
  // Content is int number
  case 2:
    content = DOUBLE_QUOTE + INT64 + contentData + DOUBLE_QUOTE;
    break;
  // Content is float number
  case 3:
    content = DOUBLE_QUOTE + FLOAT + contentData + DOUBLE_QUOTE;
    break;
  // Content is image (.png)
  case 4:
  {
    urlContent = contentData;
    isUrl = true;
    break;
  }
  default:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Content type not supported: " + link->GetId());
  }
}

std::string SCsLink::Dump(std::string const & filePath) const
{
  Buffer buffer;

  if (!mainIdtf.empty())
  {
    SCsWriter::WriteMainIdtf(buffer, systemIdtf, mainIdtf);
  }

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
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "File extension not found: " + fileExtension);
    }

    std::ofstream file(fullPath, std::ios::binary);
    if (!file)
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Failed to open file for writing: " + fullPath.string());
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

  std::string const & edgeType = connector->GetType();
  std::string const & id = connector->GetId();

  isUnsupported = SCgToSCsElement::ConvertSCgEdgeTypeToSCsElementType(edgeType, symbol);

  if (symbol.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + edgeType);
  }

  alias = SCsWriter::MakeAlias(EDGE, id);

  // TO FUNC
  auto const & scgSourceElement = connector->GetSource();
  source = SCsElementFactory::CreateElementFromSCgElement(scgSourceElement);
  source->ConvertFromSCgElement(scgSourceElement);

  auto const & scgTargetElement = connector->GetTarget();
  target = SCsElementFactory::CreateElementFromSCgElement(connector->GetTarget());
  target->ConvertFromSCgElement(scgTargetElement);

  SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(scgElement, target);
  SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(scgElement, source);

  sourceIdtf = source->GetSystemIdtf();
  targetIdtf = target->GetSystemIdtf();
}

std::string SCsConnector::Dump(std::string const & filepath) const
{
  Buffer buffer;

  if (!mainIdtf.empty())
  {
    SCsWriter::WriteMainIdtf(buffer, systemIdtf, mainIdtf);
  }

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
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Invalid SCgElement passed to SCsContour::ConvertFromSCgElement.");
  }

  auto const & scgElements = contour->GetElements();

  for (auto const & scgElement : scgElements)
  {
    std::shared_ptr<SCsElement> scsElement = nullptr;
    std::string const & tag = scgElement->GetTag();

    if (tag == NODE || tag == BUS)
    {
      SCgLink * link = dynamic_cast<SCgLink *>(scgElement.get());
      if (link && link->GetContentType() != NO_CONTENT)
      {
        scsElement = std::make_shared<SCsLink>();
      }
      else
      {
        std::list<std::shared_ptr<SCsElement>> & elements = GetContourElements();
        scsElement = std::make_shared<SCsNodeInContour>(elements);
      }
    }
    else if (tag == PAIR || tag == ARC)
    {
      std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(scgElement);
      if(connector->GetSource() == contour && connector->GetTarget() == scgElement)
      {
        continue;
      }
      scsElement = std::make_shared<SCsConnector>();
    }
    else if (tag == CONTOUR)
    {
      scsElement = std::make_shared<SCsContour>();
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Unsupported SCgElement type.");
    }

    if (scsElement)
    {
      scsElement->ConvertFromSCgElement(scgElement);
      SCsWriter::CorrectorOfSCgIdtf::CorrectIdtf(scgElement, scsElement);
      AddElement(scsElement);
    }
  }
}

void SCsContour::AddElement(std::shared_ptr<SCsElement> const & element)
{
  elements.push_back(element);
}

std::string SCsContour::Dump(std::string const & filepath) const
{
  Buffer buffer;
  Buffer contourBuffer;

  if (!mainIdtf.empty())
  {
    SCsWriter::WriteMainIdtf(buffer, systemIdtf, mainIdtf);
  }

  for (auto const & element : elements)
  {
    contourBuffer.Write(element->Dump(filepath));
  }

  buffer.Write(
      systemIdtf + SPACE + EQUAL + SPACE + OPEN_CONTOUR + NEWLINE + contourBuffer.GetValue() + NEWLINE + CLOSE_CONTOUR
      + ELEMENT_END + NEWLINE);

  return buffer.GetValue();
}

// SCsNode in contour

void SCsNodeInContour::ConvertFromSCgElement(std::shared_ptr<SCgElement> const & scgElement)
{
  multipleArcCounter = 1;

  contourElements.erase(
      std::remove_if(
          contourElements.begin(),
          contourElements.end(),
          [&](std::shared_ptr<SCsElement> const & element)
          {
              auto connector = std::dynamic_pointer_cast<SCgConnector>(element);
              if (connector && connector->GetSource()->GetIdtf() == contourIdtf
                  && connector->GetTarget()->GetIdtf() == systemIdtf)
              {
                  ++multipleArcCounter;
                  return true;
              }
              return false;
          }),
      contourElements.end());

}

std::string SCsNodeInContour::Dump(std::string const & filepath) const
{
  Buffer buffer;

  if (!mainIdtf.empty())
  {
    SCsWriter::WriteMainIdtf(buffer, systemIdtf, mainIdtf);
  }

  auto const edgeName =
      (EDGE_FROM_CONTOUR + UNDERSCORE + contourIdtf + UNDERSCORE + EDGE_TO_NODE + UNDERSCORE + systemIdtf + UNDERSCORE
       + std::to_string(multipleArcCounter));
  buffer.Write(
      edgeName + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + contourIdtf + SPACE + SC_EDGE_MAIN_R + SPACE + systemIdtf
      + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE + NEWLINE);

  return buffer.GetValue();
}

// SCsFactory

std::shared_ptr<SCsElement> SCsElementFactory::CreateElementFromSCgElement(
    std::shared_ptr<SCgElement> const & scgElement)
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
    else
    {
      scsElement = std::make_shared<SCsNode>();
    }
  }
  else if (tag == PAIR || tag == ARC)
  {
    scsElement = std::make_shared<SCsConnector>();
  }
  else if (tag == CONTOUR)
  {
    scsElement = std::make_shared<SCsContour>();
  }
  else
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Unsupported SCgElement type.");
  }

  return scsElement;
}
