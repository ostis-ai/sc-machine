/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>

#include <sc-memory/cpp/sc_addr.hpp>

using namespace std;

namespace utils
{

class CommonUtils
{
public:
  static bool checkType(ScMemoryContext * ms_context, ScAddr const & node, ScType scType);

  static int readInt(ScMemoryContext * ms_context, ScAddr const & scLink);

  static int readNumber(ScMemoryContext * ms_context, ScAddr const & number);

  static string readString(ScMemoryContext * ms_context, ScAddr const & scLink);

  static string getIdtfValue(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & idtfRelation);

  static int getPowerOfSet(ScMemoryContext * ms_context, ScAddr const & set);
};
}
