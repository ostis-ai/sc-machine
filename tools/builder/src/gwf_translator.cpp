/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "gwf_translator.hpp"

#include "utils.hpp"

#include "base64/base64.h"
#include "tinyxml/tinyxml2.h"

#include "sc-memory/sc_memory.hpp"

#include <fstream>
#include <iostream>
#include <unordered_map>

#include <assert.h>

std::string const sEdge = "arc";
std::string const sPair = "pair";
std::string const sBus = "bus";
std::string const sNode = "node";
std::string const sContour = "contour";

GwfTranslator::GwfTranslator(ScMemoryContext & context)
  : Translator(context)
{
}

bool GwfTranslator::TranslateImpl(Params const & params)
{
  std::string data;
  GetFileContent(params.m_fileName, data);

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError error = doc.Parse(data.c_str());

  if (error != tinyxml2::XML_SUCCESS)
    PARSE_ERROR(params.m_fileName, -1, doc.GetErrorStr2());

  tinyxml2::XMLElement *root = doc.FirstChildElement("GWF");
  if (!root)
    PARSE_ERROR(params.m_fileName, -1, "Can't find root element");

  root = root->FirstChildElement("staticSector");
  if (!root)
    PARSE_ERROR(params.m_fileName, -1, "Can't find static sector");

  // collect elements
  std::vector<tinyxml2::XMLElement*> nodes;
  std::vector<tinyxml2::XMLElement*> edges;
  std::vector<tinyxml2::XMLElement*> buses;
  std::vector<tinyxml2::XMLElement*> all;

  tinyxml2::XMLElement *el = root->FirstChildElement();
  while (el)
  {
    all.push_back(el);
    if (el->Name() == sEdge || el->Name() == sPair)
    {
      edges.push_back(el);
    }
    else if (el->Name() == sBus)
    {
      buses.push_back(el);
    }
    else
    {
      nodes.push_back(el);
    }

    el = el->NextSiblingElement();
  }

  std::unordered_map<std::string, ScAddr> idMap;

  // create all nodes
  std::vector<tinyxml2::XMLElement*>::iterator it, itEnd = nodes.end();
  for (it = nodes.begin(); it != itEnd; ++it)
  {
    el = *it;

    std::string const idtf = el->Attribute("idtf");
    std::string const id = el->Attribute("id");

    auto itId = idMap.find(id);
    if (itId != idMap.end())
      continue;

    if (idtf.size() > 0)
    {
      ScAddr const addr = GetScAddr(idtf);
      if (addr.IsValid())
      {
        idMap[id] = addr;
        continue;    // skip elements that already exists
      }
    }
  }
}
//    if (el->Name() == sContour)
//    {
//      AppendScAddr(m_ctx.CreateNode(ScType::NodeConstStruct, idtf);
//    } 
//    else
//    {
//      tinyxml2::XMLElement *content = el->FirstChildElement("content");
//      if (!content)
//        PARSE_ERROR(params.m_fileName, -1, "There are no child content for node with id=" + id);
//
//      if (content->IntAttribute("type") == 0)
//      {
//        std::string const strType = el->Attribute("type");
//        ScAddr const addr = m_ctx.CreateNode(ConvertType(strType));
//        appendScAddr(addr, idtf);
//      } 
//      else
//      {
//        // need to create link
//        ScAddr const addr = m_ctx.CreateLink();
//        
//        // setup content
//        std::string data = content->GetText();
//        if (content->IntAttribute("type") == 4)
//          data = base64_decode(data);
//
//        sc_stream *stream = sc_stream_memory_new(data.c_str(), (sc_uint)data.size(), SC_STREAM_FLAG_READ, SC_FALSE);
//        sc_memory_set_link_content(mContext, addr, stream);
//        sc_stream_free(stream);
//
//        if (mParams.autoFormatInfo)
//        {
//          String ext = StringUtil::getFileExtension(content->Attribute("file_name"));
//          if (!ext.empty())
//            generateFormatInfo(addr, ext);
//        }
//      }
//    }
//
//    if (!idtf.empty())
//      sc_helper_set_system_identifier(mContext, addr, idtf.c_str(), (sc_uint32)idtf.size());
//
//    id_map[id] = addr;
//  }
//
//  // process buses
//  itEnd = buses.end();
//  for (it = buses.begin(); it != itEnd; ++it)
//  {
//    el = *it;
//
//    tStringAddrMap::iterator itOwner = id_map.find(el->Attribute("owner"));
//    if (itOwner == id_map.end())
//      continue;
//
//    id_map[el->Attribute("id")] = itOwner->second;
//  }
//
//  // now create edges
//  bool created = true;
//  while (created)
//  {
//    created = false;
//
//    itEnd = edges.end();
//    for (it = edges.begin(); it != itEnd; ++it)
//    {
//      el = *it;
//
//      sc_addr addr;
//      String id = el->Attribute("id");
//      String idtf = el->Attribute("idtf");
//
//      if (id_map.find(id) != id_map.end())
//        continue;
//
//      if (getScAddr(idtf, addr))
//        continue;
//
//      // get begin and end elements
//      tStringAddrMap::iterator itB = id_map.find(el->Attribute("id_b"));
//      if (itB == id_map.end())
//        continue;
//
//      tStringAddrMap::iterator itE = id_map.find(el->Attribute("id_e"));
//      if (itE == id_map.end())
//        continue;
//
//      // create arc
//      created = true;
//      addr = sc_memory_arc_new(mContext, convertType(el->Attribute("type")), itB->second, itE->second);
//      appendScAddr(addr, idtf);
//      id_map[id] = addr;
//
//      if (!idtf.empty())
//        sc_helper_set_system_identifier(mContext, addr, idtf.c_str(), (sc_uint32)idtf.size());
//    }
//  }
//
//  // now append elemnts into contours
//  itEnd = all.end();
//  for (it = all.begin(); it != itEnd; ++it)
//  {
//    el = *it;
//
//    tStringAddrMap::iterator itSelf = id_map.find(el->Attribute("id"));
//    if (itSelf == id_map.end())
//      continue;
//
//    tStringAddrMap::iterator itP = id_map.find(el->Attribute("parent"));
//    if (itP == id_map.end())
//      continue;
//
//    sc_memory_arc_new(mContext, sc_type_arc_pos_const_perm, itP->second, itSelf->second);
//  }
//
//  return false;
//}
//
ScAddr GwfTranslator::GetScAddr(std::string const & idtf)
{
  return {};
}
//bool GwfTranslator::getScAddr(const String &idtf, sc_addr &addr)
//{
//  tStringAddrMap::iterator it = mLocalIdtfAddrs.find(idtf);
//  if (it != mLocalIdtfAddrs.end())
//  {
//    addr = it->second;
//    return true;
//  }
//
//  it = mSysIdtfAddrs.find(idtf);
//  if (it != mSysIdtfAddrs.end())
//  {
//    addr = it->second;
//    return true;
//  }
//
//  it = msGlobalIdtfAddrs.find(idtf);
//  if (it != msGlobalIdtfAddrs.end())
//  {
//    addr = it->second;
//    return true;
//  }
//
//  if (sc_helper_find_element_by_system_identifier(mContext, idtf.c_str(), (sc_uint32)idtf.size(), &addr) == SC_RESULT_OK)
//    return true;
//
//  return false;
//}

