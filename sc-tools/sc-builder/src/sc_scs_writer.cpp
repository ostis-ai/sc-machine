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

const std::unordered_map<std::string, std::string> IMAGE_FORMATS = {{"png", "format_png"}};

const std::string N_REL_SYSTEM_IDTF = "nrel_system_identifier";
const std::string N_REL_MAIN_IDTF = "nrel_main_idtf";

std::string ScsWriter::GetElementValue(
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
    std::vector<std::unordered_map<std::string, std::string>> contoursList;
    std::vector<std::unordered_map<std::string, std::string>> edgesList;

    auto processNodeOrBus = [&](std::unordered_map<std::string, std::string> const & element)
    {
      if (parent == "0")
      {
        if (ScsWriter::GetElementValue(element, "tag") == "node")
        {
          WriteNode(buffer, element);
        }
        writtenElements.insert(ScsWriter::GetElementValue(element, "id"));
        buffer.Write("\n");
      }
    };

    auto processContour = [&](std::unordered_map<std::string, std::string> const & contour)
    {
      WriteContour(buffer, contour, elementsList, nestedLevel);
      writtenElements.insert(ScsWriter::GetElementValue(contour, "id"));
      buffer.Write("\n");
    };

    auto processEdge = [&](std::vector<std::unordered_map<std::string, std::string>> & edges)
    {
      WriteEdge(buffer, elementsList, edges);
    };

    for (auto const & element : elementsList)
    {
      std::string const & elementParent = ScsWriter::GetElementValue(element, "parent");
      std::string const & elementType = ScsWriter::GetElementValue(element, "tag");

      if (elementType == "node" || elementType == "bus")
      {
        processNodeOrBus(element);
      }
      else if (elementType == "contour")
      {
        if (elementParent == parent)
        {
          contoursList.push_back(element);
        }
      }
      else if (elementType == "arc" || elementType == "pair")
      {
        if (elementParent == parent)
        {
          edgesList.push_back(element);
        }
      }
    }

    for (auto const & contour : contoursList)
    {
      processContour(contour);
    }

    processEdge(edgesList);
  }
  catch (std::exception const & e)
  {
    std::cerr << "Exception in process elements: " << e.what() << '\n';
  }
}

