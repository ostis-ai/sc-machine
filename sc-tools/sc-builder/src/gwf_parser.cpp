/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "gwf_parser.hpp"

#include <iostream>

#include <sc-memory/utils/sc_base64.hpp>
#include <sc-memory/sc_debug.hpp>

#include "sc_scg_element.hpp"

using namespace Constants;

void XmlCharDeleter::operator()(xmlChar * ptr) const
{
  if (ptr != nullptr)
    xmlFree(ptr);
}

void GWFParser::Parse(std::string const & xmlStr, SCgElements & elements)
{
  xmlInitParser();

  auto xmlTree = std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)>(
      xmlReadMemory(xmlStr.c_str(), xmlStr.size(), "noname.xml", nullptr, 0), xmlFreeDoc);
  if (xmlTree == nullptr)
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "Failed to open XML xmlTree.");

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

  if (staticSector == nullptr)
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "StaticSector not found in XML xmlTree.");

  if (staticSector->children == nullptr)
    return;

  ProcessStaticSector(staticSector, elements);
  xmlCleanupParser();
}

void GWFParser::ProcessStaticSector(xmlNodePtr staticSector, SCgElements & elementsWithoutParents)
{
  SCgElements allElements;
  SCgConnectors connectors;  // connectors = {connector: (sourceId, targetId)}
  SCgContours contours;

  for (xmlNodePtr child = staticSector->children; child != nullptr; child = child->next)
  {
    if (child->type == XML_ELEMENT_NODE)
    {
      auto const id = GetXmlPropStr(child, ID);
      auto const parent = GetXmlPropStr(child, PARENT);
      auto const identifier = GetXmlPropStr(child, IDENTIFIER);
      auto const type = GetXmlPropStr(child, TYPE);
      auto const tag = XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter>(xmlStrdup(child->name)));

      SCgElementPtr scgElement;
      if (tag == NODE)
      {
        if (HasContent(child))
          scgElement = CreateLink(id, parent, identifier, type, tag, child);
        else
          scgElement = CreateNode(id, parent, identifier, type, tag, child);
      }
      else if (tag == BUS)
        scgElement = CreateBus(id, parent, identifier, type, tag, child);
      else if (tag == CONTOUR)
        scgElement = CreateContour(id, parent, identifier, type, tag, child);
      else if (tag == PAIR || tag == ARC)
        scgElement = CreateConnector(id, parent, identifier, type, tag, child, connectors);
      else
        SC_THROW_EXCEPTION(
            utils::ExceptionParseError, "GWFParser::ProcessStaticSector: Unknown tag  " << tag << " with id " << id);

      if (parent == NO_PARENT)
        elementsWithoutParents[id] = scgElement;
      else
      {
        if (!contours.count(parent))
          contours.insert({parent, {}});

        contours[parent].insert({id, scgElement});
      }

      allElements[id] = scgElement;
    }
  }

  // To correctly process contours(connectors), all elements are first collected, then the contours(connectors) are
  // assigned elements
  FillConnectors(connectors, allElements);
  FillContours(contours, allElements);
}

std::shared_ptr<SCgNode> GWFParser::CreateNode(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el) const
{
  return std::make_shared<SCgNode>(id, parent, identifier, type, tag);
}

std::shared_ptr<SCgLink> GWFParser::CreateLink(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
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
        SC_THROW_EXCEPTION(
            utils::ExceptionParseError, "GWFParser::CreateLink: Content type is not supported: " << contentType);

      return std::make_shared<SCgLink>(id, parent, identifier, type, tag, contentType, mimeType, fileName, contentData);
    }
  }

  return nullptr;
}

bool GWFParser::HasContent(xmlNodePtr const node) const
{
  for (xmlNodePtr child = node->children; child; child = child->next)
  {
    if (child->type == XML_ELEMENT_NODE && xmlStrcmp(child->name, CONTENT) == 0)
      return true;
  }
  return false;
}

std::shared_ptr<SCgBus> GWFParser::CreateBus(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el) const
{
  auto const nodeId = GetXmlPropStr(el, OWNER);
  return std::make_shared<SCgBus>(id, parent, identifier, type, tag, nodeId);
}

std::shared_ptr<SCgContour> GWFParser::CreateContour(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el) const
{
  return std::make_shared<SCgContour>(id, parent, identifier, type, tag);
}

std::shared_ptr<SCgConnector> GWFParser::CreateConnector(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag,
    xmlNodePtr el,
    SCgConnectors & connectors) const
{
  std::string const & sourceId = GetXmlPropStr(el, ID_B);
  std::string const & targetId = GetXmlPropStr(el, ID_E);

  auto connector = std::make_shared<SCgConnector>(id, parent, identifier, type, tag, nullptr, nullptr);
  connectors.insert({connector, {sourceId, targetId}});
  return connector;
}

void GWFParser::FillConnectors(SCgConnectors const & connectors, SCgElements const & elements)
{
  auto const & ResolveBus = [&](SCgElementPtr element) -> SCgElementPtr
  {
    if (element->GetTag() == BUS)
    {
      if (auto const & bus = std::dynamic_pointer_cast<SCgBus>(element))
        element = elements.at(bus->GetNodeId());
    }

    return element;
  };

  for (auto const & [connector, incidentElements] : connectors)
  {
    std::string const & sourceId = incidentElements.first;
    std::string const & targetId = incidentElements.second;

    auto sourceIt = elements.find(sourceId);
    auto targetIt = elements.find(targetId);

    SCgElementPtr const & source = sourceIt != elements.cend() ? ResolveBus(sourceIt->second) : nullptr;
    SCgElementPtr const & target = targetIt != elements.cend() ? ResolveBus(targetIt->second) : nullptr;

    if (source == nullptr)
      SC_THROW_EXCEPTION(
          utils::ExceptionParseError,
          "GWFParser::FillConnector: Source element not found for connector `" << connector->GetId() << "`.");
    if (target == nullptr)
      SC_THROW_EXCEPTION(
          utils::ExceptionParseError,
          "GWFParser::FillConnector: Target element not found for connector `" << connector->GetId() << "`.");

    connector->SetSource(source);
    connector->SetTarget(target);
  }
}

void GWFParser::FillContours(SCgContours const & contours, SCgElements const & allElements)
{
  for (auto const & [contourId, elements] : contours)
  {
    auto const & it = allElements.find(contourId);
    if (it == allElements.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionParseError, "GWFParser::FillContours: Invalid contour id `" << contourId << "`.");

    auto const & contour = std::dynamic_pointer_cast<SCgContour>(it->second);
    contour->SetElements(elements);
  }
}

std::string GWFParser::XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr) const
{
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
  if (prop == nullptr)
    SC_THROW_EXCEPTION(
        utils::ExceptionParseError,
        "GWFParser::GetXmlPropStr: Gwf-element doesn't have property name `" << propName << "`.");

  std::string const propStr = XmlCharToString(prop);
  return propStr;
}
