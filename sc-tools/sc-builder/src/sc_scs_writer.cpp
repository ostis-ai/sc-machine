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

std::string SCsWriter::Write(
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elements,
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

void SCsWriter::ProcessElementsList(
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elementsList,
    Buffer & buffer,
    std::string const & filePath)
{
  try
  {
    for (auto const & element : elementsList)
    {
      auto const & tag = element.second->GetTag();

      if (tag == NODE || tag == BUS)
      {
        // The type of each node must be specified
        WriteNode(buffer, element.second, filePath);
        continue;
      }

      auto const & parent = element.second->GetParent();

      // If an element has a parent, there must be a terminal ancestor element that has no parent
      // By starting with ancestors, we ensure that all elements are covered

      if (parent == NO_PARENT)
      {
        if (tag == PAIR || tag == ARC)
        {
          std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(element.second);
          WriteConnector(buffer, connector);
        }
        else if (tag == CONTOUR)
        {
          std::shared_ptr<SCgContour> contour = std::dynamic_pointer_cast<SCgContour>(element.second);
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

void SCsWriter::WriteNode(Buffer & buffer, std::shared_ptr<SCgElement> const & element, std::string const & filePath)
{
  std::shared_ptr<SCgLink> link = std::dynamic_pointer_cast<SCgLink>(element);

  if (link->GetContentType() != NO_CONTENT)
  {
    WriteLink(buffer, link, filePath);
  }
  else
  {
    std::string const & nodeType = element->GetType();
    std::string scsNodeType = "";

    SCgToSCsElement::ConvertSCgNodeTypeToSCsElementType(nodeType, scsNodeType);

    if (scsNodeType.empty())
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + nodeType);
    }

    buffer.Write(
        element->GetIdtf() + NEWLINE + SPACE + SPACE + SC_EDGE_MAIN_L + SPACE + scsNodeType + ELEMENT_END + NEWLINE
        + NEWLINE);
  }
}

void SCsWriter::WriteConnector(Buffer & buffer, std::shared_ptr<SCgConnector> const & connector)
{
  std::string const & edgeType = connector->GetType();
  std::string symbol = "";

  bool isUnsupported = SCgToSCsElement::ConvertSCgEdgeTypeToSCsElementType(edgeType, symbol);

  if (symbol.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + edgeType);
  }

  const std::string alias = MakeAlias(EDGE, connector->GetId());
  if (isUnsupported)
  {
    buffer.Write(
        alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + connector->GetSource()->GetIdtf() + SPACE + SC_EDGE_DCOMMON_R
        + SPACE + connector->GetTarget()->GetIdtf() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
    buffer.Write(symbol + SPACE + SC_EDGE_MAIN_R + SPACE + alias + ELEMENT_END + NEWLINE);
  }
  else
  {
    buffer.Write(
        alias + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + connector->GetSource()->GetIdtf() + SPACE + symbol + SPACE
        + connector->GetTarget()->GetIdtf() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE);
  }

  buffer.Write(NEWLINE);
}

void SCsWriter::WriteContour(Buffer & buffer, std::shared_ptr<SCgContour> const & contour, int tabLevel)
{
  Buffer contourBuffer;

  auto & contourElements = contour->GetElements();

  for (auto & element : contourElements)
  {
    auto const & tag = element->GetTag();

    if (tag == NODE || tag == BUS)
    {
      // In SCS, single nodes cannot be specified within a contour
      // Therefore, their connections must be explicitly defined using a connector

      WriteNodeForContour(buffer, element, contour);
    }
    else if (tag == PAIR || tag == ARC)
    {
      std::shared_ptr<SCgConnector> connector = std::dynamic_pointer_cast<SCgConnector>(element);

      if (connector->GetSource() == contour)
      {
        if (CheckForNode(connector->GetTarget(), contourElements))
        {
          continue;
        }
      }

      WriteConnector(contourBuffer, connector);
    }
    else if (tag == CONTOUR)
    {
      std::shared_ptr<SCgContour> contour = std::dynamic_pointer_cast<SCgContour>(element);
      WriteContour(contourBuffer, contour, tabLevel + 1);
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Uncorrect element tag " + tag);
    }
  }

  contourBuffer.AddTabs(tabLevel);
  buffer.Write(
      contour->GetIdtf() + SPACE + EQUAL + SPACE + OPEN_CONTOUR + NEWLINE + contourBuffer.GetValue() + CLOSE_CONTOUR
      + ELEMENT_END + NEWLINE);
}

void SCsWriter::WriteLink(Buffer & buffer, std::shared_ptr<SCgLink> const & link, std::string const & filePath)
{
  int const contentType = std::stoi(link->GetContentType());

  bool isUrl = false;
  bool isImage = false;
  std::string contentRes;
  std::string imageFormat;

  std::string const & fileName = link->GetFileName();
  std::string const & contentData = link->GetContentData();

  std::filesystem::path const basePath = std::filesystem::path(filePath).parent_path();
  std::filesystem::path const fullPath = basePath / fileName;

  switch (contentType)
  {
  // Content is string
  case 1:
    contentRes = contentData;
    break;
  // Content is int number
  case 2:
    contentRes = DOUBLE_QUOTE + INT64 + contentData + DOUBLE_QUOTE;
    break;
  // Content is float number
  case 3:
    contentRes = DOUBLE_QUOTE + FLOAT + contentData + DOUBLE_QUOTE;
    break;
  // Content is image (.png)
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
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Content type not supported: " + link->GetId());
  }

  auto const & elIdtf = link->GetIdtf();

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
    std::string isVar = IsVariable(link->GetType()) ? UNDERSCORE : "";
    buffer.Write(
        elIdtf + SPACE + EQUAL + SPACE + isVar + OPEN_BRACKET + contentRes + CLOSE_BRACKET + ELEMENT_END + NEWLINE);
  }

  buffer.Write(NEWLINE);
}

void SCsWriter::WriteNodeForContour(
    Buffer & buffer,
    std::shared_ptr<SCgElement> const & node,
    std::shared_ptr<SCgContour> const & contour)
{
  size_t counter = 1;
  // Counter of multiple arcs belonging to a node to a contour

  auto & elements = contour->GetElements();

  elements.erase(
      std::remove_if(
          elements.begin(),
          elements.end(),
          [&](std::shared_ptr<SCgElement> & element)
          {
            auto connector = std::dynamic_pointer_cast<SCgConnector>(element);
            if (connector && connector->GetSource() == contour && connector->GetTarget() == node)
            {
              counter++;
              return true;
            }
            return false;
          }),
      elements.end());

  auto const edgeName =
      (EDGE_FROM_CONTOUR + UNDERSCORE + contour->GetId() + UNDERSCORE + EDGE_TO_NODE + UNDERSCORE + node->GetId()
       + UNDERSCORE + std::to_string(counter));
  buffer.Write(
      edgeName + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS + contour->GetIdtf() + SPACE + SC_EDGE_MAIN_R + SPACE
      + node->GetIdtf() + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE + NEWLINE);
}

bool SCsWriter::IsRussianIdtf(const std::string& idtf) {
    std::regex idtfPatternRus("^[0-9a-zA-Z_\\xD0\\x80-\\xD1\\x8F\\xD1\\x90-\\xD1\\x8F\\xD1\\x91\\xD0\\x81*' ]*$");
    return std::regex_match(idtf, idtfPatternRus);
}

bool SCsWriter::IsEnglishIdtf(const std::string& idtf) {
    std::regex idtfPatternEng("^[0-9a-zA-Z_]*$");
    return std::regex_match(idtf, idtfPatternEng);
}

std::string SCsWriter::GenerateCorrectedIdtf(std::string & systemIdtf, std::string & elementId, bool isVar) {
    if (systemIdtf.empty()) {
        return GenerateIdtfForUnresolvedCharacters(systemIdtf, elementId, isVar);
    } 
    else if (isVar) {
        if (systemIdtf[0] != UNDERSCORE[0]) {
            return CorrectIdtfForVariable(systemIdtf);
        }
    } 
    else if (systemIdtf[0] == UNDERSCORE[0]) {
        return CorrectIdtfForNonVariable(systemIdtf);
    }
    return systemIdtf;
}

std::string SCsWriter::GenerateIdtfForUnresolvedCharacters(std::string & systemIdtf, const std::string & elementId, bool isVar) {
    std::string prefix = isVar ? EL_VAR_PREFIX : EL_PREFIX;
    systemIdtf = prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(elementId, DASH, UNDERSCORE);
    return systemIdtf;
}

std::string SCsWriter::CorrectIdtfForVariable(std::string & systemIdtf) {
    return UNDERSCORE + utils::StringUtils::ReplaceAll(systemIdtf, DASH, UNDERSCORE);
}

std::string SCsWriter::CorrectIdtfForNonVariable(std::string & systemIdtf) {
    return utils::StringUtils::ReplaceAll(systemIdtf.substr(1), DASH, UNDERSCORE);
}

void SCsWriter::WriteMainIdtf(Buffer & buffer, const std::string & systemIdtf, const std::string & mainIdtf) {
    std::string output;
    if (mainIdtf[0] == OPEN_BRACKET[0]) {
        output = NEWLINE + systemIdtf + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON
                 + SPACE + mainIdtf + ELEMENT_END + NEWLINE;
    } else {
        output = NEWLINE + systemIdtf + NEWLINE + SPACE + SC_EDGE_DCOMMON_R + SPACE + N_REL_MAIN_IDTF + COLON
                 + SPACE + OPEN_BRACKET + mainIdtf + CLOSE_BRACKET + ELEMENT_END + NEWLINE;
    }
    buffer.Write(output + NEWLINE);
}

void SCsWriter::ProcessSpecialTags(std::shared_ptr<SCgElement> const & element) {
    if (element->GetTag() == PAIR || element->GetTag() == ARC) {
        auto const & id = element->GetId();
        element->SetIdtf(MakeAlias(EDGE, id));
    }
}

void SCsWriter::CorrectIdtf(Buffer & buffer, std::shared_ptr<SCgElement> const & element) {
    std::string systemIdtf = element->GetIdtf();
    bool isVar = IsVariable(element->GetType());

    std::string mainIdtf;
    if (!IsEnglishIdtf(systemIdtf)) {
        if (IsRussianIdtf(systemIdtf)) {
            mainIdtf = systemIdtf;
        }
        systemIdtf.clear();
    }

    std::string id = element->GetId();

    systemIdtf = GenerateCorrectedIdtf(systemIdtf, id, isVar);
    element->SetIdtf(systemIdtf);

    if (!mainIdtf.empty()) {
        WriteMainIdtf(buffer, systemIdtf, mainIdtf);
    }

    ProcessSpecialTags(element);
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
}
