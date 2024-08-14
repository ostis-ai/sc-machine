/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_context.hpp"

#include <algorithm>

#include "sc_event_subscription.hpp"

#include "sc_action.hpp"
#include "sc_keynodes.hpp"

ScAgentContext::ScAgentContext() noexcept
  : ScAgentContext(ScAddr::Empty)
{
}

ScAgentContext::ScAgentContext(sc_memory_context * context) noexcept
  : ScMemoryContext(context)
{
}

ScAgentContext::ScAgentContext(ScAddr const & userAddr) noexcept
  : ScMemoryContext(userAddr)
{
}

ScAgentContext::ScAgentContext(ScAgentContext && other) noexcept
  : ScMemoryContext(std::move(other)) {};

ScAgentContext & ScAgentContext::operator=(ScAgentContext && other) noexcept
{
  if (this == &other)
    return *this;

  ScMemoryContext::operator=(std::move(other));
  return *this;
}

std::shared_ptr<ScElementaryEventSubscription<ScElementaryEvent>> ScAgentContext::CreateEventSubscription(
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionAddr,
    std::function<void(ScElementaryEvent const &)> const & eventCallback) const
{
  if (!IsElement(eventClassAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create elementary sc-event subscription due sc-event class is not valid.");

  if (!HelperCheckEdge(ScKeynodes::sc_event, eventClassAddr, ScType::EdgeAccessConstPosPerm))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create elementary sc-event subscription due sc-event class is not belongs to `sc_event`.");

  if (!IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create elementary sc-event subscription due subscription sc-element is not valid.");

  if (eventClassAddr == ScKeynodes::sc_event_change_content && !GetElementType(subscriptionAddr).IsLink())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of changing link content due subscription sc-element is not "
        "sc-link.");

  return std::shared_ptr<ScElementaryEventSubscription<ScElementaryEvent>>(
      new ScElementaryEventSubscription<ScElementaryEvent>(*this, eventClassAddr, subscriptionAddr, eventCallback));
}

ScAction ScAgentContext::CreateAction(ScAddr const & actionClassAddr)
{
  ScAction action{this, actionClassAddr};
  return action;
}

ScAction ScAgentContext::UseAction(ScAddr const & actionAddr)
{
  ScAction action{this, actionAddr, ScAddr::Empty};
  return action;
}

ScSet ScAgentContext::CreateSet()
{
  ScSet set{this};
  return set;
}

ScSet ScAgentContext::UseSet(ScAddr const & setAddr)
{
  ScSet set{this, setAddr};
  return set;
}

ScStructure ScAgentContext::CreateStructure()
{
  ScStructure structure{this};
  return structure;
}

ScStructure ScAgentContext::UseStructure(ScAddr const & structureAddr)
{
  ScStructure structure{this, structureAddr};
  return structure;
}
