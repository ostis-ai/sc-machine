/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _uiSc2SCnJsonTranslator_h_
#define _uiSc2SCnJsonTranslator_h_

#include "uiTranslatorFromSc.h"
#include "uiTranslators.h"

struct ScStructureElementInfo
{
  typedef std::unordered_set<ScStructureElementInfo *> ScStructureElementInfoList;

  sc_type type;
  sc_addr addr;
  ScStructureElementInfo * sourceInfo;
  ScStructureElementInfo * targetInfo;
  ScStructureElementInfo * structKeyword;
  ScStructureElementInfoList outputArcs;
  ScStructureElementInfoList inputArcs;
  ScStructureElementInfoList structureElements;

  bool isInTree;
};

/*!
 * \brief Class that translates sc-construction into
 * SCn-code (json representation)
 */
class uiSc2SCnJsonTranslator : public uiTranslateFromSc
{
public:
  explicit uiSc2SCnJsonTranslator();
  virtual ~uiSc2SCnJsonTranslator();

  static sc_result ui_translate_sc2scn(sc_event_subscription const * event_subscription, sc_addr arg);

protected:
  //! @copydoc uiTranslateFromSc::runImpl
  void runImpl() override;

  //! Collect information of translated sc-elements and store it
  void CollectScStructureElementsInfo();

  //! Collect sc-structure elements and store it
  void ResolveStructure(sc_addr structure_addr);

  //! Generate json for specified element
  void ParseScnJsonSentence(ScStructureElementInfo * elInfo, int level, bool isStruct, ScJson & result);

  //!
  void ParseScnJsonArc(ScStructureElementInfo * elInfo, ScJson & result);

  //!
  void ParseScnJsonLink(ScStructureElementInfo * elInfo, ScJson & result);

  //! Get children for specified element
  void ParseChildrenScnJson(ScStructureElementInfo * elInfo, bool isStruct, ScJson & children);

  //! Get children by direction for specified arcs list
  void ParseChildrenScnJsonByDirection(
      ScStructureElementInfo::ScStructureElementInfoList const & arcs,
      String const & direction,
      bool isStruct,
      ScJson & children);

  //! Get base json information about specified element
  void ParseScElementInfo(ScStructureElementInfo * elInfo, ScJson & result);

  //! get full json of linked nodes for specified children
  void ParseLinkedNodesScnJson(ScJson & children, int level = 1, bool isStruct = false);

  ScStructureElementInfo * ResolveStructureElementInfo(sc_addr const & addr, sc_type type = 0);

  //! Find struct keyword in specified elements list
  static ScStructureElementInfo * FindStructureKeyword(
      ScStructureElementInfo::ScStructureElementInfoList const & structureElements);

  //! Get children for specified modifier
  void ParseChildrenScnJsonByModifier(
      ScStructureElementInfo * elInfo,
      sc_addr modifierAddr,
      bool isStruct,
      ScJson & children);

  void UpdateChildArcs(
      ScStructureElementInfo::ScStructureElementInfoList const & arcs,
      bool isStruct,
      ScJson & fullChild,
      String const & direction);

  //! Get json of arc
  void ParseScnJsonChild(ScStructureElementInfo * arcInfo, String const & direction, bool isStruct, ScJson & child);

  //! Resolve additional filter elements for specified cmd_addr
  void ResolveFilterList(sc_addr);

  //! Get default ordered list of modifiers
  void InitOrderList();

  //! Get next element from ordered set
  sc_addr GetNextElementArc(sc_addr elementArc);

  //! Get default filter list
  void InitFilterList();

private:
  //! List of keywords
  tScAddrSet mKeywordsList;
  //! List of elements to filter
  tScAddrSet mFilterList;
  //! Ordered list of modifiers
  tScAddrList mOrderList;
  //! Collection of objects information
  typedef std::map<sc_addr, ScStructureElementInfo *> tScElemetsInfoMap;
  tScElemetsInfoMap mStructureElementsInfo;
  //! Store structure elements if keyword is struct to remove them from keyword childrens
  ScStructureElementInfo::ScStructureElementInfoList structureElements;
  //! Max level of full discripted node
  int const maxLevel = 3;
};

#endif  // _uiSc2SCnJsonTranslator_h_
