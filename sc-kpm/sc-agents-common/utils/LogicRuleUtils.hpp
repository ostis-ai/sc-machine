/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/sc_addr.hpp>

using namespace std;

namespace utils
{

class LogicRuleUtils
{
public:
  static ScAddr getIfStatement(ScMemoryContext * context, ScAddr const & logicRule);

  static ScAddr getElseStatement(ScMemoryContext * context, ScAddr const & logicRule);
};
}
