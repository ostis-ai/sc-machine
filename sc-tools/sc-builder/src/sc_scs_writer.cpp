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

// UTILS CLASS

std::string Utils::ReplaceAll(std::string const & str, std::string const & from, std::string const & to)
{
  std::string result = str;
  size_t start_pos = 0;
  while ((start_pos = result.find(from, start_pos)) != std::string::npos)
  {
    result.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return result;
}

std::string Utils::MakeAlias(std::string const & prefix, std::string const & element_id)
{
  return ALIAS_PREFIX + prefix + UNDERSCORE + ReplaceAll(element_id, DASH, UNDERSCORE);
}

bool Utils::IsVariable(std::string const & el_type)
{
  return el_type.find(VAR) != std::string::npos;
}

// SCS WRITER CLASS

std::unordered_map<std::string, std::string> const IMAGE_FORMATS = {{".png", "format_png"}};

std::string SCSWriter::Write(
    std::unordered_map<std::string, std::shared_ptr<SCGElement>> const & elements,
    std::string const & filePath)
{
  Buffer buffer;

  for (auto const & el : elements)
  {
    CorrectIdtf(buffer, el.second);
  }

  ProcessElementsList(elements, buffer, filePath);

  return buffer.GetValue();
}

void SCSWriter::ProcessElementsList(
    std::unordered_map<std::string, std::shared_ptr<SCGElement>> const & elementsList,
    Buffer & buffer,
    std::string const & filePath)
{
  try
  {
    for (auto const & element : elementsList)
    {
      auto const & tag = element.second->getTag();
      if (tag == NODE || tag == BUS)
      {
        WriteNode(buffer, element.second, filePath);
        continue;
      }

      auto const & parent = element.second->getParent();
      if (parent == NO_PARENT)
      {
        if (tag == PAIR || tag == ARC)
        {
          std::shared_ptr<Connector> connector = std::dynamic_pointer_cast<Connector>(element.second);
          WriteConnector(buffer, connector);
        }
        else if (tag == CONTOUR)
        {
          std::shared_ptr<Contour> contour = std::dynamic_pointer_cast<Contour>(element.second);
          WriteContour(buffer, contour, 1);
        }
      }
    }
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("SCSWriter: exception in process elements " << e.Message());
  }
}

void SCSWriter::WriteNode(Buffer & buffer, std::shared_ptr<SCGElement> const & element, std::string const & filePath)
{
  std::shared_ptr<Link> link = std::dynamic_pointer_cast<Link>(element);

  if (link->getContentType() != NO_CONTENT)
  {
    WriteLink(buffer, link, filePath);
  }
  else
  {
    std::string const & nodeType = element->getType();
    std::string scsNodeType = "";

    SCGToSCSElement::ConvertNodeType(nodeType, scsNodeType);

    if (scsNodeType.empty())
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + nodeType);
    }

    buffer.Write(
        element->getIdtf() + NEWLINE + SPACE + SPACE + SC_EDGE_MAIN_L + SPACE + scsNodeType + ELEMENT_END + NEWLINE
        + NEWLINE);
  }
}

