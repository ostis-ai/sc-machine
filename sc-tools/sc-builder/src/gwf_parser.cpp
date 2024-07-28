#include "gwf_parser.hpp"

std::string GwfParser::XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr)
{
  if (!ptr)
  {
    return "";
  }

  std::size_t length = xmlStrlen(ptr.get());

  std::string result(length, '\0');
  std::copy(ptr.get(), ptr.get() + length, result.begin());

  return result;
}

std::unique_ptr<xmlChar, XmlCharDeleter> GwfParser::GetXmlProp(xmlNodePtr node, char const * propName)
{
  xmlChar * propValue = xmlGetProp(node, BAD_CAST propName);
  // BAD_CAST macro to cast const char* to const xmlChar* (libxml/xmlstring.h)
  return std::unique_ptr<xmlChar, XmlCharDeleter>(propValue);
}

void GwfParser::ProcessStaticSector(
    xmlNodePtr staticSector,
    std::unordered_map<std::string, std::shared_ptr<ScgElement>> & elements)
{
  try
  {
    std::vector<std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>>> connectorsList;

    for (xmlNodePtr child = staticSector->children; child != nullptr; child = child->next)
    {
      if (child->type == XML_ELEMENT_NODE)
      {
        auto const id = XmlCharToString(GetXmlProp(child, "id"));
        auto const parent = XmlCharToString(GetXmlProp(child, "parent"));
        auto idtf = XmlCharToString(GetXmlProp(child, "idtf"));
        auto const type = XmlCharToString(GetXmlProp(child, "type"));
        auto const tag = XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter>(xmlStrdup(child->name)));

        std::shared_ptr<ScgElement> scgElement;

        if (tag == "node" && HasContent(child))
        {
          scgElement = CreateLink(id, parent, idtf, type, tag, child);
        }
        else if (tag == "node")
        {
          scgElement = CreateNode(id, parent, idtf, type, tag, child);
        }
        else if (tag == "bus")
        {
          scgElement = CreateBus(id, parent, idtf, type, tag, child);
        }
        else if (tag == "contour")
        {
          scgElement = CreateContour(id, parent, idtf, type, tag, child, elements);
        }
        else if (tag == "pair" || tag == "arc")
        {
          scgElement = CreateConnector(id, parent, idtf, type, tag, child, connectorsList);
        }

        elements[id] = scgElement;
      }
    }

    for (auto connector : connectorsList)
    {
      fillConnector(connector, elements);
    }
  }
  catch (std::exception const & e)
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionParseError, std::string("Exception caught in process static sector: ") + e.what());
  }
}

std::unordered_map<std::string, std::shared_ptr<ScgElement>> GwfParser::Parse(std::string const & xmlStr)
{
  try
  {
    xmlInitParser();

    auto xmlTree = std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)>(
        xmlReadMemory(xmlStr.c_str(), xmlStr.size(), "noname.xml", nullptr, 0), xmlFreeDoc);

    if (!xmlTree)
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "Failed to open XML xmlTree.");
    }

    xmlNodePtr rootElement = xmlDocGetRootElement(xmlTree.get());
    xmlNodePtr staticSector = nullptr;

    for (xmlNodePtr node = rootElement->children; node != nullptr; node = node->next)
    {
      if (xmlStrEqual(node->name, (xmlChar const *)"staticSector"))
      {
        staticSector = node;
        break;
      }
    }

    if (!staticSector)
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "StaticSector not found in XML xmlTree.");
    }

    if (!staticSector->children)
    {
      return {};
    }

    std::unordered_map<std::string, std::shared_ptr<ScgElement>> elements;

    ProcessStaticSector(staticSector, elements);

    xmlCleanupParser();

    return elements;
  }
  catch (std::exception const & e)
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, std::string("Error in parse: ") + e.what());
    xmlCleanupParser();
    return {};
  }
}

std::shared_ptr<Node> GwfParser::CreateNode(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr & el)
{
  return std::make_shared<Node>(id, parent, idtf, type, tag);
}

