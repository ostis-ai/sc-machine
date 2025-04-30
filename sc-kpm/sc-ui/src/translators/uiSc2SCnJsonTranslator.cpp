/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiSc2SCnJsonTranslator.h"

#include <algorithm>
#include <string_view>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_keynodes.hpp>

extern "C"
{
#include <sc-core/sc_helper.h>
}

#include "uiTranslators.h"
#include "uiKeynodes.h"
#include "uiUtils.h"

namespace ScnTranslatorConstants
{
static std::string_view const STRUCT{"struct"};
static std::string_view const SOURCE_NODE{"sourceNode"};
static std::string_view const TARGET_NODE{"targetNode"};
static std::string_view const CHILDREN{"children"};
static std::string_view const CONTENT{"content"};
static std::string_view const CONTENT_TYPE{"contentType"};
static std::string_view const MODIFIERS{"modifiers"};
static std::string_view const MODIFIER_ARCS{"modifierArcs"};
static std::string_view const RIGHT{"right"};
static std::string_view const LEFT{"left"};
static std::string_view const ADDR{"addr"};
static std::string_view const ARCS{"arcs"};
static std::string_view const LINKED_NODES{"linkedNodes"};
static std::string_view const IDTF{"idtf"};
static std::string_view const TYPE{"type"};
static std::string_view const DIRECTION{"direction"};
static std::string_view const FORMAT_TXT{"format_txt"};
static std::string_view const FORMAT_HTML{"format_html"};
static std::string_view const FORMAT_LARGE_TXT{"format_large_txt"};
static std::string_view const formats[]{"format_html", "format_github_source_link", "format_pdf", "format_png"};
static tScAddrToStringMap formats_keynodes;

static size_t const FORMAT_LARGE_TXT_SIZE = 100;
};  // namespace ScnTranslatorConstants

uiSc2SCnJsonTranslator::uiSc2SCnJsonTranslator() = default;

uiSc2SCnJsonTranslator::~uiSc2SCnJsonTranslator()
{
  std::for_each(
      mStructureElementsInfo.begin(),
      mStructureElementsInfo.end(),
      [](std::pair<sc_addr, ScStructureElementInfo *> pair)
      {
        delete pair.second;
        pair.second = nullptr;
      });
}

void uiSc2SCnJsonTranslator::runImpl()
{
  if (ScnTranslatorConstants::formats_keynodes.empty())
  {
    for (std::string_view const & formatStr : ScnTranslatorConstants::formats)
    {
      sc_addr format_keynode;
      sc_helper_resolve_system_identifier(s_default_ctx, formatStr.data(), &format_keynode);
      ScnTranslatorConstants::formats_keynodes.emplace(format_keynode, formatStr);
    }
  }

  // get command arguments
  sc_iterator5 * it5 = sc_iterator5_a_a_f_a_f_new(
      s_default_ctx,
      sc_type_node | sc_type_const,
      sc_type_const_common_arc,
      mInputConstructionAddr,
      sc_type_const_perm_pos_arc,
      keynode_action_nrel_result);
  if (sc_iterator5_next(it5) == SC_TRUE)
  {
    sc_iterator3 * it3 =
        sc_iterator3_f_a_a_new(s_default_ctx, sc_iterator5_value(it5, 0), sc_type_const_perm_pos_arc, 0);
    while (sc_iterator3_next(it3) == SC_TRUE)
    {
      sc_addr const keyword = sc_iterator3_value(it3, 2);
      ResolveStructure(keyword);
      mKeywordsList.insert(keyword);
    }
    sc_iterator3_free(it3);
  }
  sc_iterator5_free(it5);

  CollectScStructureElementsInfo();

  ScJson results;
  for (auto const & keyword : mKeywordsList)
  {
    if (!mStructureElementsInfo[keyword])
      continue;
    ScJson result;
    ParseScnJsonSentence(mStructureElementsInfo[keyword], 0, false, result);
    results.push_back(result);
  }
  mOutputData = results.dump();
}

