#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <memory>
#include <list>

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

class ScgElement
{
public:
  ScgElement(
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

  virtual ~ScgElement() = default;

  std::string const & getId() const
  {
    return id;
  }

  std::string const & getParent() const
  {
    return parent;
  }

  std::string const & getIdtf() const
  {
    return idtf;
  }

  std::string const & getType() const
  {
    return type;
  }

  std::string const & getTag() const
  {
    return tag;
  }

  void setIdtf(std::string const & newIdtf)
  {
    idtf = newIdtf;
  }

protected:
  std::string id;
  std::string parent;
  std::string idtf;
  std::string type;
  std::string tag;
};

class Node : public ScgElement
{
public:
  Node(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag)
    : ScgElement(id, parent, idtf, type, tag)
  {
  }
};

class Link : public Node
{
public:
  Link(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      std::string const & contentType,
      std::string const & mimeType,
      std::string const & fileName,
      std::string const & contentData)
    : Node(id, parent, idtf, type, tag)
    , contentType(contentType)
    , mimeType(mimeType)
    , fileName(fileName)
    , contentData(contentData)
  {
  }

  std::string const & getContentType() const
  {
    return contentType;
  }

  std::string const & getMimeType() const
  {
    return mimeType;
  }

  std::string const & getFileName() const
  {
    return fileName;
  }

  std::string const & getContentData() const
  {
    return contentData;
  }

private:
  std::string contentType;
  std::string mimeType;
  std::string fileName;
  std::string contentData;
};

class Bus : public Node
{
public:
  Bus(std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      std::string const & nodeId)
    : Node(id, parent, idtf, type, tag)
    , nodeId(nodeId)
  {
  }

  std::string const & getNodeId() const
  {
    return nodeId;
  }

private:
  std::string nodeId;
};

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

class Contour : public Node
{
public:
  Contour(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag)
    : Node(id, parent, idtf, type, tag)
  {
  }

  void addElement(std::shared_ptr<ScgElement> & element)
  {
    elements.push_back(element);
  }

  std::vector<std::shared_ptr<ScgElement>> & getElements()
  {
    return elements;
  }

private:
  std::vector<std::shared_ptr<ScgElement>> elements;
};


class Connector : public ScgElement
{
public:
  Connector(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      std::shared_ptr<ScgElement> sourceEl,
      std::shared_ptr<ScgElement> targetEl)
    : ScgElement(id, parent, idtf, type, tag)
    , source(sourceEl)
    , target(targetEl)
  {
  }

  std::shared_ptr<ScgElement> getSource() const
  {
    return source;
  }

  std::shared_ptr<ScgElement> getTarget() const
  {
    return target;
  }

  void setSource(std::shared_ptr<ScgElement> sourceEl)
  {
    source = sourceEl;
  }

  void setTarget(std::shared_ptr<ScgElement> targetEl)
  {
    target = targetEl;
  }

private:
  std::shared_ptr<ScgElement> source;
  std::shared_ptr<ScgElement> target;
};

class GwfParser
{
public:
  std::unordered_map<std::string, std::shared_ptr<ScgElement>> Parse(std::string const & xmlStr);

private:
  std::shared_ptr<Node> CreateNode(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr & el);

  std::shared_ptr<Link> CreateLink(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr & el);

  bool HasContent(const xmlNodePtr node);

  std::shared_ptr<Bus> CreateBus(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr & el);

  std::shared_ptr<Contour> CreateContour(
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      std::string const & tag,
      xmlNodePtr & el,
      std::vector<std::unordered_map<std::shared_ptr<Contour>, std::string>> & contoursList);

  std::shared_ptr<Connector> CreateConnector(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & idtf,
      std::string const & type,
      xmlNodePtr & el,
      std::vector<std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>>> &
          connectorsList);

  void FillConnector(
      std::unordered_map<std::shared_ptr<Connector>, std::pair<std::string, std::string>> & connectorSourceTarget,
      std::unordered_map<std::string, std::shared_ptr<ScgElement>> elements);

  void FillContour(
    std::unordered_map<std::shared_ptr<Contour>, std::string> & contourAndId,
    std::unordered_map<std::string, std::shared_ptr<ScgElement>> elements
  );

  void ProcessStaticSector(
      xmlNodePtr staticSector,
      std::unordered_map<std::string, std::shared_ptr<ScgElement>> & elements);

  std::string XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr);
  std::unique_ptr<xmlChar, XmlCharDeleter> GetXmlProp(xmlNodePtr node, char const * propName);
};
