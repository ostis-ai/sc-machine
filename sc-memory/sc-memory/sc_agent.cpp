/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent.hpp"

#include "sc_event.hpp"
#include "sc_keynodes.hpp"

sc_bool ScActionAgent::CheckInitiationCondition(ScActionEvent const & event)
{
  return ScMemory::ms_globalContext->HelperCheckEdge(
      GetActionClass(), event.GetOtherElement(), ScType::EdgeAccessConstPosPerm);
}
