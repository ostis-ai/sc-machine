#include "sc_scs_writer.hpp"

// BUFFER CLASS

Buffer::Buffer()
  : value("")
{
}

void Buffer::Write(std::string const & s)
{
  value += s;
}

void Buffer::AddTabs(std::size_t count)
{
  std::istringstream iss(value);
  std::string line;
  std::string new_value;
  std::string tabs(count * 4, ' ');

  while (std::getline(iss, line))
  {
    new_value += tabs + line + "\n";
  }

  value = new_value;
}

std::string Buffer::GetValue() const
{
  return value;
}

// UTILS CLASS

std::string Utils::m_ReplaceAll(std::string const & str, std::string const & from, std::string const & to)
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

std::string Utils::m_MakeAlias(std::string const & prefix, std::string const & element_id)
{
  return "@" + prefix + "_" + m_ReplaceAll(element_id, "-", "_");
}

bool Utils::m_IsVariable(std::string const & el_type)
{
  return el_type.find("/var/") != std::string::npos;
}

// SCS WRITER CLASS

std::unordered_map<std::string, std::string> ScsWriter::m_imageFormats = {{"png", "format_png"}};

const std::string N_REL_SYSTEM_IDTF = "nrel_system_identifier";
const std::string N_REL_MAIN_IDTF = "nrel_main_idtf";

std::string ScsWriter::m_GetElementValue(
    std::unordered_map<std::string, std::string> const & element,
    std::string const & key)
{
  auto it = element.find(key);
  if (it != element.end())
  {
    return it->second;
  }
  else
  {
    throw std::runtime_error("No value for this key: " + key);
  }
}

std::string ScsWriter::Write(
    std::vector<std::unordered_map<std::string, std::string>> & elementsList,
    std::string const & filePath)
{
  Buffer buffer;

  ScsWriter::filePath = filePath;

  for (auto & el : elementsList)
  {
    CorrectIdtf(buffer, el);
  }

  ProcessElementsList(elementsList, buffer, "0", 0);

  return buffer.GetValue();
}

void ScsWriter::ProcessElementsList(
    std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
    Buffer & buffer,
    std::string const & parent,
    std::size_t nestedLevel)
{
  try
  {
    std::vector<std::unordered_map<std::string, std::string>> contoursList = {};
    std::vector<std::unordered_map<std::string, std::string>> edgesList = {};

    for (auto const & el : elementsList)
    {
      std::string const & elParent = ScsWriter::m_GetElementValue(el, "parent");
      std::string const & elTag = ScsWriter::m_GetElementValue(el, "tag");
      std::string const & elId = ScsWriter::m_GetElementValue(el, "id");

      if (elTag == "node" || elTag == "bus")
      {
        if (parent == "0")
        {
          if (elTag == "node")
          {
            WriteNode(buffer, el);
          }
          ScsWriter::writtenElements.insert(elId);
          buffer.Write("\n");
        }
      }
      else if (elTag == "contour")
      {
        if (elParent == parent)
        {
          contoursList.push_back(el);
        }
      }
      else if (elTag == "arc" || elTag == "pair")
      {
        if (elParent == parent)
        {
          edgesList.push_back(el);
        }
      }
    }
    for (auto const & contour : contoursList)
    {
      WriteContour(buffer, contour, elementsList, nestedLevel);
      const std::string contourId = ScsWriter::m_GetElementValue(contour, "id");
      ScsWriter::writtenElements.insert(contourId);
      buffer.Write("\n");
    }

    WriteEdge(buffer, elementsList, edgesList);
  }
  catch (std::exception const & e)
  {
    std::cerr << "Exeption in process elements" << e.what() << '\n';
  }
}