ScType GwfTranslator::ConvertType(std::string const & type)
{
  if (type == "node/-/not_define")
    return ScType::Node;

  if (type == "node/const/general_node" || type == "node/const/general")
    return ScType::NodeConst;

  if (type == "node/var/general_node" || type == "node/const/var")
    return ScType::NodeVar;

  if (type == "node/const/relation")
    return ScType::NodeConstNoRole;

  if (type == "node/var/relation")
    return ScType::NodeVarNoRole;

  if (type == "node/const/attribute")
    return ScType::NodeConstRole;

  if (type == "node/var/attribute")
    return ScType::NodeVarRole;

  if (type == "node/const/nopredmet")
    return ScType::NodeConstStruct;

  if (type == "node/var/nopredmet")
    return ScType::NodeVarStruct;

  if (type == "node/const/predmet")
    return ScType::NodeConstAbstract;

  if (type == "node/var/predmet")
    return ScType::NodeVarAbstract;

  if (type == "node/const/group")
    return ScType::NodeConstClass;

  if (type == "node/var/group")
    return ScType::NodeVarClass;

  if (type == "node/const/asymmetry" || type == "node/const/symmetry" || type == "node/const/tuple")
    return ScType::NodeConstTuple;

  if (type == "node/var/asymmetry" || type == "node/var/symmetry" || type == "node/var/tuple")
    return ScType::NodeVarTuple;

  // -------
  if (type == "arc/-/-")
    return ScType::EdgeAccess;

  if (type == "pair/orient")
    return ScType::EdgeDCommon;

  if (type == "pair/noorient")
    return ScType::EdgeUCommon;

  if (type == "pair/const/synonym" || type == "pair/const/noorient")
    return ScType::EdgeDCommonConst;

  if (type == "pair/var/synonym" || type == "pair/var/noorient")
    return ScType::EdgeDCommonVar;

  if (type == "pair/const/orient")
    return ScType::EdgeUCommonConst;

  if (type == "pair/var/orient")
    return ScType::EdgeUCommonVar;

  if (type == "arc/const/fuz")
    return ScType::EdgeAccessConstFuzPerm;

  if (type == "arc/var/fuz")
    return ScType::EdgeAccessVarFuzPerm;

  if (type == "arc/const/fuz/temp")
    return ScType::EdgeAccessConstFuzTemp;

  if (type == "arc/var/fuz/temp")
    return ScType::EdgeAccessVarFuzTemp;

  if (type == "arc/const/neg")
    return ScType::EdgeAccessConstNegPerm;

  if (type == "arc/var/neg")
    return ScType::EdgeAccessVarNegPerm;

  if (type == "arc/const/neg/temp")
    return ScType::EdgeAccessConstNegTemp;

  if (type == "arc/var/neg/temp")
    return ScType::EdgeAccessVarNegTemp;

  if (type == "arc/const/pos")
    return ScType::EdgeAccessConstPosPerm;

  if (type == "arc/var/pos")
    return ScType::EdgeAccessVarPosPerm;

  if (type == "arc/const/pos/temp")
    return ScType::EdgeAccessConstPosTemp;

  if (type == "arc/var/pos/temp")
    return ScType::EdgeAccessVarPosTemp;
              
  return ScType();
}