void SCSWriter::WriteConnector(Buffer & buffer, std::shared_ptr<Connector> const & connector)
{
  std::string const & edgeType = connector->getType();
  std::string symbol = "";

  bool isUnsupported = SCGToSCSElement::ConvertEdgeType(edgeType, symbol);

  if (symbol.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + edgeType);
  }

  const std::string alias = Utils::MakeAlias(EDGE, connector->getId());
  if (isUnsupported)
  {
    buffer.Write(
        alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + connector->getSource()->getIdtf() + SPACE + SC_EDGE_DCOMMON_R
        + SPACE + connector->getTarget()->getIdtf() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
    buffer.Write(symbol + SPACE + SC_EDGE_MAIN_R + SPACE + alias + ELEMENT_END + NEWLINE);
  }
  else
  {
    buffer.Write(
        alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + connector->getSource()->getIdtf() + SPACE + symbol + SPACE
        + connector->getTarget()->getIdtf() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
  }

  buffer.Write(NEWLINE);
}

void SCSWriter::WriteContour(Buffer & buffer, std::shared_ptr<Contour> const & contour, int tabLevel)
{
  Buffer contourBuffer;

  auto & contourElements = contour->getElements();

  for (auto & element : contourElements)
  {
    auto const & tag = element->getTag();

    if (tag == NODE || tag == BUS)
    {
      WriteNodeForContour(buffer, element, contour);
    }
    else if (tag == PAIR || tag == ARC)
    {
      std::shared_ptr<Connector> connector = std::dynamic_pointer_cast<Connector>(element);

      if (connector->getSource() == contour)
      {
        if (CheckForNode(connector->getTarget(), contourElements))
        {
          continue;
        }
      }

      WriteConnector(contourBuffer, connector);
    }
    else if (tag == CONTOUR)
    {
      std::shared_ptr<Contour> contour = std::dynamic_pointer_cast<Contour>(element);
      WriteContour(contourBuffer, contour, tabLevel + 1);
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Uncorrect element tag " + tag);
    }
  }
  contourBuffer.AddTabs(tabLevel);
  buffer.Write(
      contour->getIdtf() + SPACE + EQUAL + SPACE + OPEN_CONTOUR + NEWLINE + contourBuffer.GetValue() + CLOSE_CONTOUR
      + ELEMENT_END + NEWLINE);
}

void SCSWriter::WriteLink(Buffer & buffer, std::shared_ptr<Link> const & link, std::string const & filePath)
{
  int const contentType = std::stoi(link->getContentType());

  bool isUrl = false;
  bool isImage = false;
  std::string contentRes;
  std::string imageFormat;

  std::string const & fileName = link->getFileName();
  std::string const & contentData = link->getContentData();

  std::filesystem::path const basePath = std::filesystem::path(filePath).parent_path();
  std::filesystem::path const fullPath = basePath / fileName;

  switch (contentType)
  {
  case 1:
    contentRes = contentData;
    break;
  case 2:
    contentRes = DOUBLE_QUOTE + INT64 + contentData + DOUBLE_QUOTE;
    break;
  case 3:
    contentRes = DOUBLE_QUOTE + FLOAT + contentData + DOUBLE_QUOTE;
    break;
  case 4:
  {
    std::ofstream file(fullPath, std::ios::binary);
    if (!file)
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Failed to open file for writing: " + fullPath.string());
    }
    file.write(contentData.data(), contentData.size());
    file.close();

    contentRes = FILE_PREFIX + fullPath.filename().string();
    isUrl = true;

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
    break;
  }
  default:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Content type not supported: " + link->getId());
  }

  auto const & elIdtf = link->getIdtf();

  if (isUrl)
  {
    buffer.Write(elIdtf + SPACE + EQUAL + SPACE + DOUBLE_QUOTE + contentRes + DOUBLE_QUOTE + ELEMENT_END + NEWLINE);
    if (isImage)
    {
      buffer.Write(
          FORMAT_EDGE + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + elIdtf + SPACE + SC_EDGE_DCOMMON_R + SPACE
          + imageFormat + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
      buffer.Write(
          NREL_FORMAT_EDGE + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + NREL_FORMAT + SPACE + SC_EDGE_MAIN_R + SPACE
          + FORMAT_EDGE + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
    }
  }
  else
  {
    std::string isVar = Utils::IsVariable(link->getType()) ? UNDERSCORE : "";
    buffer.Write(
        elIdtf + SPACE + EQUAL + SPACE + isVar + OPEN_BRACKET + contentRes + CLOSE_BRACKET + ELEMENT_END + NEWLINE);
  }

  buffer.Write(NEWLINE);
}

void SCSWriter::WriteNodeForContour(
    Buffer & buffer,
    std::shared_ptr<SCGElement> const & node,
    std::shared_ptr<Contour> const & contour)
{
  size_t counter = 1;
  auto & elements = contour->getElements();

  elements.erase(
      std::remove_if(
          elements.begin(),
          elements.end(),
          [&](std::shared_ptr<SCGElement> & element)
          {
            auto connector = std::dynamic_pointer_cast<Connector>(element);
            if (connector && connector->getSource() == contour && connector->getTarget() == node)
            {
              counter++;
              return true;
            }
            return false;
          }),
      elements.end());

  auto const edgeName =
      (EDGE_FROM_CONTOUR + UNDERSCORE + contour->getId() + UNDERSCORE + EDGE_TO_NODE + UNDERSCORE + node->getId()
       + UNDERSCORE + std::to_string(counter));
  buffer.Write(
      edgeName + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + contour->getIdtf() + SPACE + SC_EDGE_MAIN_R + SPACE
      + node->getIdtf() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE + NEWLINE);
}

void SCSWriter::CorrectIdtf(Buffer & buffer, std::shared_ptr<SCGElement> const & element)
{
  std::regex idtfPatternEng("^[0-9a-zA-Z_]*$");
  std::regex idtfPatternRus("^[0-9a-zA-Z_\\xD0\\x80-\\xD1\\x8F\\xD1\\x90-\\xD1\\x8F\\xD1\\x91\\xD0\\x81*' ]*$");

  auto systemIdtf = element->getIdtf();
  bool isVar = Utils::IsVariable(element->getType());

  std::string mainIdtf;
  if (!std::regex_match(systemIdtf, idtfPatternEng))
  {
    if (std::regex_match(systemIdtf, idtfPatternRus))
    {
      mainIdtf = systemIdtf;
    }
    systemIdtf.clear();
  }

  if (systemIdtf.empty())
  {
    if (isVar)
    {
      element->setIdtf(EL_PREFIX + UNDERSCORE + Utils::ReplaceAll(element->getId(), DASH, UNDERSCORE));
    }
    else
    {
      element->setIdtf(EL_PREFIX + UNDERSCORE + Utils::ReplaceAll(element->getId(), DASH, UNDERSCORE));
    }
  }
  else if (isVar)
  {
    if (!systemIdtf.empty() && systemIdtf[0] != UNDERSCORE[0])
    {
      element->setIdtf(UNDERSCORE + Utils::ReplaceAll(systemIdtf, DASH, UNDERSCORE));
    }
  }
  else
  {
    if (!systemIdtf.empty() && systemIdtf[0] == UNDERSCORE[0])
    {
      element->setIdtf(systemIdtf.substr(1));
      element->setIdtf(Utils::ReplaceAll(element->getIdtf(), DASH, UNDERSCORE));
    }
  }

  if (!mainIdtf.empty())
  {
    std::string output;
    if (mainIdtf[0] == OPEN_BRACKET[0])
    {
      output = NEWLINE + element->getIdtf() + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON
               + SPACE + mainIdtf + ELEMENT_END + NEWLINE;
    }
    else
    {
      output = NEWLINE + element->getIdtf() + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON
               + SPACE + OPEN_BRACKET + mainIdtf + CLOSE_BRACKET + ELEMENT_END + NEWLINE;
    }
    buffer.Write(output + NEWLINE);
  }

  if (element->getTag() == PAIR || element->getTag() == ARC)
  {
    auto const & id = element->getId();
    element->setIdtf(Utils::MakeAlias(EDGE, id));
  }
}

bool SCSWriter::CheckForNode(
    std::shared_ptr<SCGElement> const refElement,
    std::vector<std::shared_ptr<SCGElement>> const & contourElements)
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