void ScsWriter::WriteNode(Buffer & buffer, std::unordered_map<std::string, std::string> const & element)
{
  int const elContentType = std::stoi(ScsWriter::m_GetElementValue(element, "content_type"));

  if (elContentType == 0)
  {
    const std::string scgElType = ScsWriter::m_GetElementValue(element, "type");
    const std::string elIdtf = ScsWriter::m_GetElementValue(element, "idtf");

    std::string scsElType = ScgToScsElement::m_GetElement(scgElType, "NodeTypeSets");

    if (scsElType.empty())
    {
      scsElType =
          ScgToScsElement::m_GetElement(ScgToScsElement::m_GetElement(scgElType, "BackwardNodeTypes"), "NodeTypeSets");
    }
    if (scsElType.empty())
    {
      scsElType = ScgToScsElement::m_GetElement(scgElType, "UnsupportedNodeTypeSets");
    }
    if (scsElType.empty())
    {
      throw std::runtime_error("No matching scs node type for scg node: " + scgElType);
      scsElType = "?" + scgElType + "?";
    }

    buffer.Write(elIdtf + "\n  <- " + scsElType + ";;\n");
  }
  else
  {
    WriteLink(buffer, element);
  }
}

void ScsWriter::WriteEdge(
    Buffer & buffer,
    std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
    std::vector<std::unordered_map<std::string, std::string>> & edgesList)
{
  bool processedEdge = true;

  while (!edgesList.empty() && processedEdge)
  {
    processedEdge = false;

    std::vector<std::unordered_map<std::string, std::string>> processList = edgesList;

    edgesList.clear();

    for (auto & el : processList)
    {
      const std::string srcId = ScsWriter::m_GetElementValue(el, "source");
      const std::string trgId = ScsWriter::m_GetElementValue(el, "target");

      if (writtenElements.find(srcId) == writtenElements.end() || writtenElements.find(trgId) == writtenElements.end())
      {
        edgesList.push_back(el);
        continue;
      }

      std::unordered_map<std::string, std::string> srcElement;
      for (auto const & element : elementsList)
      {
        if (element.find("id") != element.end() && element.at("id") == srcId)
        {
          srcElement = element;
          break;
        }
      }
      if (srcElement.empty())
      {
        throw std::runtime_error("Can't find source element with id: " + srcId);
      }
      if (ScsWriter::m_GetElementValue(srcElement, "tag") == "bus")
      {
        std::string node_id = srcElement["node_id"];
        for (auto const & element : elementsList)
        {
          if (element.find("id") != element.end() && element.at("id") == node_id)
          {
            srcElement = element;
            break;
          }
        }
      }

      std::unordered_map<std::string, std::string> trgElement;
      for (auto const & element : elementsList)
      {
        if (element.find("id") != element.end() && element.at("id") == trgId)
        {
          trgElement = element;
          break;
        }
      }
      if (trgElement.empty())
      {
        throw std::runtime_error("Can't find target element with id: " + trgId);
      }

      if (ScsWriter::m_GetElementValue(srcElement, "tag") == "pair"
          || ScsWriter::m_GetElementValue(srcElement, "tag") == "arc")
      {
        srcElement["idtf"] = Utils::m_MakeAlias("edge", srcId);
      }
      if (ScsWriter::m_GetElementValue(trgElement, "tag") == "pair"
          || ScsWriter::m_GetElementValue(trgElement, "tag") == "arc")
      {
        trgElement["idtf"] = Utils::m_MakeAlias("edge", trgId);
      }

      const std::string edgeType = ScsWriter::m_GetElementValue(el, "type");
      bool isUnsupported = false;
      std::string symbol = ScgToScsElement::m_GetElement(edgeType, "EdgeTypes");

      if (symbol.empty())
      {
        symbol =
            ScgToScsElement::m_GetElement(ScgToScsElement::m_GetElement(edgeType, "BackwardEdgeTypes"), "EdgeTypes");
      }
      if (symbol.empty())
      {
        symbol = ScgToScsElement::m_GetElement(edgeType, "UnsupportedEdgeTypes");
        isUnsupported = true;
      }
      if (symbol.empty())
      {
        throw std::runtime_error("No matching scs node type for scg node: " + edgeType);
        symbol = "?" + edgeType + "?";
      }

      const std::string alias = Utils::m_MakeAlias("edge", ScsWriter::m_GetElementValue(el, "id"));
      if (isUnsupported)
      {
        buffer.Write(
            alias + " = (" + ScsWriter::m_GetElementValue(srcElement, "idtf") + " => "
            + ScsWriter::m_GetElementValue(trgElement, "idtf") + ");;\n");
        buffer.Write(symbol + " -> " + alias + ";;\n");
      }
      else
      {
        buffer.Write(
            alias + " = (" + ScsWriter::m_GetElementValue(srcElement, "idtf") + " " + symbol + " "
            + ScsWriter::m_GetElementValue(trgElement, "idtf") + ");;\n");
      }

      processedEdge = true;
      writtenElements.insert(ScsWriter::m_GetElementValue(el, "id"));
      el["idtf"] = alias;
    }

    if (!edgesList.empty())
    {
      for (auto & el : edgesList)
      {
        const std::string srcId = ScsWriter::m_GetElementValue(el, "source");
        const std::string trgId = ScsWriter::m_GetElementValue(el, "target");

        std::unordered_map<std::string, std::string> srcElement;
        for (auto const & element : elementsList)
        {
          if (element.find("id") != element.end() && element.at("id") == srcId)
          {
            srcElement = element;
            break;
          }
        }
        if (srcElement.empty())
        {
          throw std::runtime_error("Can't find source element with id: " + srcId);
        }

        std::unordered_map<std::string, std::string> trgElement;
        for (auto const & element : elementsList)
        {
          if (element.find("id") != element.end() && element.at("id") == trgId)
          {
            trgElement = element;
            break;
          }
        }
        if (trgElement.empty())
        {
          throw std::runtime_error("Can't find target element with id: " + trgId);
        }
      }
    }
  }
}