void ScsWriter::WriteNode(Buffer & buffer, std::unordered_map<std::string, std::string> const & element)
{
  const std::string scgElType = ScsWriter::GetElementValue(element, "type");
  const std::string elIdtf = ScsWriter::GetElementValue(element, "idtf");
  int const elContentType = std::stoi(ScsWriter::GetElementValue(element, "content_type"));

  if (elContentType == 0)
  {
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
  auto SetEdgeAlias = [this](std::unordered_map<std::string, std::string> & element, std::string const & elementId)
  {
    const std::string & tag = this->GetElementValue(element, "tag");
    if (tag == "pair" || tag == "arc")
    {
      element["idtf"] = Utils::m_MakeAlias("edge", elementId);
    }
  };

  auto FindElementById = [&](std::string const & id) -> std::unordered_map<std::string, std::string>
  {
    auto foundElement = std::find_if(
        elementsList.begin(),
        elementsList.end(),
        [&](const auto & element)
        {
          auto it = element.find("id");
          return (it != element.end() && it->second == id);
        });

    if (foundElement != elementsList.end())
    {
      return *foundElement;
    }

    throw std::runtime_error("Can't find element with id: " + id);
  };

  bool processedEdge = true;

  while (!edgesList.empty() && processedEdge)
  {
    processedEdge = false;

    std::vector<std::unordered_map<std::string, std::string>> processList = edgesList;

    edgesList.clear();

    for (auto & el : processList)
    {
      const std::string srcId = ScsWriter::GetElementValue(el, "source");
      const std::string trgId = ScsWriter::GetElementValue(el, "target");

      if (writtenElements.find(srcId) == writtenElements.end() || writtenElements.find(trgId) == writtenElements.end())
      {
        edgesList.push_back(el);
        continue;
      }

      std::unordered_map<std::string, std::string> srcElement = FindElementById(srcId);
      if (ScsWriter::GetElementValue(srcElement, "tag") == "bus")
      {
        std::string node_id = srcElement["node_id"];
        srcElement = FindElementById(node_id);
      }

      std::unordered_map<std::string, std::string> trgElement = FindElementById(trgId);

      SetEdgeAlias(srcElement, srcId);
      SetEdgeAlias(trgElement, trgId);

      const std::string edgeType = ScsWriter::GetElementValue(el, "type");
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

      const std::string alias = Utils::m_MakeAlias("edge", ScsWriter::GetElementValue(el, "id"));
      if (isUnsupported)
      {
        buffer.Write(
            alias + " = (" + ScsWriter::GetElementValue(srcElement, "idtf") + " => "
            + ScsWriter::GetElementValue(trgElement, "idtf") + ");;\n");
        buffer.Write(symbol + " -> " + alias + ";;\n");
      }
      else
      {
        buffer.Write(
            alias + " = (" + ScsWriter::GetElementValue(srcElement, "idtf") + " " + symbol + " "
            + ScsWriter::GetElementValue(trgElement, "idtf") + ");;\n");
      }

      processedEdge = true;
      writtenElements.insert(ScsWriter::GetElementValue(el, "id"));
      el["idtf"] = alias;
    }

    if (!edgesList.empty())
    {
      for (auto & el : edgesList)
      {
        const std::string srcId = ScsWriter::GetElementValue(el, "source");
        const std::string trgId = ScsWriter::GetElementValue(el, "target");

        std::unordered_map<std::string, std::string> srcElement = FindElementById(srcId);

        std::unordered_map<std::string, std::string> trgElement = FindElementById(trgId);
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
  const std::string elementId = ScsWriter::GetElementValue(element, "id");
  Buffer contourBuff;
  ProcessElementsList(elementsList, contourBuff, elementId, nestedLevel + 1);
  contourBuff.AddTabs(nestedLevel + 1);

  buffer.Write(element.at("idtf") + " = [*\n" + contourBuff.GetValue() + "\n*];;\n");
}

void ScsWriter::WriteLink(Buffer & buffer, std::unordered_map<std::string, std::string> const & element)
{
  const std::string contentData = ScsWriter::GetElementValue(element, "content_data");
  int const contentType = std::stoi(ScsWriter::GetElementValue(element, "content_type"));
  const std::string elementIdtf = ScsWriter::GetElementValue(element, "idtf");
  const std::string elementType = ScsWriter::GetElementValue(element, "type");

  bool isUrl = false;
  bool isImage = false;
  std::string contentRes = "";
  std::string imageFormat = "";

  switch (contentType)
  {
  case 1:
    contentRes = contentData;
    break;
  case 2:
    contentRes = "\"int64:" + contentData + "\"";
    break;
  case 3:
    contentRes = "\"float:" + contentData + "\"";
    break;
  case 4:
  {
    const std::string contentFileName = ScsWriter::GetElementValue(element, "file_name");
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

    if (IMAGE_FORMATS.find(fileExtension) != IMAGE_FORMATS.end())
    {
      imageFormat = ScsWriter::GetElementValue(IMAGE_FORMATS, fileExtension);
      isImage = true;
    }
    break;
  }
  default:
    throw std::runtime_error(
        "Content type of this node is not supported: " + ScsWriter::GetElementValue(element, "id"));
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
  std::filesystem::path filePath(ScsWriter::filePath);
  std::filesystem::path fullFilePath = filePath.parent_path() / contentFileName;

  std::ofstream file(fullFilePath, std::ios::binary);
  if (!file.is_open())
  {
    throw std::runtime_error("Failed to open file for writing: " + fullFilePath.string());
  }
  file.write(contentData.data(), contentData.size());
  file.close();

  std::string contentRes = "file://" + fullFilePath.filename().string();

  bool isUrl = true;
  bool isImage = false;

  std::string fileExtension = fullFilePath.extension().string();
  std::string imageFormat = "";

  if (IMAGE_FORMATS.find(fileExtension) != IMAGE_FORMATS.end())
  {
    imageFormat = ScsWriter::GetElementValue(IMAGE_FORMATS, fileExtension);
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

  std::string systemIdtf = ScsWriter::GetElementValue(element, "idtf");
  bool isVar = Utils::m_IsVariable(ScsWriter::GetElementValue(element, "type"));

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
