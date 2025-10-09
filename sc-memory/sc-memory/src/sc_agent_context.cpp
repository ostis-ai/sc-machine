/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_agent_context.hpp"

#include <algorithm>

#include "sc-memory/sc_event_subscription.hpp"

#include "sc-memory/sc_action.hpp"
#include "sc-memory/sc_keynodes.hpp"
#include "sc-memory/sc_oriented_set.hpp"

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

// clang-format off
ScAgentContext::ScAgentContext(ScAgentContext && other) noexcept
  : ScMemoryContext(std::move(other)) {};
// clang-format on

ScAgentContext & ScAgentContext::operator=(ScAgentContext && other) noexcept
{
  if (this == &other)
    return *this;

  ScMemoryContext::operator=(std::move(other));
  return *this;
}

std::shared_ptr<ScElementaryEventSubscription<ScElementaryEvent>> ScAgentContext::CreateElementaryEventSubscription(
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr,
    std::function<void(ScElementaryEvent const &)> const & eventCallback) noexcept(false)
{
  ValidateEventElements(eventClassAddr, subscriptionElementAddr, "elementary sc-event subscription");

  return std::shared_ptr<ScElementaryEventSubscription<ScElementaryEvent>>(
      new ScElementaryEventSubscription<ScElementaryEvent>(
          *this, eventClassAddr, subscriptionElementAddr, eventCallback));
}

std::shared_ptr<ScWaiter> ScAgentContext::CreateEventWaiter(
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback) noexcept(false)
{
  ValidateEventElements(eventClassAddr, subscriptionElementAddr, "sc-event waiter");

  auto eventWait = std::shared_ptr<ScEventWaiter<ScElementaryEvent>>(
      new ScEventWaiter<ScElementaryEvent>(*this, eventClassAddr, subscriptionElementAddr));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

std::shared_ptr<ScWaiter> ScAgentContext::CreateConditionWaiter(
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback,
    std::function<bool(ScElementaryEvent const &)> const & checkCallback) noexcept(false)
{
  ValidateEventElements(eventClassAddr, subscriptionElementAddr, "condition waiter");

  auto eventWait = std::shared_ptr<ScConditionWaiter<ScElementaryEvent>>(
      new ScConditionWaiter<ScElementaryEvent>(*this, eventClassAddr, subscriptionElementAddr, checkCallback));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

std::shared_ptr<ScWaiter> ScAgentContext::CreateConditionWaiter(
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr,
    std::function<bool(ScElementaryEvent const &)> const & checkCallback) noexcept(false)
{
  return CreateConditionWaiter(eventClassAddr, subscriptionElementAddr, {}, checkCallback);
}

ScAction ScAgentContext::GenerateAction(ScAddr const & actionClassAddr) noexcept(false)
{
  if (!IsElement(actionClassAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to generate sc-action with action class `" << actionClassAddr
                                                             << "`, because action class is not valid.");

  ScAddr const & actionAddr = GenerateNode(ScType::ConstNode);
  GenerateConnector(ScType::ConstPermPosArc, actionClassAddr, actionAddr);
  ScAction action{this, actionAddr};
  return action;
}

ScAction ScAgentContext::ConvertToAction(ScAddr const & actionAddr) noexcept(false)
{
  if (!IsElement(actionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to convert sc-action sc-address `"
            << actionAddr << "` to object of `ScAction` class, because its sc-address is not valid.");

  ScAction action{this, actionAddr};
  return action;
}

ScSet ScAgentContext::GenerateSet()
{
  ScAddr const & setAddr = GenerateNode(ScType::ConstNode);
  ScSet set{this, setAddr};
  return set;
}

ScSet ScAgentContext::ConvertToSet(ScAddr const & setAddr) noexcept(false)
{
  if (!IsElement(setAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to convert sc-set sc-address `"
            << setAddr << "` to object of `ScSet` class, because its sc-address is not valid.");

  ScSet set{this, setAddr};
  return set;
}

ScOrientedSet ScAgentContext::GenerateOrientedSet()
{
  ScAddr const & setAddr = GenerateNode(ScType::ConstNode);
  ScOrientedSet set{this, setAddr};
  return set;
}

ScOrientedSet ScAgentContext::ConvertToOrientedSet(ScAddr const & setAddr) noexcept(false)
{
  if (!IsElement(setAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to convert sc-oriented-set sc-address `"
            << setAddr << "` to object of `ScOrientedSet` class, because its sc-address is not valid.");

  ScOrientedSet set{this, setAddr};
  return set;
}

ScStructure ScAgentContext::GenerateStructure()
{
  ScAddr const & structureAddr = GenerateNode(ScType::ConstNodeStructure);
  ScStructure structure{this, structureAddr};
  return structure;
}

ScStructure ScAgentContext::ConvertToStructure(ScAddr const & structureAddr) noexcept(false)
{
  if (!IsElement(structureAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to convert sc-structure sc-address `"
            << structureAddr << "` to object of `ScStructure` class, because its sc-address is not valid.");

  ScStructure structure{this, structureAddr};
  return structure;
}

void ScAgentContext::ValidateEventElements(
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr,
    std::string const & validatorName)
{
  if (!IsElement(eventClassAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create " << validatorName << " because sc-event class is not valid.");

  if (!CheckConnector(ScKeynodes::sc_event, eventClassAddr, ScType::ConstPermPosArc))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create " << validatorName
                              << " because sc-event class is not belongs to "
                                 "`sc_event`.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create " << validatorName << " because subscription sc-element is not valid.");

  if (eventClassAddr == ScKeynodes::sc_event_before_change_link_content
      && !GetElementType(subscriptionElementAddr).IsLink())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create " << validatorName
                              << " of changing link content because subscription sc-element is "
                                 "not sc-link.");
}
