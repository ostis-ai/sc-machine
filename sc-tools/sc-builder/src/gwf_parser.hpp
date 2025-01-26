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

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "gwf_translator_constants.hpp"
#include "sc_scg_element.hpp"

class XmlCharDeleter
{
public:
  void operator()(xmlChar * ptr) const;
};

class GWFParser
{
public:
  static void Parse(std::string const & xmlStr, SCgElements & elements);

private:
  static std::shared_ptr<SCgNode> CreateNode(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag);

  static std::shared_ptr<SCgLink> CreateLink(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag,
      xmlNodePtr el);

  static bool HasContent(xmlNodePtr node);

  static std::shared_ptr<SCgBus> CreateBus(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag,
      xmlNodePtr el);

  static std::shared_ptr<SCgContour> CreateContour(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag);

  static std::shared_ptr<SCgConnector> CreateConnector(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag,
      xmlNodePtr el,
      SCgConnectors & connectors);

  static void FillConnectors(SCgConnectors const & connectors, SCgElements const & elements);

  static void FillContours(SCgContours const & contours, SCgElements const & elements);

  static void ProcessStaticSector(xmlNodePtr staticSector, SCgElements & elementsWithParents);

  static std::string XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr);
  static std::unique_ptr<xmlChar, XmlCharDeleter> GetXmlProp(xmlNodePtr node, std::string const & propName);
  static std::string GetXmlPropStr(xmlNodePtr node, std::string const & propName);
};
