/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_memory.hpp"

#include "uiSc2SCnJsonTranslator.h"

#include "uiTranslators.h"
#include "uiKeynodes.h"
#include "uiUtils.h"
#include <iostream>
#include <algorithm>
// #include <string_view>

// constexpr std::string_view SCN_TRANSLATOR_CONSTANTS [] ={

// };

uiSc2SCnJsonTranslator::uiSc2SCnJsonTranslator()
 : mScElementsInfoPool(nullptr)
{
}

uiSc2SCnJsonTranslator::~uiSc2SCnJsonTranslator()
{
  delete[] mScElementsInfoPool;
}

void uiSc2SCnJsonTranslator::runImpl()
{
  // get command arguments
  sc_iterator5 * it5 = sc_iterator5_a_a_f_a_f_new(
      s_default_ctx,
      sc_type_node | sc_type_const,
      sc_type_arc_common | sc_type_const,
      mInputConstructionAddr,
      sc_type_arc_pos_const_perm,
      keynode_question_nrel_answer);
  if (sc_iterator5_next(it5) == SC_TRUE)
  {
    sc_iterator3 * it3 =
        sc_iterator3_f_a_a_new(s_default_ctx, sc_iterator5_value(it5, 0), sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it3) == SC_TRUE)
      mKeywordsList.push_back(sc_iterator3_value(it3, 2));
    sc_iterator3_free(it3);
  }
  sc_iterator5_free(it5);

  collectScElementsInfo();

  sc_json result = sc_json::array();

  tScAddrList::iterator it, itEnd = mKeywordsList.end();
  for (it = mKeywordsList.begin(); it != itEnd; ++it)
  {
    result.insert(result.end(), json(mScElementsInfo[*it], 0, false));
  }
  mOutputData = result.dump();
}

void uiSc2SCnJsonTranslator::collectScElementsInfo()
{
  sc_uint32 elementsCount = mObjects.size();
  mScElementsInfoPool = new sScElementInfo[elementsCount];

  sc_uint32 poolUsed = 0;
  // first collect information about elements
  tScAddrToScTypeMap::const_iterator it, itEnd = mObjects.end();
  for (it = mObjects.begin(); it != itEnd; ++it)
  {
    if (mScElementsInfo.find(it->first) != mScElementsInfo.end())
      continue;

    mScElementsInfoPool[poolUsed].addr = it->first;
    mScElementsInfoPool[poolUsed].type = it->second;
    mScElementsInfo[it->first] = &(mScElementsInfoPool[poolUsed]);
    poolUsed++;
  }

  // now we need to iterate all arcs and collect output/input arcs info
  sc_type elType;
  sc_addr begAddr, endAddr, arcAddr;
  for (it = mObjects.begin(); it != itEnd; ++it)
  {
    arcAddr = it->first;
    elType = it->second;

    sScElementInfo * elInfo = mScElementsInfo[arcAddr];
    elInfo->isInTree = false;

    // skip nodes and links
    if (!(elType & sc_type_arc_mask))
      continue;

    // get begin/end addrs
    if (sc_memory_get_arc_begin(s_default_ctx, arcAddr, &begAddr) != SC_RESULT_OK)
      continue;  // @todo process errors
    if (!isNeedToTranslate(begAddr))
      continue;
    if (sc_memory_get_arc_end(s_default_ctx, arcAddr, &endAddr) != SC_RESULT_OK)
      continue;  // @todo process errors
    if (!isNeedToTranslate(endAddr))
      continue;

    sScElementInfo * begInfo = mScElementsInfo[begAddr];
    sScElementInfo * endInfo = mScElementsInfo[endAddr];

    // filter by modifiers
    

    elInfo->source = begInfo;
    elInfo->target = endInfo;

    // check if arc is not broken
    if (begInfo == nullptr || endInfo == nullptr)
      continue;
    endInfo->inputArcs.push_back(mScElementsInfo[arcAddr]);
    begInfo->outputArcs.push_back(mScElementsInfo[arcAddr]);
  }

  // find structures elements
  tScAddrList::const_iterator keywordIt, keywordItEnd = mKeywordsList.end();
  sScElementInfo::tScElementInfoList::const_iterator elIt, elItEnd, structureElementIt;
  sScElementInfo * elInfo;
  for (keywordIt = mKeywordsList.begin(); keywordIt != keywordItEnd; ++keywordIt)
  {
    elInfo = mScElementsInfo[*keywordIt];
    if (elInfo->type & sc_type_node_struct)
    {
      elItEnd = elInfo->outputArcs.end();
      for (elIt = elInfo->outputArcs.begin(); elIt != elItEnd;)
      {
        if ((*elIt)->type & sc_type_arc_pos_const_perm && (*elIt)->inputArcs.empty())
        {
          structureElementIt = std::find((*elIt)->target->inputArcs.begin(), (*elIt)->target->inputArcs.end(), *elIt);
          if (structureElementIt != (*elIt)->target->inputArcs.end())
          {
            elInfo->structureElements.push_back((*elIt)->target);
            (*elIt)->target->inputArcs.erase(structureElementIt);
            elIt = elInfo->outputArcs.erase(elIt);
          }
          else
          {
            ++elIt;
          }
        }
        else
        {
          ++elIt;
        }
      }
      elInfo->structKeyword = findStructKeyword(elInfo->structureElements);
    }
  }
}

