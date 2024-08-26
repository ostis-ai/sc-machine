#include "gwf_parser.hpp"

using namespace Constants;

std::unordered_map<std::string, std::shared_ptr<SCgElement>> GWFParser::Parse(std::string const & xmlStr)
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
      if (xmlStrEqual(node->name, STATIC_SECTOR))
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

    std::unordered_map<std::string, std::shared_ptr<SCgElement>> elements;

    ProcessStaticSector(staticSector, elements);

    xmlCleanupParser();

    return elements;
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("GWFParser::Parse: Error in parse " << e.Message());
    xmlCleanupParser();
    return {};
  }
}

void GWFParser::ProcessStaticSector(
    xmlNodePtr staticSector,
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> & elements)
{
  try
  {
    std::vector<std::unordered_map<std::shared_ptr<SCgConnector>, std::pair<std::string, std::string>>> connectors;
    // connectorsList = {connectorMap1, connectorMap2, ...}
    // connectorMap = {connector: (sourceId, targetId)}
    std::vector<std::unordered_map<std::shared_ptr<SCgContour>, std::string>> contours;
    // contoursList = {contourMap1, contourMap2, ...}
    // contourMap = {contour: contourId}

    for (xmlNodePtr child = staticSector->children; child != nullptr; child = child->next)
    {
      if (child->type == XML_ELEMENT_NODE)
      {
        auto const id = GetXmlPropStr(child, ID);
        auto const parent = GetXmlPropStr(child, PARENT);
        auto const idtf = GetXmlPropStr(child, IDTF);
        auto const type = GetXmlPropStr(child, TYPE);
        auto const tag = XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter>(xmlStrdup(child->name)));

        std::shared_ptr<SCgElement> scgElement;

        if (tag == NODE)
        {
          if (HasContent(child))
          {
            scgElement = CreateLink(id, parent, idtf, type, tag, child);
          }
          else
          {
            scgElement = CreateNode(id, parent, idtf, type, tag, child);
          }
        }
        else if (tag == BUS)
        {
          scgElement = CreateBus(id, parent, idtf, type, tag, child);
        }
        else if (tag == CONTOUR)
        {
          scgElement = CreateContour(id, parent, idtf, type, tag, child, contours);
        }
        else if (tag == PAIR || tag == ARC)
        {
          scgElement = CreateConnector(id, parent, idtf, type, tag, child, connectors);
        }
        else
        {
          SC_THROW_EXCEPTION(
              utils::ExceptionParseError, "GWFParser::ProcessStaticSector: Unknown tag  " << tag << " with id " << id);
        }

        elements[id] = scgElement;
      }
    }

    // To correctly process contours(connectors), all elements are first collected, then the contours(connectors) are
    // assigned elements

    for (auto & connector : connectors)
    {
      FillConnector(connector, elements);
    }
    for (auto & contour : contours)
    {
      FillContour(contour, elements);
    }
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR("GWFParser::ProcessStaticSector: Exception caught in process static sector " + std::string(e.what()));
  }
}

std::shared_ptr<SCgNode> GWFParser::CreateNode(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el) const
{
  return std::make_shared<SCgNode>(id, parent, idtf, type, tag);
}

std::shared_ptr<SCgLink> GWFParser::CreateLink(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el) const
{
  for (xmlNodePtr contentChild = el->children; contentChild; contentChild = contentChild->next)
  {
    if (contentChild->type == XML_ELEMENT_NODE && xmlStrcmp(contentChild->name, CONTENT) == 0)
    {
      auto const contentType = GetXmlPropStr(contentChild, TYPE);
      auto const mimeType = GetXmlPropStr(contentChild, MIME_TYPE);
      auto const fileName = GetXmlPropStr(contentChild, FILE_NAME);
      auto contentData = XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter>(xmlNodeGetContent(contentChild)));

      if (!contentType.empty() && std::stoi(contentType) < 4)
      {
        // Content is num or string that don't need conversion
      }
      else if (contentType == "4")
      {
        // Content in binary format (image)
        contentData = ScBase64::Decode(contentData);
      }
      else
      {
        SC_THROW_EXCEPTION(
            utils::ExceptionParseError, "GWFParser::CreateLink: Content type is not supported: " << contentType);
      }

      return std::make_shared<SCgLink>(id, parent, idtf, type, tag, contentType, mimeType, fileName, contentData);
    }
  }

  return nullptr;
}

