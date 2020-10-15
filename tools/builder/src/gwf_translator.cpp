/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "gwf_translator.h"
#include "tinyxml/tinyxml2.h"
#include "base64/base64.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <assert.h>


GwfTranslator::GwfTranslator(sc_memory_context *ctx)
  : iTranslator(ctx)
{

}

GwfTranslator::~GwfTranslator()
{

}

bool GwfTranslator::translateImpl()
{
  // open file and read data
  bool result = true;
  std::ifstream ifs(mParams.fileName.c_str());
  if (ifs.is_open())
  {
    String data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    result = processString(data);
  } else
    return false;

  ifs.close();

  return result;
}

const String& GwfTranslator::getFileExt() const
{
  return GwfTranslatorFactory::EXTENSION;
}


bool GwfTranslator::processString(const String &data)
{
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError error = doc.Parse(data.c_str());

  if (error != tinyxml2::XML_SUCCESS)
  {
    THROW_EXCEPT(Exception::ERR_PARSE,
                 doc.GetErrorStr2(),
                 mParams.fileName,
                 -1);
  }

  tinyxml2::XMLElement *root = doc.FirstChildElement("GWF");
  if (!root)
  {
    THROW_EXCEPT(Exception::ERR_PARSE,
                 "Can't find root element",
                 mParams.fileName,
                 -1);
  }
  root = root->FirstChildElement("staticSector");
  if (!root)
  {
    THROW_EXCEPT(Exception::ERR_PARSE,
                 "Cna't find static sector",
                 mParams.fileName,
                 -1);
  }

  // collect elements
  std::vector<tinyxml2::XMLElement*> nodes;
  std::vector<tinyxml2::XMLElement*> edges;
  std::vector<tinyxml2::XMLElement*> buses;
  std::vector<tinyxml2::XMLElement*> all;

  static std::string s_arc = "arc";
  static std::string s_pair = "pair";
  static std::string s_bus = "bus";

  tinyxml2::XMLElement *el = root->FirstChildElement();
  while (el)
  {
    all.push_back(el);
    if (el->Name() == s_arc || el->Name() == s_pair)
      edges.push_back(el);
    else if (el->Name() == s_bus)
      buses.push_back(el);
    else
      nodes.push_back(el);

    el = el->NextSiblingElement();
  }

  static std::string s_node = "node";
  static std::string s_contour = "contour";

  tStringAddrMap id_map;
  tStringAddrMap::iterator itId;

  // create all nodes
  std::vector<tinyxml2::XMLElement*>::iterator it, itEnd = nodes.end();
  for (it = nodes.begin(); it != itEnd; ++it)
  {
    el = *it;

    String idtf = el->Attribute("idtf");
    String id = el->Attribute("id");
    sc_addr addr;

    itId = id_map.find(id);
    if (itId != id_map.end())
      continue;

    if (idtf.size() > 0)
    {
      if (getScAddr(idtf, addr))
      {
        id_map[id] = addr;
        continue;    // skip elements that already exists
      }
    }

    if (el->Name() == s_contour)
    {
      addr = sc_memory_node_new(mContext, sc_type_const | sc_type_node_struct);
      appendScAddr(addr, idtf);
    } else
    {
      tinyxml2::XMLElement *content = el->FirstChildElement("content");
      if (!content)
      {
        THROW_EXCEPT(Exception::ERR_PARSE,
                     "There are no child content for node with id=" + id,
                     mParams.fileName,
                     -1);
      }

      if (content->IntAttribute("type") == 0)
      {
        addr = sc_memory_node_new(mContext, convertType(el->Attribute("type")));
        appendScAddr(addr, idtf);
      } else
      {
        // need to create link
        addr = sc_memory_link_new(mContext);

        // setup content
        String data = content->GetText();

        if (content->IntAttribute("type") == 4)
          data = base64_decode(data);

        sc_stream *stream = sc_stream_memory_new(data.c_str(), (sc_uint)data.size(), SC_STREAM_FLAG_READ, SC_FALSE);
        sc_memory_set_link_content(mContext, addr, stream);
        sc_stream_free(stream);

        if (mParams.autoFormatInfo)
        {
          String ext = StringUtil::getFileExtension(content->Attribute("file_name"));
          if (!ext.empty())
            generateFormatInfo(addr, ext);
        }
      }
    }

    if (!idtf.empty())
      sc_helper_set_system_identifier(mContext, addr, idtf.c_str(), (sc_uint32)idtf.size());

    id_map[id] = addr;
  }

  // process buses
  itEnd = buses.end();
  for (it = buses.begin(); it != itEnd; ++it)
  {
    el = *it;

    tStringAddrMap::iterator itOwner = id_map.find(el->Attribute("owner"));
    if (itOwner == id_map.end())
      continue;

    id_map[el->Attribute("id")] = itOwner->second;
  }

  // now create edges
  bool created = true;
  while (created)
  {
    created = false;

    itEnd = edges.end();
    for (it = edges.begin(); it != itEnd; ++it)
    {
      el = *it;

      sc_addr addr;
      String id = el->Attribute("id");
      String idtf = el->Attribute("idtf");

      if (id_map.find(id) != id_map.end())
        continue;

      if (getScAddr(idtf, addr))
        continue;

      // get begin and end elements
      tStringAddrMap::iterator itB = id_map.find(el->Attribute("id_b"));
      if (itB == id_map.end())
        continue;

      tStringAddrMap::iterator itE = id_map.find(el->Attribute("id_e"));
      if (itE == id_map.end())
        continue;

      // create arc
      created = true;
      addr = sc_memory_arc_new(mContext, convertType(el->Attribute("type")), itB->second, itE->second);
      appendScAddr(addr, idtf);
      id_map[id] = addr;

      if (!idtf.empty())
        sc_helper_set_system_identifier(mContext, addr, idtf.c_str(), (sc_uint32)idtf.size());
    }
  }

  // now append elemnts into contours
  itEnd = all.end();
  for (it = all.begin(); it != itEnd; ++it)
  {
    el = *it;

    tStringAddrMap::iterator itSelf = id_map.find(el->Attribute("id"));
    if (itSelf == id_map.end())
      continue;

    tStringAddrMap::iterator itP = id_map.find(el->Attribute("parent"));
    if (itP == id_map.end())
      continue;

    sc_memory_arc_new(mContext, sc_type_arc_pos_const_perm, itP->second, itSelf->second);
  }

  return false;
}

