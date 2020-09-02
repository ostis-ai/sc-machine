/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "LogicRuleUtils.hpp"

#include "IteratorUtils.hpp"
#include "keynodes/coreKeynodes.hpp"


using namespace std;
using namespace scAgentsCommon;

namespace utils
{

ScAddr LogicRuleUtils::getIfStatement(ScMemoryContext * context, const ScAddr & logicRule)
{
  ScAddr ifStatement;
  ScAddr implEdge;
  implEdge = IteratorUtils::getFirstByOutRelation(context, logicRule, CoreKeynodes::rrel_main_key_sc_element);
  if (context->HelperCheckEdge(CoreKeynodes::nrel_implication, implEdge, ScType::EdgeAccessConstPosPerm))
    ifStatement = context->GetEdgeSource(implEdge);
  return ifStatement;
}

ScAddr LogicRuleUtils::getElseStatement(ScMemoryContext * context, const ScAddr & logicRule)
{
  ScAddr implEdge;
  ScAddr elseStatement;
  implEdge = IteratorUtils::getFirstByOutRelation(context, logicRule, CoreKeynodes::rrel_main_key_sc_element);
  if (context->HelperCheckEdge(CoreKeynodes::nrel_implication, implEdge, ScType::EdgeAccessConstPosPerm))
    elseStatement = context->GetEdgeTarget(implEdge);
  return elseStatement;
}


}