void uiSc2SCnJsonTranslator::CollectScStructureElementsInfo()
{
  // now we need to iterate all arcs and collect output/incoming sc-arcs info
  // first collect information about elements
  std::set<sc_addr> filtered;
  for (auto const & it : mEdges)
  {
    sc_addr const arcAddr = it.first;

    // get begin/end addrs
    sc_addr begAddr;
    if (sc_memory_get_arc_begin(s_default_ctx, arcAddr, &begAddr) != SC_RESULT_OK)
      continue;  // @todo process errors
    sc_addr endAddr;
    if (sc_memory_get_arc_end(s_default_ctx, arcAddr, &endAddr) != SC_RESULT_OK)
      continue;  // @todo process errors

    if (std::any_of(
            mFilterList.cbegin(),
            mFilterList.cend(),
            [begAddr](sc_addr const & modifier)
            {
              return modifier == begAddr;
            }))
    {
      filtered.insert(arcAddr);
      filtered.insert(endAddr);
    }

    ScStructureElementInfo * elInfo = ResolveStructureElementInfo(it.first, it.second);
    ScStructureElementInfo * begInfo = ResolveStructureElementInfo(begAddr);
    ScStructureElementInfo * endInfo = ResolveStructureElementInfo(endAddr);

    elInfo->sourceInfo = begInfo;
    elInfo->targetInfo = endInfo;
  }

  for (auto const & it : mEdges)
  {
    if (filtered.find(it.first) != filtered.cend())
      continue;

    ScStructureElementInfo * edgeInfo = mStructureElementsInfo[it.first];
    edgeInfo->sourceInfo->outputArcs.insert(mStructureElementsInfo[it.first]);
    edgeInfo->targetInfo->inputArcs.insert(mStructureElementsInfo[it.first]);
  }
}

void uiSc2SCnJsonTranslator::ResolveStructure(sc_addr structure_addr)
{
  // find structures elements
  ScStructureElementInfo::ScStructureElementInfoList::const_iterator structureElementIt;
  ScStructureElementInfo * elInfo = ResolveStructureElementInfo(structure_addr);
  if (sc_type_has_subtype(elInfo->type, sc_type_node_structure))
  {
    // get the key elements of the structure
    ScStructureElementInfo::ScStructureElementInfoList keynodes;
    std::for_each(
        elInfo->outputArcs.cbegin(),
        elInfo->outputArcs.cend(),
        [&keynodes](ScStructureElementInfo * arc)
        {
          if (std::find_if(
                  arc->inputArcs.cbegin(),
                  arc->inputArcs.cend(),
                  [](ScStructureElementInfo * modifierArc)
                  {
                    return modifierArc->sourceInfo->addr == keynode_rrel_key_sc_element;
                  })
              != arc->inputArcs.cend())
          {
            keynodes.insert(arc->targetInfo);
          }
        });
    // if there are no key elements, we do not collect structure elements
    if (keynodes.empty())
      return;

    ScStructureElementInfo::ScStructureElementInfoList removableArcs;
    for (auto const & elInfoOutputArc : elInfo->outputArcs)
    {
      // find structure elements by const_perm_pos_arc without modifiers
      if (sc_type_has_not_subtype(elInfoOutputArc->type, sc_type_const_perm_pos_arc))
        continue;

      elInfo->structureElements.insert(elInfoOutputArc->targetInfo);

      structureElementIt = elInfoOutputArc->targetInfo->inputArcs.find(elInfoOutputArc);
      if (structureElementIt != elInfoOutputArc->targetInfo->inputArcs.end())
      {
        // remove arc from source and target
        elInfoOutputArc->targetInfo->inputArcs.erase(structureElementIt);
        removableArcs.insert(elInfoOutputArc);
      }
    }
    for (auto const & arc : removableArcs)
    {
      elInfo->outputArcs.erase(arc);
    }

    // get keyword from key elements
    elInfo->structKeyword = FindStructureKeyword(keynodes);
  }
}

ScStructureElementInfo * uiSc2SCnJsonTranslator::ResolveStructureElementInfo(sc_addr const & addr, sc_type type)
{
  ScStructureElementInfo * elInfo = mStructureElementsInfo[addr];
  if (elInfo)
    return elInfo;

  if (type == 0)
    sc_memory_get_element_type(s_default_ctx, addr, &type);

  elInfo = new ScStructureElementInfo();
  elInfo->addr = addr;
  elInfo->type = type;
  elInfo->isInTree = false;
  mStructureElementsInfo[addr] = elInfo;

  return elInfo;
}

