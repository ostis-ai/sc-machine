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
#include <algorithm>
#include <string_view>

namespace scnTranslatorConstants
{
constexpr std::string_view STRUCT = "struct";
constexpr std::string_view SOURCE_NODE = "sourceNode";
constexpr std::string_view TARGET_NODE = "targetNode";
constexpr std::string_view CHILDREN = "children";
constexpr std::string_view CONTENT = "content";
constexpr std::string_view CONTENT_TYPE = "contentType";
constexpr std::string_view MODIFIERS = "modifiers";
constexpr std::string_view MODIFIER_ARCS = "modifierArcs";
constexpr std::string_view RIGHT = "right";
constexpr std::string_view LEFT = "left";
constexpr std::string_view ADDR = "addr";
constexpr std::string_view ARCS = "arcs";
constexpr std::string_view LINKED_NODES = "linkedNodes";
constexpr std::string_view IDTF = "idtf";
constexpr std::string_view TYPE = "type";
constexpr std::string_view DIRECTION = "direction";
constexpr std::string_view FORMAT_TXT = "format_txt";
constexpr std::string_view formats[] = {"format_html", "format_github_source_link", "format_pdf", "format_png"};
};  // namespace scnTranslatorConstants

uiSc2SCnJsonTranslator::uiSc2SCnJsonTranslator()
{
}

uiSc2SCnJsonTranslator::~uiSc2SCnJsonTranslator()
{
  std::for_each(mScElementsInfo.begin(), mScElementsInfo.end(), [](auto & pair) {
    delete pair.second;
    pair.second = nullptr;
  });
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
      mKeywordsList.insert(sc_iterator3_value(it3, 2));
    sc_iterator3_free(it3);
  }
  sc_iterator5_free(it5);

  collectScElementsInfo();

  sc_json results;
  for (auto const & keyword : mKeywordsList)
  {
    if (!mScElementsInfo[keyword])
      continue;
    sc_json result;
    json(mScElementsInfo[keyword], 0, false, result);
    results.push_back(result);
  }
  mOutputData = results.dump();
}

void uiSc2SCnJsonTranslator::collectScElementsInfo()
{
  sc_uint32 elementsCount = mObjects.size();

  // first collect information about elements
  for (auto const & it : mObjects)
  {
    if (mScElementsInfo.find(it.first) != mScElementsInfo.cend())
      continue;

    auto * elInfo = new sScElementInfo();
    elInfo->addr = it.first;
    elInfo->type = it.second;
    mScElementsInfo.insert({elInfo->addr, elInfo});
  }

  // now we need to iterate all arcs and collect output/input arcs info
  sc_type elType;
  sc_addr begAddr, endAddr, arcAddr;
  for (auto const & it : mObjects)
  {
    elType = it.second;

    // skip nodes and links
    if (!(elType & sc_type_arc_mask))
      continue;

    arcAddr = it.first;

    sScElementInfo * elInfo = mScElementsInfo[arcAddr];
    elInfo->isInTree = false;

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

    // check if arc is not broken
    if (begInfo == nullptr || endInfo == nullptr)
      continue;

    // filter
    if (std::any_of(mFiltersList.begin(), mFiltersList.end(), [begAddr](sc_addr const & modifier) {
          return modifier == begAddr;
        }))
    {
      if (endInfo->type & sc_type_arc_mask)
      {
        endInfo->source->outputArcs.erase(endInfo);
        endInfo->target->inputArcs.erase(endInfo);
      }
      continue;
    }

    elInfo->source = begInfo;
    elInfo->target = endInfo;

    endInfo->inputArcs.insert(mScElementsInfo[arcAddr]);
    begInfo->outputArcs.insert(mScElementsInfo[arcAddr]);
  }

  // find structures elements
  sScElementInfo::tScElementInfoList::const_iterator structureElementIt;
  sScElementInfo * elInfo;
  for (auto const & keyword : mKeywordsList)
  {
    elInfo = mScElementsInfo[keyword];
    if (elInfo && elInfo->type & sc_type_node_struct)
    {
      // get the key elements of the structure
      sScElementInfo::tScElementInfoList keynodes;
      std::for_each(elInfo->outputArcs.cbegin(), elInfo->outputArcs.cend(), [&keynodes](sScElementInfo * arc) {
        if (std::find_if(arc->inputArcs.cbegin(), arc->inputArcs.cend(), [](sScElementInfo * modifierArc) {
              return modifierArc->source->addr == keynode_rrel_key_sc_element;
            }) != arc->inputArcs.cend())
        {
          keynodes.insert(arc->target);
        }
      });
      // if there are no key elements, we do not collect structure elements
      if (keynodes.empty())
        continue;

      sScElementInfo::tScElementInfoList removableArcs;
      for (auto const & elInfoOutputArc : elInfo->outputArcs)
      {
        // find structure elements by arc_pos_const_perm without modifiers
        if (!(elInfoOutputArc->type & sc_type_arc_pos_const_perm && elInfoOutputArc->inputArcs.empty()))
          continue;

        structureElementIt = elInfoOutputArc->target->inputArcs.find(elInfoOutputArc);
        if (structureElementIt != elInfoOutputArc->target->inputArcs.end())
        {
          // remove arc from source and target
          elInfo->structureElements.insert(elInfoOutputArc->target);
          elInfoOutputArc->target->inputArcs.erase(structureElementIt);
          removableArcs.insert(elInfoOutputArc);
        }
      }
      for (auto const & arc : removableArcs)
      {
        elInfo->outputArcs.erase(arc);
      }

      // get keyword from key elements
      elInfo->structKeyword = findStructKeyword(keynodes);
    }
  }
}