sScElementInfo * uiSc2SCnJsonTranslator::findStructKeyword(sScElementInfo::tScElementInfoList structureElements)
{
  sScElementInfo::tScElementInfoList structures;
  sScElementInfo::tScElementInfoList elements = structureElements;
  std::copy_if(
      structureElements.begin(), structureElements.end(), std::back_inserter(structures), [](sScElementInfo * el) {
        return el->type & sc_type_node_struct;
      });
  if (!structures.empty())
  {
    elements = structures;
  }
  auto result =
      std::max_element(elements.begin(), elements.end(), [](sScElementInfo * left, sScElementInfo * right) {
        return (left->outputArcs.size() + left->inputArcs.size()) <
               (right->outputArcs.size() + right->inputArcs.size());
      });
  if (result != elements.end())
    return *result;
  return nullptr;
}

sc_json uiSc2SCnJsonTranslator::json(sScElementInfo * elInfo, int level, bool isStruct)
{
  sc_json result;
  bool content_exists = false;
  bool isFullLinkedNodes = true;

  result = getBaseInfo(elInfo);
  if (!elInfo->structureElements.empty())
  {
    structureElements = elInfo->structureElements;
    result["struct"] = json(elInfo->structKeyword, 0, true);
  }
  // if node is arc
  if (elInfo->type & sc_type_arc_mask)
  {
    result["sourceNode"] = getBaseInfo(elInfo->source);
    result["targetNode"] = getBaseInfo(elInfo->target);
  }
  // if node is link
  if (elInfo->type & sc_type_link)
  {
    isFullLinkedNodes = false;
    sc_addr format;
    String content;
    sc_helper_resolve_system_identifier(s_default_ctx, "format_html", &format);
    if (sc_helper_check_arc(s_default_ctx, elInfo->addr, format, sc_type_arc_common | sc_type_const) == SC_TRUE)
    {
      result["contentType"] = "format_html";
    }
    sc_helper_resolve_system_identifier(s_default_ctx, "format_github_source_link", &format);
    if (sc_helper_check_arc(s_default_ctx, elInfo->addr, format, sc_type_arc_common | sc_type_const) == SC_TRUE)
    {
      result["contentType"] = "format_github_source_link";
    }
    sc_helper_resolve_system_identifier(s_default_ctx, "format_pdf", &format);
    if (sc_helper_check_arc(s_default_ctx, elInfo->addr, format, sc_type_arc_common | sc_type_const) == SC_TRUE)
    {
      result["contentType"] = "format_pdf";
    }
    sc_helper_resolve_system_identifier(s_default_ctx, "format_png", &format);
    if (sc_helper_check_arc(s_default_ctx, elInfo->addr, format, sc_type_arc_common | sc_type_const) == SC_TRUE)
    {
      result["contentType"] = "format_png";
    }
    if (result["contentType"].is_null())
    {
      sc_stream * stream;
      sc_memory_get_link_content(s_default_ctx, elInfo->addr, &stream);
      if (stream != nullptr && ScStreamConverter::StreamToString(std::make_shared<ScStream>(stream), content))
      {
        std::istringstream streamString{ content };
        streamString >> content;
        content_exists = true;
      }

      if (content_exists)
      {
        result["content"] = content;
      }
      else
      {
        result["content"] = sc_json();
      }
      result["contentType"] = "format_txt";
    }
  }
  if (level < maxLevel || (elInfo->type & sc_type_node && elInfo->type & sc_type_node_tuple))
  {
    result["children"] = getChildrenByModifierAddr(elInfo, keynode_nrel_system_identifier, isStruct);
    sc_json children = getChildrenByModifierAddr(elInfo, keynode_nrel_main_idtf, isStruct);
    result["children"].insert(result["children"].end(), children.begin(), children.end());
    children = getChildrenByModifierAddr(elInfo, keynode_nrel_idtf, isStruct);
    result["children"].insert(result["children"].end(), children.begin(), children.end());
    children = getChildrenByModifierAddr(elInfo, keynode_rrel_key_sc_element, isStruct);
    result["children"].insert(result["children"].end(), children.begin(), children.end());
    children = getChildrens(elInfo, isStruct);
    result["children"].insert(result["children"].end(), children.begin(), children.end());
    if (isFullLinkedNodes)
      result["children"] = getJsonOfLinkedNodes(result["children"], level + 1, isStruct);
  }

  return result;
}