ScStructureElementInfo * uiSc2SCnJsonTranslator::FindStructureKeyword(
    ScStructureElementInfo::ScStructureElementInfoList const & structureElements)
{
  ScStructureElementInfo::ScStructureElementInfoList structures;
  ScStructureElementInfo::ScStructureElementInfoList elements = structureElements;
  // first try to find structures in key elements
  std::for_each(
      structureElements.cbegin(),
      structureElements.cend(),
      [&structures](ScStructureElementInfo * el)
      {
        if (sc_type_has_subtype(el->type, sc_type_node_structure))
          structures.insert(el);
      });
  if (!structures.empty())
    elements = structures;

  auto result = std::max_element(
      elements.begin(),
      elements.end(),
      [](ScStructureElementInfo * left, ScStructureElementInfo * right)
      {
        return (left->outputArcs.size() + left->inputArcs.size())
               < (right->outputArcs.size() + right->inputArcs.size());
      });
  if (result != elements.end())
    return *result;

  return nullptr;
}

void uiSc2SCnJsonTranslator::ParseScnJsonArc(ScStructureElementInfo * elInfo, ScJson & result)
{
  ParseScElementInfo(elInfo->sourceInfo, result[ScnTranslatorConstants::SOURCE_NODE.data()]);
  ParseScElementInfo(elInfo->targetInfo, result[ScnTranslatorConstants::TARGET_NODE.data()]);
}

void uiSc2SCnJsonTranslator::ParseScnJsonLink(ScStructureElementInfo * elInfo, ScJson & result)
{
  String content;
  // get format of link
  ScJson & contentType = result[ScnTranslatorConstants::CONTENT_TYPE.data()];
  // if the link has no format, then we get the content of the link
  sc_stream * stream;
  sc_memory_get_link_content(s_default_ctx, elInfo->addr, &stream);
  if (stream != nullptr && ScStreamConverter::StreamToString(std::make_shared<ScStream>(stream), content))
  {
    if (content.size() < ScnTranslatorConstants::FORMAT_LARGE_TXT_SIZE)
    {
      result[ScnTranslatorConstants::CONTENT.data()] = content;
      contentType = ScnTranslatorConstants::FORMAT_HTML.data();
    }
    else
    {
      // TODO(kilativ-dotcom): replace with sc_helper_check_arc when it'll support both direction while checking
      sc_iterator3 * iterator_for_format =
          sc_iterator3_f_a_a_new(s_default_ctx, elInfo->addr, sc_type_const_common_arc, sc_type_const_node);
      while (sc_iterator3_next(iterator_for_format) == SC_TRUE)
      {
        sc_addr const possible_format = sc_iterator3_value(iterator_for_format, 2);
        auto const format_iterator = ScnTranslatorConstants::formats_keynodes.find(possible_format);
        if (format_iterator != ScnTranslatorConstants::formats_keynodes.cend())
        {
          contentType = format_iterator->second.data();
          break;
        }
      }
      sc_iterator3_free(iterator_for_format);

      if (contentType.is_null())
      {
        result[ScnTranslatorConstants::CONTENT.data()] = ScJson();
        contentType = ScnTranslatorConstants::FORMAT_LARGE_TXT.data();
      }
    }
  }
  else
  {
    result[ScnTranslatorConstants::CONTENT.data()] = ScJson();
    contentType = ScnTranslatorConstants::FORMAT_TXT.data();
  }
}

void uiSc2SCnJsonTranslator::ParseScnJsonSentence(
    ScStructureElementInfo * elInfo,
    int level,
    bool isStruct,
    ScJson & result)
{
  bool isFullLinkedNodes = true;

  ParseScElementInfo(elInfo, result);
  if (!elInfo->structureElements.empty())
  {
    structureElements = elInfo->structureElements;
    ParseScnJsonSentence(elInfo->structKeyword, 0, true, result[ScnTranslatorConstants::STRUCT.data()]);
  }
  // if node is arc
  if (sc_type_is_connector(elInfo->type))
  {
    ParseScnJsonArc(elInfo, result);
  }
  // if node is link
  if (sc_type_is_node_link(elInfo->type))
  {
    isFullLinkedNodes = false;
    ParseScnJsonLink(elInfo, result);
  }
  // if the nesting level is not greater than the maximum or the element is not a tuple, get children
  if (level < maxLevel || sc_type_has_subtype(elInfo->type, sc_type_node_tuple))
  {
    auto & resultChildren = result[ScnTranslatorConstants::CHILDREN.data()];
    // first get children from ordered list of modifiers
    for (sc_addr modifier : mOrderList)
    {
      ParseChildrenScnJsonByModifier(elInfo, modifier, isStruct, resultChildren);
    }
    // then get the remaining children
    ParseChildrenScnJson(elInfo, isStruct, resultChildren);
    if (isFullLinkedNodes)
      ParseLinkedNodesScnJson(resultChildren, level + 1, isStruct);
  }
}

