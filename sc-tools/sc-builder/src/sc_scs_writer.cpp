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
  return "@" + prefix + "_" + ReplaceAll(element_id, "-", "_");
}

bool Utils::IsVariable(std::string const & el_type)
{
  return el_type.find("/var/") != std::string::npos;
}

// SCS WRITER CLASS

const std::unordered_map<std::string, std::string> IMAGE_FORMATS = {{".png", "format_png"}};

const std::string N_REL_SYSTEM_IDTF = "nrel_system_identifier";
const std::string N_REL_MAIN_IDTF = "nrel_main_idtf";

std::string ScsWriter::Write(
    std::unordered_map<std::string, std::shared_ptr<ScgElement>> & elementsList,
    std::string const & filePath)
{
  Buffer buffer;

  for (auto & el : elementsList)
  {
    CorrectIdtf(buffer, el.second);
  }

  ProcessElementsList(elementsList, buffer, filePath);

  return buffer.GetValue();
}

void ScsWriter::ProcessElementsList(
    std::unordered_map<std::string, std::shared_ptr<ScgElement>> const & elementsList,
    Buffer & buffer,
    std::string const & filePath)
{
  try
  {
    for (auto & element : elementsList)
    {
      auto const tag = element.second->getTag();
      if (tag == "node" || tag == "bus")
      {
        WriteNode(buffer, element.second, filePath);
        continue;
      }

      auto const parent = element.second->getParent();
      if (parent == "0")
      {
        if (tag == "pair" || tag == "arc")
        {
          std::shared_ptr<Connector> connector = std::dynamic_pointer_cast<Connector>(element.second);
          WriteConnector(buffer, connector);
        }
        else if (tag == "contour")
        {
          std::shared_ptr<Contour> contour = std::dynamic_pointer_cast<Contour>(element.second);
          WriteContour(buffer, contour, 1);
        }
      }   
    }
  }
  catch (std::exception const & e)
  {
    std::cerr << "Exception in process elements: " << e.what() << '\n';
  }
}

void ScsWriter::WriteNode(Buffer & buffer, std::shared_ptr<ScgElement> const & element, std::string const & filePath)
{
  std::shared_ptr<Link> link = std::dynamic_pointer_cast<Link>(element);

  if (link->getContentType() != "0")
  {
    WriteLink(buffer, link, filePath);
  }
  else
  {
    std::string scsElType = ScgToScsElement::GetElement(element->getType(), "NodeTypeSets");

    if (scsElType.empty())
    {
      scsElType = ScgToScsElement::GetElement(
          ScgToScsElement::GetElement(element->getType(), "BackwardNodeTypes"), "NodeTypeSets");
    }
    if (scsElType.empty())
    {
      scsElType = ScgToScsElement::GetElement(element->getType(), "UnsupportedNodeTypeSets");
    }
    if (scsElType.empty())
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + element->getType());
    }

    buffer.Write(element->getIdtf() + "\n  <- " + scsElType + ";;\n\n");
  }
}

void ScsWriter::WriteConnector(Buffer & buffer, std::shared_ptr<Connector> const & connector)
{
  const std::string edgeType = connector->getType();
  bool isUnsupported = false;
  std::string symbol = ScgToScsElement::GetElement(edgeType, "EdgeTypes");

  if (symbol.empty())
  {
    symbol = ScgToScsElement::GetElement(ScgToScsElement::GetElement(edgeType, "BackwardEdgeTypes"), "EdgeTypes");
  }
  if (symbol.empty())
  {
    symbol = ScgToScsElement::GetElement(edgeType, "UnsupportedEdgeTypes");
    isUnsupported = true;
  }
  if (symbol.empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "No matching scs node type for scg node: " + edgeType);
    symbol = "?" + edgeType + "?";
  }

  const std::string alias = Utils::MakeAlias("edge", connector->getId());
  if (isUnsupported)
  {
    buffer.Write(
        alias + " = (" + connector->getSource()->getIdtf() + " => " + connector->getTarget()->getIdtf() + ");;\n");
    buffer.Write(symbol + " -> " + alias + ";;\n");
  }
  else
  {
    buffer.Write(
        alias + " = (" + connector->getSource()->getIdtf() + " " + symbol + " " + connector->getTarget()->getIdtf()
        + ");;\n");
  }

  buffer.Write("\n");
}