sScElementInfo * uiSc2SCnJsonTranslator::findStructKeyword(sScElementInfo::tScElementInfoList const & structureElements)
{
  sScElementInfo::tScElementInfoList structures;
  sScElementInfo::tScElementInfoList elements = structureElements;
  // first try to find structures in key elements
  std::for_each(structureElements.cbegin(), structureElements.cend(), [&structures](sScElementInfo * el) {
    if (el->type & sc_type_node_struct)
      structures.insert(el);
  });
  if (!structures.empty())
    elements = structures;

  auto result = std::max_element(elements.begin(), elements.end(), [](sScElementInfo * left, sScElementInfo * right) {
    return (left->outputArcs.size() + left->inputArcs.size()) < (right->outputArcs.size() + right->inputArcs.size());
  });
  if (result != elements.end())
    return *result;

  return nullptr;
}

void uiSc2SCnJsonTranslator::json(sScElementInfo * elInfo, int level, bool isStruct, sc_json & result)
{
  bool isFullLinkedNodes = true;

  getBaseInfo(elInfo, result);
  if (!elInfo->structureElements.empty())
  {
    structureElements = elInfo->structureElements;
    json(elInfo->structKeyword, 0, true, result[scnTranslatorConstants::STRUCT.data()]);
  }
  // if node is arc
  if (elInfo->type & sc_type_arc_mask)
  {
    getBaseInfo(elInfo->source, result[scnTranslatorConstants::SOURCE_NODE.data()]);
    getBaseInfo(elInfo->target, result[scnTranslatorConstants::TARGET_NODE.data()]);
  }
  // if node is link
  if (elInfo->type & sc_type_link)
  {
    isFullLinkedNodes = false;
    sc_addr format;
    String content;
    // get format of link
    for (const std::string_view & formatStr : scnTranslatorConstants::formats)
    {
      sc_helper_resolve_system_identifier(s_default_ctx, formatStr.data(), &format);
      if (sc_helper_check_arc(s_default_ctx, elInfo->addr, format, sc_type_arc_common | sc_type_const) == SC_TRUE)
      {
        result[scnTranslatorConstants::CONTENT_TYPE.data()] = formatStr.data();
        break;
      }
    }
    // if the link has no format, then we get the content of the link
    if (result[scnTranslatorConstants::CONTENT_TYPE.data()].is_null())
    {
      sc_stream * stream;
      sc_memory_get_link_content(s_default_ctx, elInfo->addr, &stream);
      if (stream != nullptr && ScStreamConverter::StreamToString(std::make_shared<ScStream>(stream), content))
      {
        result[scnTranslatorConstants::CONTENT.data()] = content;
      }
      else
      {
        result[scnTranslatorConstants::CONTENT.data()] = sc_json();
      }
      result[scnTranslatorConstants::CONTENT_TYPE.data()] = scnTranslatorConstants::FORMAT_TXT.data();
    }
  }
  // if the nesting level is not greater than the maximum or the element is not a tuple, get childrens
  if (level < maxLevel || (elInfo->type & sc_type_node && elInfo->type & sc_type_node_tuple))
  {
    // first get childrens from ordered list of modifiers
    for (sc_addr modifier : mOrderList)
    {
      getChildrenByModifierAddr(elInfo, modifier, isStruct, result[scnTranslatorConstants::CHILDREN.data()]);
    }
    // than get the remaining childrens
    getChildrens(elInfo, isStruct, result[scnTranslatorConstants::CHILDREN.data()]);
    if (isFullLinkedNodes)
      getJsonOfLinkedNodes(result[scnTranslatorConstants::CHILDREN.data()], level + 1, isStruct);
  }
}