void uiSc2SCnJsonTranslator::ParseChildrenScnJson(ScStructureElementInfo * elInfo, bool isStruct, ScJson & children)
{
  ParseChildrenScnJsonByDirection(elInfo->outputArcs, ScnTranslatorConstants::RIGHT.data(), isStruct, children);
  ParseChildrenScnJsonByDirection(elInfo->inputArcs, ScnTranslatorConstants::LEFT.data(), isStruct, children);
}

void uiSc2SCnJsonTranslator::ParseChildrenScnJsonByDirection(
    ScStructureElementInfo::ScStructureElementInfoList const & arcs,
    String const & direction,
    bool isStruct,
    ScJson & children)
{
  std::list<ScStructureElementInfo *> orderedArcs;
  bool isTupleNotOriented = true;
  if (arcs.size() > 0)
  {
    ScStructureElementInfo * someArc = *arcs.cbegin();
    orderedArcs.push_back(someArc);

    ScIterator5Ptr it5 = ScMemory::ms_globalContext->CreateIterator5(
        ScType::Unknown,
        ScType::ConstCommonArc,
        ScAddr(someArc->addr),
        ScType::ConstPermPosArc,
        ScKeynodes::nrel_basic_sequence);
    while (it5->Next())
    {
      isTupleNotOriented = false;
      ScAddr const & prevArcAddr = it5->Get(0);

      auto it = std::find_if(
          arcs.cbegin(),
          arcs.cend(),
          [&](ScStructureElementInfo * someArc) -> bool
          {
            return ScAddr(someArc->addr) == prevArcAddr;
          });
      if (it == arcs.cend())
        break;

      orderedArcs.push_front(*it);

      it5 = ScMemory::ms_globalContext->CreateIterator5(
          ScType::Unknown,
          ScType::ConstCommonArc,
          ScAddr((*it)->addr),
          ScType::ConstPermPosArc,
          ScKeynodes::nrel_basic_sequence);
    }

    it5 = ScMemory::ms_globalContext->CreateIterator5(
        ScAddr(someArc->addr),
        ScType::ConstCommonArc,
        ScType::Unknown,
        ScType::ConstPermPosArc,
        ScKeynodes::nrel_basic_sequence);
    while (it5->Next())
    {
      isTupleNotOriented = false;
      ScAddr const & nextArcAddr = it5->Get(2);

      auto it = std::find_if(
          arcs.cbegin(),
          arcs.cend(),
          [&](ScStructureElementInfo * someArc) -> bool
          {
            return ScAddr(someArc->addr) == nextArcAddr;
          });
      if (it == arcs.cend())
        break;

      orderedArcs.push_back(*it);

      it5 = ScMemory::ms_globalContext->CreateIterator5(
          ScAddr((*it)->addr),
          ScType::ConstCommonArc,
          ScType::Unknown,
          ScType::ConstPermPosArc,
          ScKeynodes::nrel_basic_sequence);
    }
  }

  if (orderedArcs.empty() || isTupleNotOriented)
  {
    orderedArcs.clear();
    orderedArcs.assign(arcs.cbegin(), arcs.cend());
  }

  for (auto const & arc : orderedArcs)
  {
    ScJson child;
    ParseScnJsonChild(arc, direction, isStruct, child);
    if (child.is_null())
      continue;

    auto & modifiers = child[ScnTranslatorConstants::MODIFIERS.data()];
    if (modifiers[0].is_null())
    {
      modifiers = ScJson();
      children.push_back(child);
    }
    else
    {
      // if modifier of child is not null try to find child with that modifier in result children array
      ScJson const & modifierAddr = modifiers[0][ScnTranslatorConstants::ADDR.data()];
      auto result = std::find_if(
          children.begin(),
          children.end(),
          [modifierAddr](ScJson const & c)
          {
            auto const & modifiers = c[ScnTranslatorConstants::MODIFIERS.data()];
            return (
                !modifiers.is_null() && modifiers[0].contains(ScnTranslatorConstants::ADDR.data())
                && modifierAddr == modifiers[0][ScnTranslatorConstants::ADDR.data()]);
          });
      // if such element is found then merge children
      if (result == children.end())
      {
        children.push_back(child);
      }
      else
      {
        (*result)[ScnTranslatorConstants::ARCS.data()].push_back(child[ScnTranslatorConstants::ARCS.data()][0]);
        (*result)[ScnTranslatorConstants::LINKED_NODES.data()].push_back(
            child[ScnTranslatorConstants::LINKED_NODES.data()][0]);

        auto const & modifierArcs = modifiers[0][ScnTranslatorConstants::MODIFIER_ARCS.data()];
        if (!modifierArcs.empty())
        {
          (*result)[ScnTranslatorConstants::MODIFIERS.data()][0][ScnTranslatorConstants::MODIFIER_ARCS.data()]
              .push_back(modifierArcs[0]);
        }
      }
    }
  }
}

