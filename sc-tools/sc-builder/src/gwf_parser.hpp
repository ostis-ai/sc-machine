/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <memory>
#include <list>

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "gwf_translator_constants.hpp"

class XmlCharDeleter
{
public:
  void operator()(xmlChar * ptr) const;
};

class SCgElement;
class SCgNode;
class SCgLink;
class SCgBus;
class SCgContour;
class SCgConnector;

class GWFParser
{
public:
  void Parse(std::string const & xmlStr, SCgElements & elements);

private:
  xmlChar const * const STATIC_SECTOR = (xmlChar const *)"staticSector";
  xmlChar const * const CONTENT = (xmlChar const *)"content";

  std::shared_ptr<SCgNode> CreateNode(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      xmlNodePtr el) const;

  std::shared_ptr<SCgLink> CreateLink(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      xmlNodePtr el) const;

  bool HasContent(xmlNodePtr const node) const;

  std::shared_ptr<SCgBus> CreateBus(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      xmlNodePtr el) const;

  std::shared_ptr<SCgContour> CreateContour(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag,
      xmlNodePtr el) const;

  std::shared_ptr<SCgConnector> CreateConnector(
      std::string const & tag,
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      xmlNodePtr el,
      SCgConnectors & connectors) const;

  void FillConnectors(SCgConnectors const & connectors, SCgElements const & elements);

  void FillContours(SCgContours const & contours, SCgElements const & elements);

  void ProcessStaticSector(xmlNodePtr staticSector, SCgElements & elementsWithParents);

  std::string XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr) const;
  std::unique_ptr<xmlChar, XmlCharDeleter> GetXmlProp(xmlNodePtr node, std::string const & propName) const;
  std::string GetXmlPropStr(xmlNodePtr node, std::string const & propName) const;
};
