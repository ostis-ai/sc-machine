/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_type.hpp>
#include <sc-memory/sc_iterator.hpp>

using namespace std;

namespace utils
{

class IteratorUtils
{
public:
  static ScAddr getFirstFromSet(ScMemoryContext * ms_context, const ScAddr & set);

  static vector<ScAddr> getAllWithType(ScMemoryContext * ms_context, const ScAddr & set, ScType scType);

  static vector<ScAddr> getAllByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  static ScAddr getFirstByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  static ScAddr getFirstByOutRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  static ScIterator5Ptr getIterator5(
        ScMemoryContext * ms_context,
        const ScAddr & node,
        const ScAddr & relation,
        const bool nodeIsStart = true);

};
}