void uiSc2SCnJsonTranslator::getChildrens(sScElementInfo * elInfo, bool isStruct, sc_json & childrens)
{
  sScElementInfo::tScElementInfoList outputArcs = elInfo->outputArcs;
  sScElementInfo::tScElementInfoList inputArcs = elInfo->inputArcs;

  getChildrensByDirection(outputArcs, scnTranslatorConstants::RIGHT.data(), isStruct, childrens);
  getChildrensByDirection(inputArcs, scnTranslatorConstants::LEFT.data(), isStruct, childrens);
}

void uiSc2SCnJsonTranslator::getChildrensByDirection(
    sScElementInfo::tScElementInfoList const & arcs,
    String const & direction,
    bool isStruct,
    sc_json & childrens)
{
  for (auto const & arc : arcs)
  {
    sc_json children;
    getChildren(arc, direction, isStruct, children);
    if (children.is_null())
      continue;

    auto & modifiers = children[scnTranslatorConstants::MODIFIERS.data()];
    if (modifiers[0].is_null())
    {
      modifiers = sc_json();
      childrens.push_back(children);
    }
    else
    {
      // if modifier of children is not null try to find children with that modifier in result childrens array
      sc_json const & modifierAddr = modifiers[0][scnTranslatorConstants::ADDR.data()];
      auto result = std::find_if(childrens.begin(), childrens.end(), [modifierAddr](sc_json const & c) {
        auto const & modifiers = c[scnTranslatorConstants::MODIFIERS.data()];
        return (
            !modifiers.is_null() && modifiers[0].contains(scnTranslatorConstants::ADDR.data()) &&
            modifierAddr == modifiers[0][scnTranslatorConstants::ADDR.data()]);
      });
      // if such element is found then merge childrens
      if (result == childrens.end())
      {
        childrens.push_back(children);
      }
      else
      {
        (*result)[scnTranslatorConstants::ARCS.data()].push_back(children[scnTranslatorConstants::ARCS.data()][0]);
        (*result)[scnTranslatorConstants::LINKED_NODES.data()].push_back(
            children[scnTranslatorConstants::LINKED_NODES.data()][0]);

        if (!modifiers[0][scnTranslatorConstants::MODIFIER_ARCS.data()].empty())
        {
          (*result)[scnTranslatorConstants::MODIFIERS.data()][0][scnTranslatorConstants::MODIFIER_ARCS.data()]
              .push_back(modifiers[0][scnTranslatorConstants::MODIFIER_ARCS.data()][0]);
        }
      }
    }
  }
}