void ScsWriter::WriteContour(Buffer & buffer, std::shared_ptr<Contour> const & contour, int tabLevel)
{
  Buffer contourBuffer;

  auto contourElements = contour->getElements();

  for (auto element : contourElements)
  {
    auto const tag = element.second->getTag();

    if (tag == "node" || tag == "bus")
    {
      WriteNodeForContour(buffer, element.second, contour);
    }
    else if (tag == "pair" || tag == "arc")
    {
      std::shared_ptr<Connector> connector = std::dynamic_pointer_cast<Connector>(element.second);

      if (connector->getSource() == contour)
      {
        if(checkForNode(connector->getTarget(), contourElements))
        {
          continue;
        }
      }

      WriteConnector(contourBuffer, connector);
    }
    else if (tag == "contour")
    {
      std::shared_ptr<Contour> contour = std::dynamic_pointer_cast<Contour>(element.second);
      WriteContour(contourBuffer, contour, tabLevel + 1);
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Uncorrect element tag " + tag);
    }
  }
  contourBuffer.AddTabs(tabLevel);
  buffer.Write(contour->getIdtf() + " = [*\n" + contourBuffer.GetValue() + "*];;\n\n");
}

void ScsWriter::WriteLink(Buffer & buffer, std::shared_ptr<Link> const & link, std::string const & filePath)
{
  int const contentType = std::stoi(link->getContentType());

  bool isUrl = false;
  bool isImage = false;
  std::string contentRes;
  std::string imageFormat;

  std::string fileName = link->getFileName();
  std::string contentData = link->getContentData();

  std::filesystem::path basePath = std::filesystem::path(filePath).parent_path();
  std::filesystem::path fullPath = basePath / fileName;

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
    std::ofstream file(fullPath, std::ios::binary);
    if (!file)
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Failed to open file for writing: " + fullPath.string());
    }
    file.write(contentData.data(), contentData.size());
    file.close();

    contentRes = "file://" + fullPath.filename().string();
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
    contentRes = "?Content type is not supported?";
  }

  auto const elIdtf = link->getIdtf();

  if (isUrl)
  {
    buffer.Write(elIdtf + " = \"" + contentRes + "\";;\n");
    if (isImage)
    {
      buffer.Write("@format_edge = (" + elIdtf + " => " + imageFormat + ");;\n");
      buffer.Write("@nrel_format_edge = (nrel_format -> @format_edge);;\n");
    }
  }
  else
  {
    std::string isVar = Utils::IsVariable(link->getType()) ? "_" : "";
    buffer.Write(elIdtf + " = " + isVar + "[" + contentRes + "];;\n");
  }
}

void ScsWriter::WriteNodeForContour(
    Buffer & buffer,
    std::shared_ptr<ScgElement> const & node,
    std::shared_ptr<Contour> const & contour)
{
  size_t counter = 1;
  auto & elements = contour->getElements();

  for (auto it = elements.begin(); it != elements.end();)
  {
    std::shared_ptr<Connector> connector = std::dynamic_pointer_cast<Connector>(it->second);
    if (connector)
    {
      if (connector->getSource() == contour && connector->getTarget() == node)
      {
        counter++;
        it = elements.erase(it);
      }
      else
      {
        ++it;
      }
    }
    else
    {
      ++it;
    }
  }

  auto const edgeName =
      ("@edge_from_contour_" + contour->getId() + "_to_node_" + node->getId() + "_" + std::to_string(counter));
  buffer.Write(edgeName + " = (" + contour->getIdtf() + " -> " + node->getIdtf() + ");;\n\n");
}

void ScsWriter::CorrectIdtf(Buffer & buffer, std::shared_ptr<ScgElement> & element)
{
  std::regex idtfPatternEng("^[0-9a-zA-Z_]*$");
  std::regex idtfPatternRus("^[0-9a-zA-Z_а-яёА-ЯЁ*' ]*$");

  std::string systemIdtf = element->getIdtf();
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
      element->setIdtf(".._el_" + Utils::ReplaceAll(element->getId(), "-", "_"));
    }
    else
    {
      element->setIdtf("..el_" + Utils::ReplaceAll(element->getId(), "-", "_"));
    }
  }
  else if (isVar)
  {
    if (!systemIdtf.empty() && systemIdtf[0] != '_')
    {
      element->setIdtf("_" + Utils::ReplaceAll(systemIdtf, "-", "_"));
    }
  }
  else
  {
    if (!systemIdtf.empty() && systemIdtf[0] == '_')
    {
      element->setIdtf(systemIdtf.substr(1));
      element->setIdtf(Utils::ReplaceAll(element->getIdtf(), "-", "_"));
    }
  }

  if (!mainIdtf.empty())
  {
    std::string output;
    if (mainIdtf[0] == '[')
    {
      output = "\n" + element->getIdtf() + "\n => " + N_REL_SYSTEM_IDTF + ": " + mainIdtf + ";;\n";
    }
    else
    {
      output = "\n" + element->getIdtf() + "\n => " + N_REL_SYSTEM_IDTF + ": [" + mainIdtf + "];;\n";
    }
    buffer.Write(output);
  }
}

bool ScsWriter::checkForNode(
    std::shared_ptr<ScgElement> refElement,
    std::unordered_map<std::string, std::shared_ptr<ScgElement>> contourElements)
{
  for (const auto & element : contourElements)
  {
    if (element.second == refElement)
    {
      return true;
    }
  }

  return false;
}
