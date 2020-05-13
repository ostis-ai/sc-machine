/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>

#include <sc-memory/cpp/sc_addr.hpp>
#include <sc-memory/cpp/sc_iterator.hpp>

using namespace std;

namespace utils
{

class IteratorUtils
{
public:
  static ScAddr getFirstFromSet(ScMemoryContext * ms_context, ScAddr const & set);

  static vector<ScAddr> getAllWithType(ScMemoryContext * ms_context, ScAddr const & set, ScType const & scType);

  static vector<ScAddr> getAllByInRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation);

  static ScAddr getFirstByInRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation);

  static ScAddr getFirstByOutRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation);

  static ScIterator5Ptr getIterator5(
        ScMemoryContext * ms_context,
        ScAddr const & node,
        ScAddr const & relation,
        bool nodeIsStart = true);

  static bool addSetToOutline(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & outline);

  static bool addNodeWithOutRelationToOutline(
        ScMemoryContext * ms_context,
        ScAddr const & node,
        ScAddr const & relation,
        ScAddr const & outline);
};
}