bool GwfTranslator::getScAddr(const String &idtf, sc_addr &addr)
{
  tStringAddrMap::iterator it = mLocalIdtfAddrs.find(idtf);
  if (it != mLocalIdtfAddrs.end())
  {
    addr = it->second;
    return true;
  }

  it = mSysIdtfAddrs.find(idtf);
  if (it != mSysIdtfAddrs.end())
  {
    addr = it->second;
    return true;
  }

  it = msGlobalIdtfAddrs.find(idtf);
  if (it != msGlobalIdtfAddrs.end())
  {
    addr = it->second;
    return true;
  }

  if (sc_helper_find_element_by_system_identifier(mContext, idtf.c_str(), (sc_uint32)idtf.size(), &addr) == SC_RESULT_OK)
    return true;

  return false;
}

sc_type GwfTranslator::convertType(const String &type)
{
  if (type == "node/-/not_define" || type == "node/-/-/not_define")
    return sc_type_node;

  if (type == "node/const/general_node" || type == "node/const/general" || type == "node/const/perm/general")
    return sc_type_node | sc_type_const;

  if (type == "node/var/general_node" || type == "node/const/var" || type == "node/var/perm/general")
    return sc_type_node | sc_type_var;

  if (type == "node/const/relation" || type == "node/const/perm/relation")
    return sc_type_node | sc_type_node_norole | sc_type_const;

  if (type == "node/var/relation" || type == "node/var/perm/relation")
    return sc_type_node | sc_type_node_norole | sc_type_var;

  if (type == "node/const/attribute" || type == "node/const/perm/role")
    return sc_type_node | sc_type_node_role | sc_type_const;

  if (type == "node/var/attribute" || type == "node/var/perm/role")
    return sc_type_node | sc_type_node_role | sc_type_var;

  if (type == "node/const/nopredmet" || type == "node/const/perm/struct")
    return sc_type_node | sc_type_node_struct | sc_type_const;

  if (type == "node/var/nopredmet" || type == "node/var/perm/struct")
    return sc_type_node | sc_type_node_struct | sc_type_var;

  if (type == "node/const/predmet" || type == "node/const/perm/terminal")
    return sc_type_node | sc_type_node_abstract | sc_type_const;

  if (type == "node/var/predmet" || type == "node/var/perm/terminal")
    return sc_type_node | sc_type_node_abstract | sc_type_var;

  if (type == "node/const/group" || type == "node/const/perm/group")
    return sc_type_node | sc_type_node_class | sc_type_const;

  if (type == "node/var/group" || type == "node/var/perm/group")
    return sc_type_node | sc_type_node_class | sc_type_var;

  if (type == "node/const/asymmetry" || type == "node/const/symmetry" || type == "node/const/tuple" ||
      type == "node/const/perm/tuple")
    return sc_type_node | sc_type_node_tuple | sc_type_const;

  if (type == "node/var/asymmetry" || type == "node/var/symmetry" || type == "node/var/tuple" ||
      type == "node/var/perm/tuple")
    return sc_type_node | sc_type_node_tuple | sc_type_var;

  // -------
  if (type == "arc/-/-")
    return sc_type_arc_access;

  if (type == "pair/orient" || type == "pair/-/-/-/orient")
    return sc_type_arc_common;

  if (type == "pair/noorient" || type == "pair/-/-/-/noorient")
    return sc_type_edge_common;

  if (type == "pair/const/synonym" || type == "pair/const/noorient" || type == "pair/const/-/perm/noorien")
    return sc_type_edge_common | sc_type_const;

  if (type == "pair/var/synonym" || type == "pair/var/noorient" || type == "pair/var/-/perm/noorien")
    return sc_type_edge_common | sc_type_var;

  if (type == "pair/const/orient" || type == "pair/const/-/perm/orient")
    return sc_type_arc_common | sc_type_const;

  if (type == "pair/var/orient" || type == "pair/var/-/perm/orient")
    return sc_type_arc_common | sc_type_var;

  if (type == "arc/const/fuz" || type == "pair/const/fuz/perm/orient/membership")
    return sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_perm;

  if (type == "arc/var/fuz" || type == "pair/var/fuz/perm/orient/membership")
    return sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_perm;

  if (type == "arc/const/fuz/temp" || type == "pair/const/fuz/temp/orient/membership")
    return sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_temp;

  if (type == "arc/var/fuz/temp" || type == "pair/var/fuz/temp/orient/membership")
    return sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_temp;

  if (type == "arc/const/neg" || type == "pair/const/neg/perm/orient/membership")
    return sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_perm;

  if (type == "arc/var/neg" || type == "pair/var/neg/perm/orient/membership")
    return sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_perm;

  if (type == "arc/const/neg/temp" || type == "pair/const/neg/temp/orient/membership")
    return sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_temp;

  if (type == "arc/var/neg/temp" || type == "pair/var/neg/temp/orient/membership")
    return sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_temp;

  if (type == "arc/const/pos" || type == "pair/const/pos/perm/orient/membership")
    return sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_perm;

  if (type == "arc/var/pos" || type == "pair/var/pos/perm/orient/membership")
    return sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_perm;

  if (type == "arc/const/pos/temp" || type == "pair/const/pos/temp/orient/membership")
    return sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_temp;

  if (type == "arc/var/pos/temp" || type == "pair/var/pos/temp/orient/membership")
    return sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_temp;

  // Return default type when not supported
  std::basic_string<char> mainType = type.substr(0, 4);
  if (mainType == "node")
      return sc_type_node | sc_type_const;

  if (mainType == "pair" || mainType == "arc/")
      return sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_perm;

  THROW_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
               "Can't determine type for " << type,
               mParams.fileName, -1);

  return 0;
}

// -------------------
const String GwfTranslatorFactory::EXTENSION = "gwf";

GwfTranslatorFactory::GwfTranslatorFactory()
{
}

GwfTranslatorFactory::~GwfTranslatorFactory()
{
}

iTranslator* GwfTranslatorFactory::createInstance(sc_memory_context *ctx)
{
  return new GwfTranslator(ctx);
}

const String& GwfTranslatorFactory::getFileExt() const
{
  return EXTENSION;
}