void uiSc2SCnJsonTranslator::getJsonOfLinkedNodes(sc_json & childrens, int level, bool isStruct)
{
  sc_addr keynode_nrel_sc_text_translation, linkedNodeAddr;
  sc_helper_resolve_system_identifier(s_default_ctx, "nrel_sc_text_translation", &keynode_nrel_sc_text_translation);

  for (auto & children : childrens)
  {
    // reduce level for rrel_key_sc_element and nrel_sc_text_translation arc nodes
    auto const & modifiers = children[scnTranslatorConstants::MODIFIERS.data()];
    if (!modifiers.is_null() && modifiers[0].contains(scnTranslatorConstants::ADDR.data()))
    {
      sc_addr_hash modifier_addr_hash = modifiers[0][scnTranslatorConstants::ADDR.data()].get<sc_addr_hash>();
      if (modifier_addr_hash == SC_ADDR_LOCAL_TO_INT(keynode_nrel_sc_text_translation) ||
          modifier_addr_hash == SC_ADDR_LOCAL_TO_INT(keynode_rrel_key_sc_element))
      {
        --level;
      }
    }
    for (size_t i = 0; i < children[scnTranslatorConstants::LINKED_NODES.data()].size(); ++i)
    {
      sc_json linkedNode = children[scnTranslatorConstants::LINKED_NODES.data()][i];
      sc_addr_hash const & addr_hash = linkedNode[scnTranslatorConstants::ADDR.data()].get<sc_addr_hash>();
      linkedNodeAddr = {SC_ADDR_LOCAL_SEG_FROM_INT(addr_hash), SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_hash)};

      sScElementInfo * linkedNodeInfo = mScElementsInfo[linkedNodeAddr];
      if (!(linkedNodeInfo->type & sc_type_node_struct))
      {
        json(linkedNodeInfo, level, isStruct, linkedNode);
      }

      children[scnTranslatorConstants::LINKED_NODES.data()][i] = linkedNode;
    }
  }
}

void uiSc2SCnJsonTranslator::getBaseInfo(sScElementInfo * elInfo, sc_json & result)
{
  if (!elInfo)
    return;

  result[scnTranslatorConstants::ADDR.data()] = std::stoi(uiTranslateFromSc::buildId(elInfo->addr));
  String idtf;
  bool idtf_exists = ui_translate_get_identifier(elInfo->addr, mOutputLanguageAddr, idtf);
  if (idtf_exists)
  {
    result[scnTranslatorConstants::IDTF.data()] = idtf;
  }
  else
  {
    result[scnTranslatorConstants::IDTF.data()] = sc_json();
  }
  result[scnTranslatorConstants::TYPE.data()] = elInfo->type;
}

