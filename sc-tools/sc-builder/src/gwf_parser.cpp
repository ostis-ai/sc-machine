#include "gwf_parser.hpp"

std::unordered_map<std::string, std::shared_ptr<SCGElement>> GWFParser::Parse(std::string const & xmlStr)
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

    std::unordered_map<std::string, std::shared_ptr<SCGElement>> elements;

    ProcessStaticSector(staticSector, elements);

    xmlCleanupParser();

    return elements;
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("SCSWriter: Error in parse " + std::string(e.what()));
    xmlCleanupParser();
    return {};
  }
}

void GWFParser::ProcessStaticSector(
    xmlNodePtr staticSector,
    std::unordered_map<std::string, std::shared_ptr<SCGElement>> & elements)
{
  try
  {
    std::vector<std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>>> connectorsList;
    // connectorsList = {connectorMap1, connectorMap2, ...}
    // connectorMap = {connector: (sourceId, targetId)}
    std::vector<std::unordered_map<std::shared_ptr<Contour>, std::string>> contoursList;
    // contoursList = {contourMap1, contourMap2, ...}
    // contourMap = {contour: contourId}

    for (xmlNodePtr child = staticSector->children; child != nullptr; child = child->next)
    {
      if (child->type == XML_ELEMENT_NODE)
      {
        auto const id = XmlCharToString(GetXmlProp(child, "id"));
        auto const parent = XmlCharToString(GetXmlProp(child, "parent"));
        auto idtf = XmlCharToString(GetXmlProp(child, "idtf"));
        auto const type = XmlCharToString(GetXmlProp(child, "type"));
        auto const tag = XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter>(xmlStrdup(child->name)));

        std::shared_ptr<SCGElement> scgElement;

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
          scgElement = CreateContour(id, parent, idtf, type, tag, child, contoursList);
        }
        else if (tag == "pair" || tag == "arc")
        {
          scgElement = CreateConnector(id, parent, idtf, type, tag, child, connectorsList);
        }

        elements[id] = scgElement;
      }
    }

    // To correctly process contours(connectors), all elements are first collected, then the contours(connectors) are
    // assigned elements

    for (auto & connector : connectorsList)
    {
      FillConnector(connector, elements);
    }
    for (auto & countour : contoursList)
    {
      FillContour(countour, elements);
    }
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("SCSWriter: Exception caught in process static sector " + std::string(e.what()));
  }
}

std::shared_ptr<Node> GWFParser::CreateNode(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr & el)
{
  return std::make_shared<Node>(id, parent, idtf, type, tag);
}

std::shared_ptr<Link> GWFParser::CreateLink(
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
        // Content is num or string that don't need conversion
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

bool GWFParser::HasContent(const xmlNodePtr node)
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

std::shared_ptr<Bus> GWFParser::CreateBus(
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

std::shared_ptr<Contour> GWFParser::CreateContour(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr & el,
    std::vector<std::unordered_map<std::shared_ptr<Contour>, std::string>> & contoursList)
{
  auto contour = std::make_shared<Contour>(id, parent, idtf, type, tag);

  std::unordered_map<std::shared_ptr<Contour>, std::string> elementMap;
  elementMap[contour] = id;

  contoursList.push_back(elementMap);

  return contour;
}

std::shared_ptr<Connector> GWFParser::CreateConnector(
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

void GWFParser::FillConnector(
    std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>> & connectorSourceTarget,
    std::unordered_map<std::string, std::shared_ptr<SCGElement>> elements)
{
  for (auto & pair : connectorSourceTarget)
  {
    std::shared_ptr<Connector> const & connector = pair.first;
    std::pair<std::string, std::string> & sourceAndTarget = pair.second;

    std::string const & sourceId = sourceAndTarget.first;
    std::string const & targetId = sourceAndTarget.second;

    auto sourceIt = elements.find(sourceId);
    auto targetIt = elements.find(targetId);

    std::shared_ptr<SCGElement> sourceEl = (sourceIt != elements.end()) ? sourceIt->second : nullptr;
    std::shared_ptr<SCGElement> targetEl = (targetIt != elements.end()) ? targetIt->second : nullptr;

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

void GWFParser::FillContour(
    std::unordered_map<std::shared_ptr<Contour>, std::string> & contourAndId,
    std::unordered_map<std::string, std::shared_ptr<SCGElement>> elements)
{
  for (auto & pair : contourAndId)
  {
    std::shared_ptr<Contour> const & contour = pair.first;
    std::string const & id = pair.second;

    for (auto & [key, element] : elements)
    {
      if (element->getParent() == id)
      {
        contour->addElement(element);
      }
    }
  }
}

std::string GWFParser::XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr)
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

std::unique_ptr<xmlChar, XmlCharDeleter> GWFParser::GetXmlProp(xmlNodePtr node, char const * propName)
{
  xmlChar * propValue = xmlGetProp(node, BAD_CAST propName);
  // BAD_CAST macro to cast const char* to const xmlChar* (libxml/xmlstring.h)
  return std::unique_ptr<xmlChar, XmlCharDeleter>(propValue);
}
