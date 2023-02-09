/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _uiSc2SCnJsonTranslator_h_
#define _uiSc2SCnJsonTranslator_h_

#include "uiTranslatorFromSc.h"
#include "uiTranslators.h"

struct sScElementInfo
{
  typedef std::list<sScElementInfo *> tScElementInfoList;

  sc_type type;
  sc_addr addr;
  sc_addr srcAddr;
  sc_addr trgAddr;
  sScElementInfo * source;
  sScElementInfo * target;
  sScElementInfo * structKeyword;
  tScElementInfoList outputArcs;
  tScElementInfoList inputArcs;
  tScElementInfoList structureElements;

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

  static sc_result ui_translate_sc2scn(const sc_event * event, sc_addr arg);

protected:
  //! @copydoc uiTranslateFromSc::runImpl
  void runImpl();

  //! Collect information of trsnslated sc-elements and store it
  void collectScElementsInfo();

  // Generate json for specified element
  sc_json json(sScElementInfo * elInfo, int level = 0, bool isStruct = false);

  // Get childrens for specified element
  sc_json getChildrens(sScElementInfo * elInfo, bool isFullLinkedNodes, int level = 1, bool isStruct = false);

  // Get childrens by direction for specified arcs list
  sc_json getChildrensByDirection(sScElementInfo::tScElementInfoList arcs, String direction, bool isStruct);

  // Get base json information about specified element
  sc_json getBaseInfo(sScElementInfo * elInfo);

  // Group childrens by modifiers
  sc_json groupChildrensByModifier(sc_json childrens);

  sc_json getJsonOfLinkedNodes(sc_json childrens, int level = 1, bool isStruct = false);

  bool getLinkContent(sc_addr link_addr, String & content);

  sScElementInfo * findStructKeyword(sScElementInfo::tScElementInfoList structureElements);

private:
  //! List of keywords
  tScAddrList mKeywordsList;
  //! Collection of objects information
  typedef std::map<sc_addr, sScElementInfo *> tScElemetsInfoMap;
  tScElemetsInfoMap mScElementsInfo;
  //! Pull of sc-elements information (used to prevent many memory allocations)
  sScElementInfo * mScElementsInfoPool;

  sScElementInfo::tScElementInfoList structureElements;
  //max level of full discripted node
  const int maxLevel = 2;
};

#endif  // _uiSc2SCnJsonTranslator_h_