void uiSc2SCnJsonTranslator::getChildrenByModifierAddr(
    sScElementInfo * elInfo,
    sc_addr modifierAddr,
    bool isStruct,
    sc_json & childrens)
{
  sc_json fullChildren;
  sScElementInfo::tScElementInfoList filtered;
  std::for_each(elInfo->outputArcs.cbegin(), elInfo->outputArcs.cend(), [modifierAddr, &filtered](sScElementInfo * el) {
    if (std::find_if(el->inputArcs.cbegin(), el->inputArcs.cend(), [modifierAddr](sScElementInfo * modifierArc) {
          return modifierArc->source->addr == modifierAddr;
        }) != el->inputArcs.cend())
    {
      filtered.insert(el);
    }
  });

  for (sScElementInfo * arcInfo : filtered)
  {
    sc_json children;
    getChildren(arcInfo, scnTranslatorConstants::RIGHT.data(), isStruct, children);
    if (children.is_null())
      continue;

    if (fullChildren[scnTranslatorConstants::ARCS.data()].is_null())
    {
      fullChildren[scnTranslatorConstants::ARCS.data()] = children[scnTranslatorConstants::ARCS.data()];
    }
    else
    {
      fullChildren[scnTranslatorConstants::ARCS.data()].push_back(children[scnTranslatorConstants::ARCS.data()][0]);
    }
    if (fullChildren[scnTranslatorConstants::LINKED_NODES.data()].is_null())
    {
      fullChildren[scnTranslatorConstants::LINKED_NODES.data()] = children[scnTranslatorConstants::LINKED_NODES.data()];
    }
    else
    {
      fullChildren[scnTranslatorConstants::LINKED_NODES.data()].push_back(
          children[scnTranslatorConstants::LINKED_NODES.data()][0]);
    }
    if (fullChildren[scnTranslatorConstants::MODIFIERS.data()].is_null())
    {
      fullChildren[scnTranslatorConstants::MODIFIERS.data()] =
          !children[scnTranslatorConstants::MODIFIERS.data()][0].is_null()
              ? children[scnTranslatorConstants::MODIFIERS.data()]
              : sc_json();
    }
    else
    {
      if (!children[scnTranslatorConstants::MODIFIERS.data()][0][scnTranslatorConstants::MODIFIER_ARCS.data()].empty())
      {
        fullChildren[scnTranslatorConstants::MODIFIERS.data()][0][scnTranslatorConstants::MODIFIER_ARCS.data()]
            .push_back(
                children[scnTranslatorConstants::MODIFIERS.data()][0][scnTranslatorConstants::MODIFIER_ARCS.data()][0]);
      }
    }
  }
  if (!fullChildren.is_null())
    childrens.push_back(fullChildren);

  fullChildren = sc_json();
  filtered.clear();
  std::for_each(
      elInfo->inputArcs.cbegin(), elInfo->inputArcs.cend(), [modifierAddr, &filtered](sScElementInfo * modifierArc) {
        if (std::find_if(
                modifierArc->inputArcs.cbegin(),
                modifierArc->inputArcs.cend(),
                [modifierAddr](sScElementInfo * modifierArc) {
                  return modifierArc->source->addr == modifierAddr;
                }) != modifierArc->inputArcs.cend())
        {
          filtered.insert(modifierArc);
        }
      });
  for (sScElementInfo * arcInfo : filtered)
  {
    sc_json children;
    getChildren(arcInfo, scnTranslatorConstants::LEFT.data(), isStruct, children);
    if (children.is_null())
      continue;

    if (fullChildren[scnTranslatorConstants::ARCS.data()].is_null())
    {
      fullChildren[scnTranslatorConstants::ARCS.data()] = children[scnTranslatorConstants::ARCS.data()];
    }
    else
    {
      fullChildren[scnTranslatorConstants::ARCS.data()].push_back(children[scnTranslatorConstants::ARCS.data()][0]);
    }
    if (fullChildren[scnTranslatorConstants::LINKED_NODES.data()].is_null())
    {
      fullChildren[scnTranslatorConstants::LINKED_NODES.data()] = children[scnTranslatorConstants::LINKED_NODES.data()];
    }
    else
    {
      fullChildren[scnTranslatorConstants::LINKED_NODES.data()].push_back(
          children[scnTranslatorConstants::LINKED_NODES.data()][0]);
    }
    if (fullChildren[scnTranslatorConstants::MODIFIERS.data()].is_null())
    {
      fullChildren[scnTranslatorConstants::MODIFIERS.data()] =
          !children[scnTranslatorConstants::MODIFIERS.data()][0].is_null()
              ? children[scnTranslatorConstants::MODIFIERS.data()]
              : sc_json();
    }
    else
    {
      if (!children[scnTranslatorConstants::MODIFIERS.data()][0][scnTranslatorConstants::MODIFIER_ARCS.data()].empty())
      {
        fullChildren[scnTranslatorConstants::MODIFIERS.data()][0][scnTranslatorConstants::MODIFIER_ARCS.data()]
            .push_back(
                children[scnTranslatorConstants::MODIFIERS.data()][0][scnTranslatorConstants::MODIFIER_ARCS.data()][0]);
      }
    }
  }
  if (!fullChildren.is_null())
    childrens.push_back(fullChildren);
}

void uiSc2SCnJsonTranslator::getChildren(
    sScElementInfo * arcInfo,
    String const & direction,
    bool isStruct,
    sc_json & children)
{
  // skip if arc is already passed
  if (arcInfo->isInTree)
    return;
  // skip if arc is struct element
  if (isStruct && (structureElements.find(arcInfo) == structureElements.cend()))
    return;
  arcInfo->isInTree = true;

  sScElementInfo * linkedNode = nullptr;
  if (direction == scnTranslatorConstants::RIGHT.data())
  {
    linkedNode = arcInfo->target;
  }
  else if (direction == scnTranslatorConstants::LEFT.data())
  {
    linkedNode = arcInfo->source;
  }

  sc_json arc;
  getBaseInfo(arcInfo, arc);
  arc[scnTranslatorConstants::DIRECTION.data()] = direction;

  sScElementInfo::tScElementInfoList modifiersList = arcInfo->inputArcs;

  auto modifierIt = modifiersList.begin();
  sc_json modifier = sc_json();
  if (modifierIt != modifiersList.end() && !((*modifierIt)->isInTree))
  {
    getBaseInfo((*modifierIt)->source, modifier);
    sc_json modifierEl;
    getBaseInfo(*modifierIt, modifierEl);
    modifier[scnTranslatorConstants::MODIFIER_ARCS.data()].push_back(modifierEl);
    (*modifierIt)->isInTree = true;
  }

  children[scnTranslatorConstants::ARCS.data()].push_back(arc);
  children[scnTranslatorConstants::MODIFIERS.data()].push_back(modifier);
  sc_json linkedNodeEl;
  getBaseInfo(linkedNode, linkedNodeEl);
  children[scnTranslatorConstants::LINKED_NODES.data()].push_back(linkedNodeEl);
}

