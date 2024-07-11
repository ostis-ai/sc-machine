#include "gwf_parser.hpp"

std::string GwfParser::XmlCharToString(xmlChar const * xml)
{
  return xml ? reinterpret_cast<char const *>(xml) : "";
}

void GwfParser::ProcessTag(xmlNodePtr child, std::unordered_map<std::string, std::string> & attributes)
{
  if (attributes["tag"] == "node")
  {
    ProcessNode(child, attributes);
  }
  else if (attributes["tag"] == "pair" || attributes["tag"] == "arc")
  {
    ProcessPair(child, attributes);
  }
  else if (attributes["tag"] == "bus")
  {
    ProcessBus(child, attributes);
  }
  else if (attributes["tag"] == "contour")
  {
  }
  else
  {
    throw std::runtime_error("Tag is not supported: " + attributes["tag"]);
  }
}

void GwfParser::ProcessNode(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes)
{
  for (xmlNodePtr contentChild = node->children; contentChild != NULL; contentChild = contentChild->next)
  {
    if (contentChild->type == XML_ELEMENT_NODE && xmlStrEqual(contentChild->name, (xmlChar const *)"content"))
    {
      xmlChar * contentTypeAttr = xmlGetProp(contentChild, (xmlChar const *)"type");
      xmlChar * mimeTypeAttr = xmlGetProp(contentChild, (xmlChar const *)"mime_type");
      xmlChar * fileNameAttr = xmlGetProp(contentChild, (xmlChar const *)"file_name");
      xmlChar * contentData = xmlNodeGetContent(contentChild);

      attributes["content_type"] = XmlCharToString(contentTypeAttr);
      attributes["mime_type"] = XmlCharToString(mimeTypeAttr);
      attributes["file_name"] = XmlCharToString(fileNameAttr);

      if (std::stoi(attributes["content_type"]) < 4)
      {
        attributes["content_data"] = XmlCharToString(contentData);
      }
      else if (attributes["content_type"] == "4")
      {
        attributes["content_data"] = base64::from_base64(XmlCharToString(contentData));
      }
      else
      {
        throw std::runtime_error("Content type is not supported: " + attributes["content_type"]);
      }

      xmlFree(contentTypeAttr);
      xmlFree(mimeTypeAttr);
      xmlFree(fileNameAttr);
      xmlFree(contentData);
    }
  }
}

void GwfParser::ProcessPair(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes)
{
  xmlChar * sourceAttr = xmlGetProp(node, (xmlChar const *)"id_b");
  xmlChar * targetAttr = xmlGetProp(node, (xmlChar const *)"id_e");

  attributes["source"] = XmlCharToString(sourceAttr);
  attributes["target"] = XmlCharToString(targetAttr);

  xmlFree(sourceAttr);
  xmlFree(targetAttr);
}

void GwfParser::ProcessBus(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes)
{
  xmlChar * node_id = xmlGetProp(node, (xmlChar const *)"owner");

  attributes["node_id"] = XmlCharToString(node_id);

  xmlFree(node_id);
}

void GwfParser::ProcessStaticSector(
    xmlNodePtr staticSector,
    std::vector<std::unordered_map<std::string, std::string>> & elements)
{
  try
  {
    for (xmlNodePtr child = staticSector->children; child != NULL; child = child->next)
    {
      if (child->type == XML_ELEMENT_NODE)
      {
        std::unordered_map<std::string, std::string> attributes;

        xmlChar * idAttr = xmlGetProp(child, (xmlChar const *)"id");
        xmlChar * parentAttr = xmlGetProp(child, (xmlChar const *)"parent");
        xmlChar * idtfAttr = xmlGetProp(child, (xmlChar const *)"idtf");
        xmlChar * typeAttr = xmlGetProp(child, (xmlChar const *)"type");
        xmlChar const * tagAttr = child->name;

        attributes["id"] = XmlCharToString(idAttr);
        attributes["parent"] = XmlCharToString(parentAttr);
        attributes["idtf"] = XmlCharToString(idtfAttr);
        attributes["type"] = XmlCharToString(typeAttr);
        attributes["tag"] = XmlCharToString(tagAttr);

        ProcessTag(child, attributes);

        elements.push_back(attributes);

        xmlFree(idAttr);
        xmlFree(parentAttr);
        xmlFree(idtfAttr);
        xmlFree(typeAttr);
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

    xmlDocPtr xmlTree = xmlReadMemory(xmlStr.c_str(), xmlStr.size(), "noname.xml", NULL, 0);

    if (xmlTree == nullptr)
    {
      throw std::runtime_error("Failed to open XML xmlTree.");
    }

    xmlNodePtr rootElement = xmlDocGetRootElement(xmlTree);

    xmlNodePtr staticSector = nullptr;

    for (xmlNodePtr node = rootElement->children; node != NULL; node = node->next)
    {
      if (xmlStrEqual(node->name, (xmlChar const *)"staticSector"))
      {
        staticSector = node;
        break;
      }
    }

    if (staticSector == nullptr)
    {
      throw std::runtime_error("StaticSector not found in XML xmlTree.");
    }
    else if (staticSector->children == nullptr)
    {
      return {};
    }

    std::vector<std::unordered_map<std::string, std::string>> elements;

    ProcessStaticSector(staticSector, elements);

    xmlFreeDoc(xmlTree);
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