bool GWFParser::HasContent(xmlNodePtr const node) const
{
  if (!node)
  {
    return false;
  }

  for (xmlNodePtr child = node->children; child; child = child->next)
  {
    if (child->type == XML_ELEMENT_NODE && xmlStrcmp(child->name, CONTENT) == 0)
    {
      return true;
    }
  }
  return false;
}

std::shared_ptr<SCgBus> GWFParser::CreateBus(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el) const
{
  auto const nodeId = GetXmlPropStr(el, OWNER);

  return std::make_shared<SCgBus>(id, parent, idtf, type, tag, nodeId);
}

std::shared_ptr<SCgContour> GWFParser::CreateContour(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el,
    std::vector<std::unordered_map<std::shared_ptr<SCgContour>, std::string>> & contoursList) const
{
  auto contour = std::make_shared<SCgContour>(id, parent, idtf, type, tag);

  std::unordered_map<std::shared_ptr<SCgContour>, std::string> elementMap;
  elementMap[contour] = id;

  contoursList.push_back(elementMap);

  return contour;
}

std::shared_ptr<SCgConnector> GWFParser::CreateConnector(
    std::string const & id,
    std::string const & parent,
    std::string const & idtf,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el,
    std::vector<std::unordered_map<std::shared_ptr<SCgConnector>, std::pair<std::string, std::string>>> &
        connectorsList) const
{
  auto const sourceId = GetXmlPropStr(el, ID_B);
  auto const targetId = GetXmlPropStr(el, ID_E);

  auto connector = std::make_shared<SCgConnector>(id, parent, idtf, type, tag, nullptr, nullptr);

  std::pair<std::string, std::string> sourceAndTarget = {sourceId, targetId};
  std::unordered_map<std::shared_ptr<SCgConnector>, std::pair<std::string, std::string>> elementMap;
  elementMap[connector] = sourceAndTarget;

  connectorsList.push_back(elementMap);

  return connector;
}

void GWFParser::FillConnector(
    std::unordered_map<std::shared_ptr<SCgConnector>, std::pair<std::string, std::string>> const &
        connectorSourceTarget,
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elements)
{
  for (auto const & pair : connectorSourceTarget)
  {
    std::shared_ptr<SCgConnector> const & connector = pair.first;
    std::pair<std::string, std::string> const & sourceAndTarget = pair.second;

    std::string const & sourceId = sourceAndTarget.first;
    std::string const & targetId = sourceAndTarget.second;

    auto sourceIt = elements.find(sourceId);
    auto targetIt = elements.find(targetId);

    std::shared_ptr<SCgElement> sourceEl = (sourceIt != elements.end()) ? sourceIt->second : nullptr;
    std::shared_ptr<SCgElement> targetEl = (targetIt != elements.end()) ? targetIt->second : nullptr;

    if (sourceEl == nullptr)
    {
      SC_THROW_EXCEPTION(
          utils::ExceptionParseError,
          "GWFParser::FillConnector: Source element not found for connector " << connector->GetId());
    }
    if (targetEl == nullptr)
    {
      SC_THROW_EXCEPTION(
          utils::ExceptionParseError,
          "GWFParser::FillConnector: Target element not found for connector " << connector->GetId());
    }

    connector->SetSource(sourceEl);
    connector->SetTarget(targetEl);
  }
}

void GWFParser::FillContour(
    std::unordered_map<std::shared_ptr<SCgContour>, std::string> const & contourAndId,
    std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elements)
{
  for (auto const & pair : contourAndId)
  {
    std::shared_ptr<SCgContour> const & contour = pair.first;
    std::string const & id = pair.second;

    for (auto const & [key, element] : elements)
    {
      if (element->GetParent() == id)
      {
        contour->AddElement(element);
      }
    }
  }
}

std::string GWFParser::XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr) const
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

std::unique_ptr<xmlChar, XmlCharDeleter> GWFParser::GetXmlProp(xmlNodePtr node, std::string const & propName) const
{
  xmlChar * propValue = xmlGetProp(node, BAD_CAST propName.c_str());
  return std::unique_ptr<xmlChar, XmlCharDeleter>(propValue);
}

std::string GWFParser::GetXmlPropStr(xmlNodePtr node, std::string const & propName) const
{
  std::unique_ptr<xmlChar, XmlCharDeleter> const prop = GetXmlProp(node, propName);

  std::string const propStr = XmlCharToString(prop);

  return propStr;
}
