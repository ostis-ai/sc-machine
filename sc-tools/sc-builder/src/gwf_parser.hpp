#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <memory>
#include <list>

#include "gwf_translator_const.hpp"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include <sc-memory/utils/sc_base64.hpp>
#include <sc-memory/sc_debug.hpp>

class XmlCharDeleter
{
public:
  void operator()(xmlChar * ptr) const
  {
    if (ptr)
    {
      xmlFree(ptr);
    }
  }
};

class SCgElement
{
public:
  SCgElement(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag)
    : id(id)
    , parent(parent)
    , idtf(idtf)
    , type(type)
    , tag(tag)
  {
  }

  virtual ~SCgElement() = default;

  std::string const & GetId() const
  {
    return id;
  }

  std::string const & GetParent() const
  {
    return parent;
  }

  std::string const & GetIdtf() const
  {
    return idtf;
  }

  std::string const & GetType() const
  {
    return type;
  }

  std::string const & GetTag() const
  {
    return tag;
  }

  void SetIdtf(std::string const & newIdtf)
  {
    idtf = newIdtf;
  }

private:
  std::string id;
  std::string parent;
  std::string idtf;
  std::string type;
  std::string tag;
};

class SCgNode : public SCgElement
{
public:
  SCgNode(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag)
    : SCgElement(id, parent, idtf, type, tag)
  {
  }
};

class SCgLink : public SCgNode
{
public:
  SCgLink(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      std::string const & contentType,
      std::string const & mimeType,
      std::string const & fileName,
      std::string const & contentData)
    : SCgNode(id, parent, idtf, type, tag)
    , contentType(contentType)
    , mimeType(mimeType)
    , fileName(fileName)
    , contentData(contentData)
  {
  }

  std::string const & GetContentType() const
  {
    return contentType;
  }

  std::string const & GetMimeType() const
  {
    return mimeType;
  }

  std::string const & GetFileName() const
  {
    return fileName;
  }

  std::string const & GetContentData() const
  {
    return contentData;
  }

private:
  std::string contentType;
  std::string mimeType;
  std::string fileName;
  std::string contentData;
};

class SCgBus : public SCgNode
{
public:
  SCgBus(std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      std::string const & nodeId)
    : SCgNode(id, parent, idtf, type, tag)
    , nodeId(nodeId)
  {
  }

  std::string const & GetNodeId() const
  {
    return nodeId;
  }

private:
  std::string nodeId;
};

class SCgContour : public SCgNode
{
public:
  SCgContour(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag)
    : SCgNode(id, parent, idtf, type, tag)
  {
  }

  void AddElement(std::shared_ptr<SCgElement> const & element)
  {
    elements.push_back(element);
  }

  std::vector<std::shared_ptr<SCgElement>> & GetElements()
  {
    return elements;
  }

private:
  std::vector<std::shared_ptr<SCgElement>> elements;
};

class SCgConnector : public SCgElement
{
public:
  SCgConnector(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      std::shared_ptr<SCgElement> sourceEl,
      std::shared_ptr<SCgElement> targetEl)
    : SCgElement(id, parent, idtf, type, tag)
    , source(sourceEl)
    , target(targetEl)
  {
  }

  std::shared_ptr<SCgElement> GetSource() const
  {
    return source;
  }

  std::shared_ptr<SCgElement> GetTarget() const
  {
    return target;
  }

  void SetSource(std::shared_ptr<SCgElement> const & sourceEl)
  {
    source = sourceEl;
  }

  void SetTarget(std::shared_ptr<SCgElement> const & targetEl)
  {
    target = targetEl;
  }

private:
  std::shared_ptr<SCgElement> source;
  std::shared_ptr<SCgElement> target;
};

class GWFParser
{
public:
  std::unordered_map<std::string, std::shared_ptr<SCgElement>> Parse(std::string const & xmlStr);

private:
  xmlChar const * const STATIC_SECTOR = (xmlChar const *)"staticSector";
  xmlChar const * const CONTENT = (xmlChar const *)"content";

  std::shared_ptr<SCgNode> CreateNode(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr el) const;

  std::shared_ptr<SCgLink> CreateLink(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr el) const;

  bool HasContent(xmlNodePtr const node) const;

  std::shared_ptr<SCgBus> CreateBus(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr el) const;

  std::shared_ptr<SCgContour> CreateContour(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      xmlNodePtr el,
      std::vector<std::unordered_map<std::shared_ptr<SCgContour>, std::string>> & contoursList) const;

  std::shared_ptr<SCgConnector> CreateConnector(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr el,
      std::vector<std::unordered_map<std::shared_ptr<SCgConnector>, std::pair<std::string, std::string>>> & connectorsList)
      const;

  void FillConnector(
      std::unordered_map<std::shared_ptr<SCgConnector>, std::pair<std::string, std::string>> const & connectorSourceTarget,
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elements);

  void FillContour(
      std::unordered_map<std::shared_ptr<SCgContour>, std::string> const & contourAndId,
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elements);

  void ProcessStaticSector(
      xmlNodePtr staticSector,
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> & elements);

  std::string XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr) const;
  std::unique_ptr<xmlChar, XmlCharDeleter> GetXmlProp(xmlNodePtr node, std::string const & propName) const;
  std::string GetXmlPropStr(xmlNodePtr node, std::string const & propName) const;
};
