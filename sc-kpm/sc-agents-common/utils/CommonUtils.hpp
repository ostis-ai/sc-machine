/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>

#include <sc-memory/sc_addr.hpp>

using namespace std;

namespace utils
{

class CommonUtils
{
public:
  static bool checkType(ScMemoryContext * ms_context, const ScAddr & node, ScType scType);

  static int readInt(ScMemoryContext * ms_context, const ScAddr & scLink);

  static int readNumber(ScMemoryContext * ms_context, const ScAddr & number);

  static string readString(ScMemoryContext * ms_context, const ScAddr & scLink);

  static string getIdtfValue(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & idtfRelation);

  static int getPowerOfSet(ScMemoryContext * ms_context, const ScAddr & set);
};
}
