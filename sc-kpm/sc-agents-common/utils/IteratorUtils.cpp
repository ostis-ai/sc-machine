/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc-memory/cpp/sc_iterator.hpp>
#include <sc-memory/cpp/sc_memory.hpp>

#include "IteratorUtils.hpp"
#include "CommonUtils.hpp"

using namespace std;

namespace utils
{

ScAddr IteratorUtils::getFirstFromSet(ScMemoryContext * ms_context, ScAddr const & set)
{
  ScAddr element;
  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Node);
  if (iterator3->Next())
  {
    element = iterator3->Get(2);
  }
  return element;
}

vector<ScAddr> IteratorUtils::getAllWithType(ScMemoryContext * ms_context, ScAddr const & set, ScType const & scType)
{
  vector<ScAddr> elementList;
  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, scType);
  while (iterator3->Next())
  {
    elementList.push_back(iterator3->Get(2));
  }
  return elementList;
}

vector<ScAddr>
IteratorUtils::getAllByInRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation)
{
  vector<ScAddr> elementList;
  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation, false);
  while (iterator5->Next())
  {
    elementList.push_back(iterator5->Get(0));
  }
  return elementList;
}


ScAddr IteratorUtils::getFirstByInRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation)
{
  ScAddr element;
  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation, false);
  if (iterator5->Next())
  {
    element = iterator5->Get(0);
  }
  return element;
}

ScAddr IteratorUtils::getFirstByOutRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation)
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
      bool nodeIsStart)
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