void uiSc2SCnJsonTranslator::resolveFilterList(sc_addr cmd_addr)
{
  // resolve filter list
  sc_iterator3 * modifierIt;
  sc_iterator5 * it = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      cmd_addr,
      sc_type_arc_pos_const_perm,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_rrel_filter_list);

  while (sc_iterator5_next(it) == SC_TRUE)
  {
    modifierIt = sc_iterator3_f_a_a_new(
        s_default_ctx, sc_iterator5_value(it, 2), sc_type_arc_pos_const_perm, sc_type_node | sc_type_const);
    while (sc_iterator3_next(modifierIt) == SC_TRUE)
    {
      mFiltersList.insert(sc_iterator3_value(modifierIt, 2));
    }
    sc_iterator3_free(modifierIt);
  }

  sc_iterator5_free(it);
}

void uiSc2SCnJsonTranslator::initFilterList()
{
  // init filter list
  sc_iterator3 * it = sc_iterator3_f_a_a_new(
      s_default_ctx,
      keynode_concept_scn_json_elements_filter_set,
      sc_type_arc_pos_const_perm,
      sc_type_node | sc_type_const);

  while (sc_iterator3_next(it) == SC_TRUE)
  {
    mFiltersList.insert(sc_iterator3_value(it, 2));
  }
  sc_iterator3_free(it);
}

void uiSc2SCnJsonTranslator::initOrderList()
{
  sc_addr keynode_rrel_1, elementArc = {};
  sc_iterator3 * elementIt;
  sc_helper_resolve_system_identifier(s_default_ctx, "rrel_1", &keynode_rrel_1);
  sc_iterator5 * it = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      keynode_concept_scn_json_elements_order_set,
      sc_type_arc_pos_const_perm,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_rrel_1);

  if (sc_iterator5_next(it) == SC_TRUE)
  {
    mOrderList.push_back(sc_iterator5_value(it, 2));
    elementArc = getNextElementArc(sc_iterator5_value(it, 1));
    while (!SC_ADDR_IS_EMPTY(elementArc))
    {
      elementIt = sc_iterator3_f_f_a_new(
          s_default_ctx, keynode_concept_scn_json_elements_order_set, elementArc, sc_type_node | sc_type_const);
      if (sc_iterator3_next(elementIt) == SC_TRUE)
      {
        mOrderList.push_back(sc_iterator3_value(elementIt, 2));
        elementArc = getNextElementArc(elementArc);
      }
      sc_iterator3_free(elementIt);
    }
  }
  sc_iterator5_free(it);
}

sc_addr uiSc2SCnJsonTranslator::getNextElementArc(sc_addr elementArc)
{
  sc_addr result = {};
  sc_iterator5 * nextIt = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      elementArc,
      sc_type_arc_common | sc_type_const,
      sc_type_arc_pos_const_perm,
      sc_type_arc_pos_const_perm,
      keynode_nrel_scn_json_elements_base_order);
  if (sc_iterator5_next(nextIt) == SC_TRUE)
    result = sc_iterator5_value(nextIt, 2);

  sc_iterator5_free(nextIt);
  return result;
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
    translator.initFilterList();
    translator.initOrderList();
    translator.resolveFilterList(cmd_addr);
    translator.translate(input_addr, format_addr, lang_addr);
  }

  return SC_RESULT_OK;
}