void uiSc2SCnJsonTranslator::ParseLinkedNodesScnJson(ScJson & children, int level, bool isStruct)
{
  for (auto & child : children)
  {
    // reduce level for rrel_key_sc_element and nrel_sc_text_translation arc nodes
    auto const & modifiers = child[ScnTranslatorConstants::MODIFIERS.data()];
    if (!modifiers.is_null() && modifiers[0].contains(ScnTranslatorConstants::ADDR.data()))
    {
      sc_addr_hash modifier_addr_hash = modifiers[0][ScnTranslatorConstants::ADDR.data()].get<sc_addr_hash>();
      if (modifier_addr_hash == SC_ADDR_LOCAL_TO_INT(keynode_nrel_sc_text_translation)
          || modifier_addr_hash == SC_ADDR_LOCAL_TO_INT(keynode_rrel_key_sc_element))
      {
        --level;
      }
    }

    auto & linked_nodes = child[ScnTranslatorConstants::LINKED_NODES.data()];
    for (size_t i = 0; i < linked_nodes.size(); ++i)
    {
      ScJson linkedNode = linked_nodes[i];
      sc_addr_hash const & addr_hash = linkedNode[ScnTranslatorConstants::ADDR.data()].get<sc_addr_hash>();
      sc_addr const & linkedNodeAddr = {
          SC_ADDR_LOCAL_SEG_FROM_INT(addr_hash), SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_hash)};

      ScStructureElementInfo * linkedNodeInfo = mStructureElementsInfo[linkedNodeAddr];
      ParseScnJsonSentence(linkedNodeInfo, level, isStruct, linkedNode);

      child[ScnTranslatorConstants::LINKED_NODES.data()][i] = linkedNode;
    }
  }
}

void uiSc2SCnJsonTranslator::ParseScElementInfo(ScStructureElementInfo * elInfo, ScJson & result)
{
  if (!elInfo)
    return;

  result[ScnTranslatorConstants::ADDR.data()] = std::stoi(uiTranslateFromSc::buildId(elInfo->addr));
  String idtf;
  bool idtf_exists = ui_translate_get_identifier(elInfo->addr, mOutputLanguageAddr, idtf);
  if (idtf_exists)
  {
    result[ScnTranslatorConstants::IDTF.data()] = idtf;
  }
  else
  {
    result[ScnTranslatorConstants::IDTF.data()] = ScJson();
  }
  result[ScnTranslatorConstants::TYPE.data()] = elInfo->type;

  ResolveStructure(elInfo->addr);
}