sc_json uiSc2SCnJsonTranslator::getChildrens(sScElementInfo * elInfo, bool isStruct)
{
  sc_json childrens = sc_json::array();
  sScElementInfo::tScElementInfoList outputArcs = elInfo->outputArcs;
  sScElementInfo::tScElementInfoList inputArcs = elInfo->inputArcs;

  sc_json rightChildrens = getChildrensByDirection(outputArcs, "right", isStruct);
  childrens.insert(childrens.end(), rightChildrens.begin(), rightChildrens.end());

  sc_json leftChildrens = getChildrensByDirection(inputArcs, "left", isStruct);
  childrens.insert(childrens.end(), leftChildrens.begin(), leftChildrens.end());

  return childrens;
}

sc_json uiSc2SCnJsonTranslator::getChildrensByDirection(
    sScElementInfo::tScElementInfoList arcs,
    String const & direction,
    bool isStruct)
{
  sc_json childrens = sc_json::array();
  sScElementInfo::tScElementInfoList::const_iterator it, itEnd = arcs.end();
  for (it = arcs.begin(); it != itEnd; ++it)
  {
    sc_json children = getChildren((*it), direction, isStruct);
    if (!children.is_null())
    {
      if (!children["modifiers"][0].is_null())
      {
        sc_json modifierAddr = children["modifiers"][0]["addr"];
        sc_json::iterator result = std::find_if(childrens.begin(), childrens.end(), [modifierAddr](sc_json c) {
          return (!c["modifiers"].is_null() && modifierAddr == c["modifiers"][0]["addr"]);
        });
        if (result != childrens.end())
        {
          if ((*result)["arcs"].is_null())
          {
            (*result)["arcs"] = children["arcs"];
          }
          else
          {
            (*result)["arcs"].push_back(children["arcs"][0]);
          }
          if ((*result)["linkedNodes"].is_null())
          {
            (*result)["linkedNodes"] = children["linkedNodes"];
          }
          else
          {
            (*result)["linkedNodes"].push_back(children["linkedNodes"][0]);
          }
          if ((*result)["modifiers"].is_null())
          {
            (*result)["modifiers"] = children["modifiers"];
          }
          else
          {
            (*result)["modifiers"][0]["modifierArcs"].push_back(children["modifiers"][0]["modifierArcs"][0]);
          }
        }
        else
        {
          childrens.push_back(children);
        }
      }
      else
      {
        children["modifiers"] = sc_json();
        childrens.push_back(children);
      }
    }
  }
  return childrens;
}

