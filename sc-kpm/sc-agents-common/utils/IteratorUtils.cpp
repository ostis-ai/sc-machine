/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "CommonUtils.hpp"

#include <sc-memory/sc_iterator.hpp>

#include "IteratorUtils.hpp"

using namespace std;

namespace utils
{

ScAddr IteratorUtils::getFirstFromSet(ScMemoryContext * ms_context, const ScAddr & set,  bool get_strictly_first)
{
  ScAddr element;
  if (get_strictly_first)
  {
    ScIterator5Ptr iterator5 = ms_context->Iterator5(
          set,
          ScType::EdgeAccessConstPosPerm,
          ScType::Unknown,
          ScType::EdgeAccessConstPosPerm,
          scAgentsCommon::CoreKeynodes::rrel_1);
    if (iterator5->Next())
    {
      element = iterator5->Get(2);
    }
  }
  else
  {
    ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
    if (iterator3->Next())
    {
        element = iterator3->Get(2);
    }
  }
  return element;
}

ScAddr IteratorUtils::getNextFromSet(
      ScMemoryContext * ms_context,
      const ScAddr & set,
      const ScAddr & previous,
      const ScAddr & sequenceRelation)
{
  ScAddr element;
  std::string const NEXT_ELEMENT_ALIAS = "_next_element";
  std::string const NEXT_ELEMENT_ACCESS_ARC_ALIAS = "_next_element_access_arc";
  std::string const PREVIOUS_ELEMENT_ACCESS_ARC_ALIAS = "_previous_element_access_arc";

  ScTemplate scTemplate;
  scTemplate.Triple(
        set,
        ScType::EdgeAccessVarPosPerm >> PREVIOUS_ELEMENT_ACCESS_ARC_ALIAS,
        previous);
  scTemplate.Triple(
        set,
        ScType::EdgeAccessVarPosPerm >> NEXT_ELEMENT_ACCESS_ARC_ALIAS,
        ScType::NodeVar >> NEXT_ELEMENT_ALIAS);
  scTemplate.TripleWithRelation(
        PREVIOUS_ELEMENT_ACCESS_ARC_ALIAS,
        ScType::EdgeDCommonVar,
        NEXT_ELEMENT_ACCESS_ARC_ALIAS,
        ScType::EdgeAccessVarPosPerm,
        sequenceRelation);
  ScTemplateSearchResult searchResult;
  ms_context->HelperSearchTemplate(scTemplate, searchResult);
  if (searchResult.Size() == 1)
  {
    element = searchResult[0][NEXT_ELEMENT_ALIAS];
  }
  else if (searchResult.Size() > 1)
  {
    throw std::runtime_error("Found several sets of rules following the previous one. Error in the sequence.");
  }

  return element;
}

vector<ScAddr> IteratorUtils::getAllWithType(ScMemoryContext * ms_context, const ScAddr & set, ScType scType)
{
  vector<ScAddr> elementList;
  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, scType);
  while (iterator3->Next())
  {
    elementList.push_back(iterator3->Get(2));
  }
  return elementList;
}

vector<ScAddr> IteratorUtils::getAllByInRelation(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddr & relation)
{
  vector<ScAddr> elementList;
  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation, false);
  while (iterator5->Next())
  {
    elementList.push_back(iterator5->Get(0));
  }
  return elementList;
}


ScAddr IteratorUtils::getFirstByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation)
{
  ScAddr element;
  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation, false);
  if (iterator5->Next())
  {
    element = iterator5->Get(0);
  }
  return element;
}

ScAddr IteratorUtils::getFirstByOutRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation)
{
  ScAddr element;
  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation);
  if (iterator5->Next())
  {
    element = iterator5->Get(2);
  }
  return element;
}

ScIterator5Ptr IteratorUtils::getIterator5(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScAddr const & relation,
      bool const nodeIsStart)
{
  bool isRole = CommonUtils::checkType(ms_context, relation, ScType::NodeConstRole);
  ScType arcType = isRole ? ScType::EdgeAccessConstPosPerm : ScType::EdgeDCommonConst;

  ScIterator5Ptr iterator5;
  if (nodeIsStart)
  {
    iterator5 = ms_context->Iterator5(node, arcType, ScType::Unknown, ScType::EdgeAccessConstPosPerm, relation);
  }
  else
  {
    iterator5 = ms_context->Iterator5(ScType::Unknown, arcType, node, ScType::EdgeAccessConstPosPerm, relation);
  }
  return iterator5;
}

}