void uiSc2SCnJsonTranslator::UpdateChildArcs(
    ScStructureElementInfo::ScStructureElementInfoList const & arcs,
    bool isStruct,
    ScJson & fullChild,
    String const & direction)
{
  for (ScStructureElementInfo * arcInfo : arcs)
  {
    ScJson child;
    ParseScnJsonChild(arcInfo, direction, isStruct, child);
    if (child.is_null())
      continue;

    ScJson & fullChildArcs = fullChild[ScnTranslatorConstants::ARCS.data()];
    if (fullChildArcs.is_null())
    {
      fullChildArcs = child[ScnTranslatorConstants::ARCS.data()];
    }
    else
    {
      fullChildArcs.push_back(child[ScnTranslatorConstants::ARCS.data()][0]);
    }

    ScJson & fullChildLinkedNodes = fullChild[ScnTranslatorConstants::LINKED_NODES.data()];
    if (fullChildLinkedNodes.is_null())
    {
      fullChildLinkedNodes = child[ScnTranslatorConstants::LINKED_NODES.data()];
    }
    else
    {
      fullChildLinkedNodes.push_back(child[ScnTranslatorConstants::LINKED_NODES.data()][0]);
    }

    ScJson & fullChildModifiers = fullChild[ScnTranslatorConstants::MODIFIERS.data()];
    if (fullChildModifiers.is_null())
    {
      ScJson const & modifiers = child[ScnTranslatorConstants::MODIFIERS.data()];
      fullChildModifiers = !modifiers.empty() ? modifiers : ScJson();
    }
    else
    {
      ScJson const & childModifierArcs =
          child[ScnTranslatorConstants::MODIFIERS.data()][0][ScnTranslatorConstants::MODIFIER_ARCS.data()];
      if (!childModifierArcs.empty())
      {
        fullChildModifiers[0][ScnTranslatorConstants::MODIFIER_ARCS.data()].push_back(childModifierArcs[0]);
      }
    }
  }
}

void uiSc2SCnJsonTranslator::ParseChildrenScnJsonByModifier(
    ScStructureElementInfo * elInfo,
    sc_addr modifierAddr,
    bool isStruct,
    ScJson & children)
{
  ScStructureElementInfo::ScStructureElementInfoList filtered;
  std::for_each(
      elInfo->outputArcs.cbegin(),
      elInfo->outputArcs.cend(),
      [modifierAddr, &filtered](ScStructureElementInfo * el)
      {
        if (std::find_if(
                el->inputArcs.cbegin(),
                el->inputArcs.cend(),
                [modifierAddr](ScStructureElementInfo * modifierArc)
                {
                  return modifierArc->sourceInfo->addr == modifierAddr;
                })
            != el->inputArcs.cend())
        {
          filtered.insert(el);
        }
      });

  ScJson fullChild;
  UpdateChildArcs(filtered, isStruct, fullChild, ScnTranslatorConstants::RIGHT.data());

  if (!fullChild.is_null())
    children.push_back(fullChild);

  filtered.clear();
  std::for_each(
      elInfo->inputArcs.cbegin(),
      elInfo->inputArcs.cend(),
      [modifierAddr, &filtered](ScStructureElementInfo * modifierArc)
      {
        if (std::find_if(
                modifierArc->inputArcs.cbegin(),
                modifierArc->inputArcs.cend(),
                [modifierAddr](ScStructureElementInfo * modifierArc)
                {
                  return modifierArc->sourceInfo->addr == modifierAddr;
                })
            != modifierArc->inputArcs.cend())
        {
          filtered.insert(modifierArc);
        }
      });
  fullChild = ScJson();
  UpdateChildArcs(filtered, isStruct, fullChild, ScnTranslatorConstants::LEFT.data());

  if (!fullChild.is_null())
    children.push_back(fullChild);
}

void uiSc2SCnJsonTranslator::ParseScnJsonChild(
    ScStructureElementInfo * arcInfo,
    String const & direction,
    bool isStruct,
    ScJson & child)
{
  // skip if arc is already passed
  if (arcInfo->isInTree)
    return;
  // skip if arc is struct element
  if (isStruct && (structureElements.find(arcInfo) == structureElements.cend()))
    return;
  arcInfo->isInTree = true;

  ScStructureElementInfo * linkedNode = nullptr;
  if (direction == ScnTranslatorConstants::RIGHT.data())
  {
    linkedNode = arcInfo->targetInfo;
  }
  else if (direction == ScnTranslatorConstants::LEFT.data())
  {
    linkedNode = arcInfo->sourceInfo;
  }

  ScJson arc;
  ParseScElementInfo(arcInfo, arc);
  arc[ScnTranslatorConstants::DIRECTION.data()] = direction;

  ScStructureElementInfo::ScStructureElementInfoList modifiersList = arcInfo->inputArcs;

  auto modifierIt = modifiersList.begin();
  ScJson modifier = ScJson();
  if (modifierIt != modifiersList.end() && !((*modifierIt)->isInTree)
      && sc_type_has_not_subtype((*modifierIt)->sourceInfo->type, sc_type_node_structure))
  {
    ParseScElementInfo((*modifierIt)->sourceInfo, modifier);
    ScJson modifierEl;
    ParseScElementInfo(*modifierIt, modifierEl);
    modifier[ScnTranslatorConstants::MODIFIER_ARCS.data()].push_back(modifierEl);
    (*modifierIt)->isInTree = true;
  }

  child[ScnTranslatorConstants::ARCS.data()].push_back(arc);
  if (!modifier.is_null())
    child[ScnTranslatorConstants::MODIFIERS.data()].push_back(modifier);
  ScJson linkedNodeEl;
  ParseScElementInfo(linkedNode, linkedNodeEl);
  child[ScnTranslatorConstants::LINKED_NODES.data()].push_back(linkedNodeEl);
}