void ScsWriter::WriteContour(
    Buffer & buffer,
    std::unordered_map<std::string, std::string> const & element,
    std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
    std::size_t nestedLevel)
{
  const std::string elementId = ScsWriter::m_GetElementValue(element, "id");
  Buffer contourBuff;
  ProcessElementsList(elementsList, contourBuff, elementId, nestedLevel + 1);
  contourBuff.AddTabs(nestedLevel + 1);

  buffer.Write(element.at("idtf") + " = [*\n" + contourBuff.GetValue() + "\n*];;\n");
}

void ScsWriter::WriteLink(Buffer & buffer, std::unordered_map<std::string, std::string> const & element)
{
  const std::string contentData = ScsWriter::m_GetElementValue(element, "content_data");
  int const contentType = std::stoi(ScsWriter::m_GetElementValue(element, "content_type"));
  const std::string elementIdtf = ScsWriter::m_GetElementValue(element, "idtf");
  const std::string elementType = ScsWriter::m_GetElementValue(element, "type");

  bool isUrl = false;
  bool isImage = false;
  std::string contentRes = "";
  std::string imageFormat = "";

  if (contentType == 1)
  {
    contentRes = contentData;
  }
  else if (contentType == 2)
  {
    contentRes = "\"int64:" + contentData + "\"";
  }
  else if (contentType == 3)
  {
    contentRes = "\"float:" + contentData + "\"";
  }
  else if (contentType == 4)
  {
    const std::string contentFileName = ScsWriter::m_GetElementValue(element, "file_name");
    contentRes = filePath.substr(0, ScsWriter::filePath.find_last_of("/")) + "/" + contentFileName;

    std::ofstream file(contentRes, std::ios::binary);
    if (!file)
    {
      throw std::runtime_error("Failed to open file for writing: " + contentRes);
    }
    file.write(contentData.data(), contentData.size());
    file.close();

    contentRes = "file://" + contentRes.substr(contentRes.find_last_of("/") + 1);
    isUrl = true;

    std::string fileExtension = contentRes.substr(contentRes.find_last_of(".") + 1);

    if (m_imageFormats.find(fileExtension) != m_imageFormats.end())
    {
      imageFormat = m_imageFormats[fileExtension];
      isImage = true;
    }
  }
  else
  {
    throw std::runtime_error(
        "Content type of this node is not supported: " + ScsWriter::m_GetElementValue(element, "id"));
    contentRes = "?Content type is not supported?";
  }

  if (isUrl)
  {
    if (isImage)
    {
      buffer.Write(elementIdtf + " = \"" + contentRes + "\";;\n");
      buffer.Write("@format_edge = (" + elementIdtf + " => " + imageFormat + ");;\n");
      buffer.Write("@nrel_format_edge = (nrel_format -> @format_edge);;\n");
    }
    else
    {
      buffer.Write(elementIdtf + " = \"" + contentRes + "\";;\n");
    }
  }
  else
  {
    std::string isVar = Utils::m_IsVariable(elementType) ? "_" : "";
    buffer.Write(elementIdtf + " = " + isVar + "[" + contentRes + "];;\n");
  }
}

