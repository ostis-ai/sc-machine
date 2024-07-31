/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent.hpp"

#include "sc_event.hpp"
#include "sc_keynodes.hpp"

ScActionAgent::ScActionAgent() = default;

ScTemplate ScActionAgent::GetInitiationCondition() const
{
  ScTemplate templ;
  templ.Triple(GetActionClass(), ScType::EdgeAccessVarPosPerm, ScType::NodeVar);
  return templ;
}