void uiSc2SCnJsonTranslator::ResolveFilterList(sc_addr cmd_addr)
{
  // resolve filter list
  sc_iterator3 * modifierIt;
  sc_iterator5 * it = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      cmd_addr,
      sc_type_const_perm_pos_arc,
      sc_type_const_node,
      sc_type_const_perm_pos_arc,
      keynode_rrel_filter_list);

  while (sc_iterator5_next(it) == SC_TRUE)
  {
    modifierIt = sc_iterator3_f_a_a_new(
        s_default_ctx, sc_iterator5_value(it, 2), sc_type_const_perm_pos_arc, sc_type_const_node);
    while (sc_iterator3_next(modifierIt) == SC_TRUE)
    {
      mFilterList.insert(sc_iterator3_value(modifierIt, 2));
    }
    sc_iterator3_free(modifierIt);
  }

  sc_iterator5_free(it);
}

void uiSc2SCnJsonTranslator::InitFilterList()
{
  // init filter list
  sc_iterator3 * it = sc_iterator3_f_a_a_new(
      s_default_ctx, keynode_concept_scn_json_elements_filter_set, sc_type_const_perm_pos_arc, sc_type_const_node);

  while (sc_iterator3_next(it) == SC_TRUE)
  {
    mFilterList.insert(sc_iterator3_value(it, 2));
  }
  sc_iterator3_free(it);
}

void uiSc2SCnJsonTranslator::InitOrderList()
{
  sc_addr keynode_rrel_1, elementArc;
  sc_iterator3 * elementIt;
  sc_helper_resolve_system_identifier(s_default_ctx, "rrel_1", &keynode_rrel_1);
  sc_iterator5 * it = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      keynode_concept_scn_json_elements_order_set,
      sc_type_const_perm_pos_arc,
      sc_type_const_node,
      sc_type_const_perm_pos_arc,
      keynode_rrel_1);

  if (sc_iterator5_next(it) == SC_TRUE)
  {
    mOrderList.push_back(sc_iterator5_value(it, 2));
    elementArc = GetNextElementArc(sc_iterator5_value(it, 1));
    while (!SC_ADDR_IS_EMPTY(elementArc))
    {
      elementIt = sc_iterator3_f_f_a_new(
          s_default_ctx, keynode_concept_scn_json_elements_order_set, elementArc, sc_type_const_node);
      if (sc_iterator3_next(elementIt) == SC_TRUE)
      {
        mOrderList.push_back(sc_iterator3_value(elementIt, 2));
        elementArc = GetNextElementArc(elementArc);
      }
      sc_iterator3_free(elementIt);
    }
  }
  sc_iterator5_free(it);
}

sc_addr uiSc2SCnJsonTranslator::GetNextElementArc(sc_addr elementArc)
{
  sc_addr result = {};
  sc_iterator5 * nextIt = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      elementArc,
      sc_type_const_common_arc,
      sc_type_const_perm_pos_arc,
      sc_type_const_perm_pos_arc,
      keynode_nrel_scn_json_elements_base_order);
  if (sc_iterator5_next(nextIt) == SC_TRUE)
    result = sc_iterator5_value(nextIt, 2);

  sc_iterator5_free(nextIt);
  return result;
}

// -------------------------------------
sc_result uiSc2SCnJsonTranslator::ui_translate_sc2scn(sc_event_subscription const *, sc_addr arg)
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
    translator.InitFilterList();
    translator.InitOrderList();
    translator.ResolveFilterList(cmd_addr);
    translator.translate(input_addr, format_addr, lang_addr);
  }

  return SC_RESULT_OK;
}