void ScsWriter::SaveContentToFile(
    Buffer & buffer,
    std::string const & contentData,
    std::string const & contentFileName,
    std::string const & elementIdtf,
    std::string const & elementType)
{
  std::string contentRes = filePath.substr(0, ScsWriter::filePath.find_last_of("/")) + "/" + contentFileName;

  std::ofstream file(contentRes, std::ios::binary);
  if (!file)
  {
    throw std::runtime_error("Failed to open file for writing: " + contentRes);
  }
  file.write(contentData.data(), contentData.size());
  file.close();

  contentRes = "file://" + contentRes.substr(contentRes.find_last_of("/") + 1);
  bool isUrl = true;

  bool isImage = false;
  std::string fileExtension = contentRes.substr(contentRes.find_last_of(".") + 1);
  std::string imageFormat = "";
  if (m_imageFormats.find(fileExtension) != m_imageFormats.end())
  {
    imageFormat = m_imageFormats[fileExtension];
    isImage = true;
  }

  if (isUrl)
  {
    if (isImage)
    {
      buffer.Write(elementIdtf + " = \"" + contentRes + "\";;\n");
      buffer.Write("@format_edge = (" + elementIdtf + " => " + imageFormat + ");;\n");
      buffer.Write("@nrel_format_edge = (nrel_format -> @format_edge);;\n");
    }
    else
    {
      buffer.Write(elementIdtf + " = \"" + contentRes + "\";;\n");
    }
  }
  else
  {
    std::string isVar = Utils::m_IsVariable(elementType) ? "_" : "";
    buffer.Write(elementIdtf + " = " + isVar + "[" + contentRes + "];;\n");
  }
}

void ScsWriter::CorrectIdtf(Buffer & buffer, std::unordered_map<std::string, std::string> & element)
{
  std::regex idtfPatternEng("^[0-9a-zA-Z_]*$");
  std::regex idtfPatternRus("^[0-9a-zA-Z_а-яёА-ЯЁ*' ]*$");

  std::string systemIdtf = ScsWriter::m_GetElementValue(element, "idtf");
  bool isVar = Utils::m_IsVariable(ScsWriter::m_GetElementValue(element, "type"));

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
      element["idtf"] = ".._el_" + Utils::m_ReplaceAll(element["id"], "-", "_");
    }
    else
    {
      element["idtf"] = "..el_" + Utils::m_ReplaceAll(element["id"], "-", "_");
    }
  }
  else if (isVar)
  {
    if (systemIdtf[0] != '_')
    {
      element["idtf"] = "_" + Utils::m_ReplaceAll(systemIdtf, "-", "_");
    }
  }
  else
  {
    if (systemIdtf[0] == '_')
    {
      element["idtf"] = systemIdtf.substr(1);
      element["idtf"] = Utils::m_ReplaceAll(element["idtf"], "-", "_");
    }
  }

  if (!mainIdtf.empty())
  {
    std::string output;
    if (mainIdtf[0] == '[')
    {
      output = "\n" + element["idtf"] + "\n => " + N_REL_SYSTEM_IDTF + ": " + mainIdtf + ";;\n";
    }
    else
    {
      output = "\n" + element["idtf"] + "\n => " + N_REL_SYSTEM_IDTF + ": [" + mainIdtf + "];;\n";
    }
    buffer.Write(output);
  }
}