sc_json uiSc2SCnJsonTranslator::getJsonOfLinkedNodes(sc_json childrens, int level, bool isStruct)
{
  sc_json new_childrens = sc_json();
  sc_json children, linkedNode;
  sc_addr keynode_nrel_sc_text_translation, linkedNodeAddr;
  sScElementInfo * linkedNodeInfo;
  sc_helper_resolve_system_identifier(s_default_ctx, "nrel_sc_text_translation", &keynode_nrel_sc_text_translation);

  for (sc_json::const_iterator it = childrens.begin(); it != childrens.end(); ++it)
  {
    children = *it;
    if (!children["modifiers"].is_null() &&
        (children["modifiers"][0]["addr"] == SC_ADDR_LOCAL_TO_INT(keynode_nrel_sc_text_translation) || children["modifiers"][0]["addr"] == SC_ADDR_LOCAL_TO_INT(keynode_rrel_key_sc_element)))
    {
      --level;
    }
    for (size_t i = 0; i < children["linkedNodes"].size(); ++i)
    {
      linkedNode = children["linkedNodes"][i];
      linkedNodeAddr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(linkedNode["addr"].get<int>());
      linkedNodeAddr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(linkedNode["addr"].get<int>());
      linkedNodeInfo = mScElementsInfo[linkedNodeAddr];
      if (!(linkedNodeInfo->type & sc_type_node_struct))
      {
        linkedNode = json(linkedNodeInfo, level, isStruct);
      }
      children["linkedNodes"][i] = linkedNode;
    }

    new_childrens.push_back(children);
  }
  return new_childrens;
}

sc_json uiSc2SCnJsonTranslator::getBaseInfo(sScElementInfo * elInfo)
{
  sc_json result;

  if (!elInfo)
  {
    return result;
  }

  result["addr"] = std::stoi(uiTranslateFromSc::buildId(elInfo->addr));
  String idtf;
  bool idtf_exists = ui_translate_get_identifier(elInfo->addr, mOutputLanguageAddr, idtf);
  if (idtf_exists)
  {
    result["idtf"] = idtf;
  }
  else
  {
    result["idtf"] = sc_json();
  }
  result["type"] = elInfo->type;

  return result;
}

