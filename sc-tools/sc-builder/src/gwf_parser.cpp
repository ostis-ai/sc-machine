#include "gwf_parser.hpp"

std::string GwfParser::XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr)
{
  return ptr ? std::string(reinterpret_cast<char const *>(ptr.get())) : "";
}

std::unique_ptr<xmlChar, XmlCharDeleter> GwfParser::GetXmlProp(xmlNodePtr node, char const * propName)
{
  return std::unique_ptr<xmlChar, XmlCharDeleter>(xmlGetProp(node, reinterpret_cast<xmlChar const *>(propName)));
}

void GwfParser::ProcessTag(xmlNodePtr child, std::unordered_map<std::string, std::string> & attributes)
{
  std::string const & tag = attributes["tag"];
  if (tag == "node")
  {
    ProcessNode(child, attributes);
  }
  else if (tag == "pair" || tag == "arc")
  {
    ProcessPair(child, attributes);
  }
  else if (tag == "bus")
  {
    ProcessBus(child, attributes);
  }
  else if (tag == "contour")
  {
  }
  else
  {
    throw std::runtime_error("Tag is not supported: " + tag);
  }
}

void GwfParser::ProcessNode(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes)
{
  for (xmlNodePtr contentChild = node->children; contentChild != nullptr; contentChild = contentChild->next)
  {
    if (contentChild->type == XML_ELEMENT_NODE
        && xmlStrEqual(contentChild->name, reinterpret_cast<xmlChar const *>("content")))
    {
      auto const contentTypeAttr = GetXmlProp(contentChild, "type");
      auto const mimeTypeAttr = GetXmlProp(contentChild, "mime_type");
      auto const fileNameAttr = GetXmlProp(contentChild, "file_name");
      auto const contentData = std::unique_ptr<xmlChar, XmlCharDeleter>(xmlNodeGetContent(contentChild));

      attributes["content_type"] = XmlCharToString(contentTypeAttr);
      attributes["mime_type"] = XmlCharToString(mimeTypeAttr);
      attributes["file_name"] = XmlCharToString(fileNameAttr);

      std::string const & contentType = attributes["content_type"];
      if (!contentType.empty() && std::stoi(contentType) < 4)
      {
        attributes["content_data"] = XmlCharToString(contentData);
      }
      else if (contentType == "4")
      {
        attributes["content_data"] = ScBase64::Decode(XmlCharToString(contentData));
      }
      else
      {
        throw std::runtime_error("Content type is not supported: " + contentType);
      }
    }
  }
}

void GwfParser::ProcessPair(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes)
{
  auto const sourceAttr = GetXmlProp(node, "id_b");
  auto const targetAttr = GetXmlProp(node, "id_e");

  attributes["source"] = XmlCharToString(sourceAttr);
  attributes["target"] = XmlCharToString(targetAttr);
}

void GwfParser::ProcessBus(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes)
{
  auto const node_id = GetXmlProp(node, "owner");

  attributes["node_id"] = XmlCharToString(node_id);
}

void GwfParser::ProcessStaticSector(
    xmlNodePtr staticSector,
    std::vector<std::unordered_map<std::string, std::string>> & elements)
{
  try
  {
    for (xmlNodePtr child = staticSector->children; child != nullptr; child = child->next)
    {
      if (child->type == XML_ELEMENT_NODE)
      {
        std::unordered_map<std::string, std::string> attributes;

        auto const idAttr = GetXmlProp(child, "id");
        auto const parentAttr = GetXmlProp(child, "parent");
        auto const idtfAttr = GetXmlProp(child, "idtf");
        auto const typeAttr = GetXmlProp(child, "type");
        xmlChar const * tagAttr = child->name;

        attributes["id"] = XmlCharToString(idAttr);
        attributes["parent"] = XmlCharToString(parentAttr);
        attributes["idtf"] = XmlCharToString(idtfAttr);
        attributes["type"] = XmlCharToString(typeAttr);
        attributes["tag"] = reinterpret_cast<char const *>(tagAttr);

        ProcessTag(child, attributes);

        elements.push_back(attributes);
      }
    }
  }
  catch (std::exception const & e)
  {
    std::cerr << "Exception caught in process static sector: " << e.what() << '\n';
  }
}

std::vector<std::unordered_map<std::string, std::string>> GwfParser::Parse(std::string const & xmlStr)
{
  try
  {
    xmlInitParser();

    auto xmlTree = std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)>(
        xmlReadMemory(xmlStr.c_str(), xmlStr.size(), "noname.xml", nullptr, 0), xmlFreeDoc);

    if (!xmlTree)
    {
      throw std::runtime_error("Failed to open XML xmlTree.");
    }

    xmlNodePtr rootElement = xmlDocGetRootElement(xmlTree.get());
    xmlNodePtr staticSector = nullptr;

    for (xmlNodePtr node = rootElement->children; node != nullptr; node = node->next)
    {
      if (xmlStrEqual(node->name, reinterpret_cast<xmlChar const *>("staticSector")))
      {
        staticSector = node;
        break;
      }
    }

    if (!staticSector)
    {
      throw std::runtime_error("StaticSector not found in XML xmlTree.");
    }

    if (!staticSector->children)
    {
      return {};
    }

    std::vector<std::unordered_map<std::string, std::string>> elements;
    ProcessStaticSector(staticSector, elements);

    xmlCleanupParser();

    return elements;
  }
  catch (std::exception const & e)
  {
    std::cerr << "Error in parse: " << e.what() << std::endl;
    xmlCleanupParser();
    return {};
  }
}