std::shared_ptr<Link> GwfParser::CreateLink(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr & el)
{
  for (xmlNodePtr contentChild = el->children; contentChild; contentChild = contentChild->next)
  {
    if (contentChild->type == XML_ELEMENT_NODE && xmlStrcmp(contentChild->name, (xmlChar const *)"content") == 0)
    {
      auto const contentType = XmlCharToString(GetXmlProp(contentChild, "type"));
      auto const mimeType = XmlCharToString(GetXmlProp(contentChild, "mime_type"));
      auto const fileName = XmlCharToString(GetXmlProp(contentChild, "file_name"));
      auto contentData = XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter>(xmlNodeGetContent(contentChild)));

      if (!contentType.empty() && std::stoi(contentType) < 4)
      {
        // content is num or str
      }
      else if (contentType == "4")
      {
        contentData = ScBase64::Decode(contentData);
      }
      else
      {
        SC_THROW_EXCEPTION(utils::ExceptionParseError, "Content type is not supported: " + contentType);
      }

      return std::make_shared<Link>(id, parent, idtf, type, tag, contentType, mimeType, fileName, contentData);
    }
  }

  return nullptr;
}

bool GwfParser::HasContent(const xmlNodePtr node)
{
  if (!node)
  {
    return false;
  }

  for (xmlNodePtr child = node->children; child; child = child->next)
  {
    if (child->type == XML_ELEMENT_NODE && xmlStrcmp(child->name, (xmlChar const *)"content") == 0)
    {
      return true;
    }
  }
  return false;
}

std::shared_ptr<Bus> GwfParser::CreateBus(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr & el)
{
  auto const nodeId = XmlCharToString(GetXmlProp(el, "owner"));

  return std::make_shared<Bus>(id, parent, idtf, type, tag, nodeId);
}

std::shared_ptr<Contour> GwfParser::CreateContour(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr & el,
    std::unordered_map<std::string, std::shared_ptr<ScgElement>> & elements)
{
  auto contour = std::make_shared<Contour>(id, parent, idtf, type, tag);

  for (auto & [key, element] : elements)
  {
    if (element->getParent() == id)
    {
      contour->addElement(element);
    }
  }

  return contour;
}

std::shared_ptr<Connector> GwfParser::CreateConnector(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr & el,
    std::vector<std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>>> & connectorsList)
{
  auto const sourceId = XmlCharToString(GetXmlProp(el, "id_b"));
  auto const targetId = XmlCharToString(GetXmlProp(el, "id_e"));

  auto connector = std::make_shared<Connector>(id, parent, idtf, type, tag, nullptr, nullptr);
  std::pair<std::string, std::string> sourseAndTarget = {sourceId, targetId};
  std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>> elementMap;
  elementMap[connector] = sourseAndTarget;

  connectorsList.push_back(elementMap);

  return connector;
}

void GwfParser::fillConnector(
    std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>> & connectorSourceTarget,
    std::unordered_map<std::string, std::shared_ptr<ScgElement>> elements)
{
  for (auto & pair : connectorSourceTarget)
  {
    auto connector = pair.first;
    auto sourceAndTarget = pair.second;

    auto sourceId = sourceAndTarget.first;
    auto targetId = sourceAndTarget.second;

    auto sourceIt = elements.find(sourceId);
    auto targetIt = elements.find(targetId);

    std::shared_ptr<ScgElement> sourceEl = (sourceIt != elements.end()) ? sourceIt->second : nullptr;
    std::shared_ptr<ScgElement> targetEl = (targetIt != elements.end()) ? targetIt->second : nullptr;

    if (sourceEl == nullptr)
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "Source element not found for connector " + connector->getId());
    }
    if (targetEl == nullptr)
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "Target element not found for connector " + connector->getId());
    }

    connector->setSource(sourceEl);
    connector->setTarget(targetEl);
  }
}
