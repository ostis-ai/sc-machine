#include "sc_scs_writer.hpp"

// BUFFER CLASS

Buffer::Buffer()
  : value("")
{
}

void Buffer::write(std::string const & s)
{
  value += s;
}

void Buffer::add_tabs(std::size_t count)
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

std::string Buffer::get_value() const
{
  return value;
}

// UTILS CLASS

std::string Utils::replace_all(std::string const & str, std::string const & from, std::string const & to)
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

std::string Utils::make_alias(std::string const & prefix, std::string const & element_id)
{
  return "@" + prefix + "_" + replace_all(element_id, "-", "_");
}

bool Utils::is_variable(std::string const & el_type)
{
  return el_type.find("/var/") != std::string::npos;
}

// SCS WRITER CLASS

std::unordered_map<std::string, std::string> ScsWriter::imageFormats = {{"png", "format_png"}};

const std::string N_REL_SYSTEM_IDTF = "nrel_system_identifier";
const std::string N_REL_MAIN_IDTF = "nrel_main_idtf";

std::string ScsWriter::getElementValue(
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

std::string ScsWriter::write(
    std::vector<std::unordered_map<std::string, std::string>> & elementsList,
    std::string const & filePath)
{
  Buffer buffer;

  ScsWriter::filePath = filePath;

  for (auto & el : elementsList)
  {
    correctIdtf(buffer, el);
  }

  processElementsList(elementsList, buffer, "0", 0);

  return buffer.get_value();
}

void ScsWriter::processElementsList(
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
      std::string const & elParent = ScsWriter::getElementValue(el, "parent");
      std::string const & elTag = ScsWriter::getElementValue(el, "tag");
      std::string const & elId = ScsWriter::getElementValue(el, "id");

      if (elTag == "node" || elTag == "bus")
      {
        if (parent == "0")
        {
          if (elTag == "node")
          {
            writeNode(buffer, el);
          }
          ScsWriter::writtenElements.insert(elId);
          buffer.write("\n");
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
      writeContour(buffer, contour, elementsList, nestedLevel);
      const std::string contourId = ScsWriter::getElementValue(contour, "id");
      ScsWriter::writtenElements.insert(contourId);
      buffer.write("\n");
    }

    writeEdge(buffer, elementsList, edgesList);
  }
  catch (std::exception const & e)
  {
    std::cerr << "Exeption in process elements" << e.what() << '\n';
  }
}

void ScsWriter::writeNode(Buffer & buffer, std::unordered_map<std::string, std::string> const & element)
{
  int const elContentType = std::stoi(ScsWriter::getElementValue(element, "content_type"));

  if (elContentType == 0)
  {
    const std::string scgElType = ScsWriter::getElementValue(element, "type");
    const std::string elIdtf = ScsWriter::getElementValue(element, "idtf");

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

    buffer.write(elIdtf + "\n  <- " + scsElType + ";;\n");
  }
  else
  {
    writeLink(buffer, element);
  }
}

void ScsWriter::writeEdge(
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
      const std::string srcId = ScsWriter::getElementValue(el, "source");
      const std::string trgId = ScsWriter::getElementValue(el, "target");

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
      if (ScsWriter::getElementValue(srcElement, "tag") == "bus")
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

      if (ScsWriter::getElementValue(srcElement, "tag") == "pair"
          || ScsWriter::getElementValue(srcElement, "tag") == "arc")
      {
        srcElement["idtf"] = Utils::make_alias("edge", srcId);
      }
      if (ScsWriter::getElementValue(trgElement, "tag") == "pair"
          || ScsWriter::getElementValue(trgElement, "tag") == "arc")
      {
        trgElement["idtf"] = Utils::make_alias("edge", trgId);
      }

      const std::string edgeType = ScsWriter::getElementValue(el, "type");
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

      const std::string alias = Utils::make_alias("edge", ScsWriter::getElementValue(el, "id"));
      if (isUnsupported)
      {
        buffer.write(
            alias + " = (" + ScsWriter::getElementValue(srcElement, "idtf") + " => "
            + ScsWriter::getElementValue(trgElement, "idtf") + ");;\n");
        buffer.write(symbol + " -> " + alias + ";;\n");
      }
      else
      {
        buffer.write(
            alias + " = (" + ScsWriter::getElementValue(srcElement, "idtf") + " " + symbol + " "
            + ScsWriter::getElementValue(trgElement, "idtf") + ");;\n");
      }

      processedEdge = true;
      writtenElements.insert(ScsWriter::getElementValue(el, "id"));
      el["idtf"] = alias;
    }

    if (!edgesList.empty())
    {
      for (auto & el : edgesList)
      {
        const std::string srcId = ScsWriter::getElementValue(el, "source");
        const std::string trgId = ScsWriter::getElementValue(el, "target");

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

void ScsWriter::writeContour(
    Buffer & buffer,
    std::unordered_map<std::string, std::string> const & element,
    std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
    std::size_t nestedLevel)
{
  const std::string elementId = ScsWriter::getElementValue(element, "id");
  Buffer contourBuff;
  processElementsList(elementsList, contourBuff, elementId, nestedLevel + 1);
  contourBuff.add_tabs(nestedLevel + 1);

  buffer.write(element.at("idtf") + " = [*\n" + contourBuff.get_value() + "\n*];;\n");
}

void ScsWriter::writeLink(Buffer & buffer, std::unordered_map<std::string, std::string> const & element)
{
  const std::string contentData = ScsWriter::getElementValue(element, "content_data");
  int const contentType = std::stoi(ScsWriter::getElementValue(element, "content_type"));
  const std::string elementIdtf = ScsWriter::getElementValue(element, "idtf");
  const std::string elementType = ScsWriter::getElementValue(element, "type");

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
    const std::string contentFileName = ScsWriter::getElementValue(element, "file_name");
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

    if (imageFormats.find(fileExtension) != imageFormats.end())
    {
      imageFormat = imageFormats[fileExtension];
      isImage = true;
    }
  }
  else
  {
    throw std::runtime_error(
        "Content type of this node is not supported: " + ScsWriter::getElementValue(element, "id"));
    contentRes = "?Content type is not supported?";
  }

  if (isUrl)
  {
    if (isImage)
    {
      buffer.write(elementIdtf + " = \"" + contentRes + "\";;\n");
      buffer.write("@format_edge = (" + elementIdtf + " => " + imageFormat + ");;\n");
      buffer.write("@nrel_format_edge = (nrel_format -> @format_edge);;\n");
    }
    else
    {
      buffer.write(elementIdtf + " = \"" + contentRes + "\";;\n");
    }
  }
  else
  {
    std::string isVar = Utils::is_variable(elementType) ? "_" : "";
    buffer.write(elementIdtf + " = " + isVar + "[" + contentRes + "];;\n");
  }
}

void ScsWriter::saveContentToFile(
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
  if (imageFormats.find(fileExtension) != imageFormats.end())
  {
    imageFormat = imageFormats[fileExtension];
    isImage = true;
  }

  if (isUrl)
  {
    if (isImage)
    {
      buffer.write(elementIdtf + " = \"" + contentRes + "\";;\n");
      buffer.write("@format_edge = (" + elementIdtf + " => " + imageFormat + ");;\n");
      buffer.write("@nrel_format_edge = (nrel_format -> @format_edge);;\n");
    }
    else
    {
      buffer.write(elementIdtf + " = \"" + contentRes + "\";;\n");
    }
  }
  else
  {
    std::string isVar = Utils::is_variable(elementType) ? "_" : "";
    buffer.write(elementIdtf + " = " + isVar + "[" + contentRes + "];;\n");
  }
}

void ScsWriter::correctIdtf(Buffer & buffer, std::unordered_map<std::string, std::string> & element)
{
  std::regex idtfPatternEng("^[0-9a-zA-Z_]*$");
  std::regex idtfPatternRus("^[0-9a-zA-Z_а-яёА-ЯЁ*' ]*$");

  std::string systemIdtf = ScsWriter::getElementValue(element, "idtf");
  bool isVar = Utils::is_variable(ScsWriter::getElementValue(element, "type"));

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
      element["idtf"] = ".._el_" + Utils::replace_all(element["id"], "-", "_");
    }
    else
    {
      element["idtf"] = "..el_" + Utils::replace_all(element["id"], "-", "_");
    }
  }
  else if (isVar)
  {
    if (systemIdtf[0] != '_')
    {
      element["idtf"] = "_" + Utils::replace_all(systemIdtf, "-", "_");
    }
  }
  else
  {
    if (systemIdtf[0] == '_')
    {
      element["idtf"] = systemIdtf.substr(1);
      element["idtf"] = Utils::replace_all(element["idtf"], "-", "_");
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
    buffer.write(output);
  }
}