#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <memory>

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "/home/iromanchuk/sc-machine/sc-memory/sc-memory/utils/sc_base64.hpp"

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

class GwfParser
{
public:
  std::vector<std::unordered_map<std::string, std::string>> Parse(std::string const & xmlStr);

private:
  void ProcessTag(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes);
  void ProcessNode(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes);
  void ProcessPair(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes);
  void ProcessBus(xmlNodePtr node, std::unordered_map<std::string, std::string> & attributes);
  void ProcessStaticSector(
      xmlNodePtr staticSector,
      std::vector<std::unordered_map<std::string, std::string>> & elements);

  std::string XmlCharToString(std::unique_ptr<xmlChar, XmlCharDeleter> const & ptr);
  std::unique_ptr<xmlChar, XmlCharDeleter> GetXmlProp(xmlNodePtr node, char const * propName);
};