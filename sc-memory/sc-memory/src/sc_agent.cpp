/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_agent.hpp"

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_keynodes.hpp"

ScActionInitiatedAgent::ScActionInitiatedAgent() noexcept = default;

ScAddr ScActionInitiatedAgent::GetEventSubscriptionElement() const
{
  return ScKeynodes::action_initiated;
}

ScTemplate ScActionInitiatedAgent::GetInitiationConditionTemplate(ScActionInitiatedEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(GetActionClass(), ScType::VarPermPosArc, event.GetOtherElement());
  return templ;
}