sc_json uiSc2SCnJsonTranslator::getChildrenByModifierAddr(sScElementInfo * elInfo, sc_addr modifierAddr, bool isStruct)
{
  sc_json childrens = sc_json::array();
  sc_json fullChildren;
  sScElementInfo::tScElementInfoList filtered;
  std::copy_if(
      elInfo->outputArcs.begin(),
      elInfo->outputArcs.end(),
      std::back_inserter(filtered),
      [modifierAddr](sScElementInfo * el) {
        return std::find_if(el->inputArcs.begin(), el->inputArcs.end(), [modifierAddr](sScElementInfo * modifierArc) {
                 return modifierArc->source->addr == modifierAddr;
               }) != el->inputArcs.end();
      });
  for (sScElementInfo * arcInfo : filtered)
  {
    sc_json children = getChildren(arcInfo, "right", isStruct);
    if (!children.is_null())
    {
      if (fullChildren["arcs"].is_null())
      {
        fullChildren["arcs"] = children["arcs"];
      }
      else
      {
        fullChildren["arcs"].push_back(children["arcs"][0]);
      }
      if (fullChildren["linkedNodes"].is_null())
      {
        fullChildren["linkedNodes"] = children["linkedNodes"];
      }
      else
      {
        fullChildren["linkedNodes"].push_back(children["linkedNodes"][0]);
      }
      if (fullChildren["modifiers"].is_null())
      {
        fullChildren["modifiers"] = children["modifiers"];
      }
      else
      {
        fullChildren["modifiers"][0]["modifierArcs"].push_back(children["modifiers"][0]["modifierArcs"][0]);
      }
    }
  }
  if (!fullChildren.is_null())
  {
    childrens.push_back(fullChildren);
  }
  fullChildren = sc_json();
  filtered.clear();
  std::copy_if(
      elInfo->inputArcs.begin(),
      elInfo->inputArcs.end(),
      std::back_inserter(filtered),
      [modifierAddr](sScElementInfo * el) {
        return std::find_if(el->inputArcs.begin(), el->inputArcs.end(), [modifierAddr](sScElementInfo * modifierArc) {
                 return modifierArc->source->addr == modifierAddr;
               }) != el->inputArcs.end();
      });
  for (sScElementInfo * arcInfo : filtered)
  {
    sc_json children = getChildren(arcInfo, "left", isStruct);
    if (!children.is_null())
    {
      if (fullChildren["arcs"].is_null())
      {
        fullChildren["arcs"] = children["arcs"];
      }
      else
      {
        fullChildren["arcs"].push_back(children["arcs"][0]);
      }
      if (fullChildren["linkedNodes"].is_null())
      {
        fullChildren["linkedNodes"] = children["linkedNodes"];
      }
      else
      {
        fullChildren["linkedNodes"].push_back(children["linkedNodes"][0]);
      }
      if (fullChildren["modifiers"].is_null())
      {
        fullChildren["modifiers"] = !children["modifiers"][0].is_null() ? children["modifiers"] : sc_json();
      }
      else
      {
        fullChildren["modifiers"][0]["modifierArcs"].push_back(children["modifiers"][0]["modifierArcs"][0]);
      }
    }
  }
  if (!fullChildren.is_null())
  {
    childrens.push_back(fullChildren);
  }
  return childrens;
}

sc_json uiSc2SCnJsonTranslator::getChildren(sScElementInfo * arcInfo, String const & direction, bool isStruct)
{
  sc_json children;

  if (arcInfo->isInTree)
    return children;
  if (isStruct && (std::find(structureElements.begin(), structureElements.end(), arcInfo) == structureElements.end()))
    return children;
  arcInfo->isInTree = true;

  sScElementInfo * linkedNode = nullptr;
  if (direction == "right")
  {
    linkedNode = arcInfo->target;
  }
  else if (direction == "left")
  {
    linkedNode = arcInfo->source;
  }

  sc_json arc = getBaseInfo(arcInfo);
  arc["direction"] = direction;

  sScElementInfo::tScElementInfoList modifiersList = arcInfo->inputArcs;

 auto modifierIt = modifiersList.begin();
  sc_json modifier = sc_json();
  if (modifierIt != modifiersList.end() && !((*modifierIt)->isInTree))
  {
    modifier = getBaseInfo((*modifierIt)->source);
    modifier["modifierArcs"].push_back(getBaseInfo(*modifierIt));
    (*modifierIt)->isInTree = true;
  }

  children["arcs"].push_back(arc);
  children["modifiers"].push_back(modifier);
  children["linkedNodes"].push_back(getBaseInfo(linkedNode));
  return children;
}

// -------------------------------------
sc_result uiSc2SCnJsonTranslator::ui_translate_sc2scn(const sc_event * event, sc_addr arg)
{
  sc_addr cmd_addr, input_addr, format_addr, lang_addr;

  if (sc_memory_get_arc_end(s_default_ctx, arg, &cmd_addr) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (ui_check_cmd_type(cmd_addr, keynode_command_translate_from_sc) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr, &lang_addr) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (format_addr == keynode_format_scn_json)
  {
    uiSc2SCnJsonTranslator translator;
    translator.translate(input_addr, format_addr, lang_addr);
  }

  return SC_RESULT_OK;
}
