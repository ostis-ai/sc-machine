/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_context.hpp"

#include "sc_event_wait.hpp"

template <class TScEvent>
std::shared_ptr<ScElementaryEventSubscription<TScEvent>> ScAgentContext::CreateEventSubscription(
    ScAddr const & subscriptionElementAddr,
    std::function<void(TScEvent const &)> const & eventCallback) const
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription due subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create sc-event subscription of changing link content due subscription sc-element is not "
          "sc-link.");
  }

  return std::shared_ptr<ScElementaryEventSubscription<TScEvent>>(
      new ScElementaryEventSubscription<TScEvent>(*this, subscriptionElementAddr, eventCallback));
}

template <class TScEvent>
std::shared_ptr<ScWait> ScAgentContext::CreateEventWait(
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback) const
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Not able to create sc-event waiter due subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create sc-event waiter of changing link content due subscription sc-element is not "
          "sc-link.");
  }

  auto eventWait = std::shared_ptr<ScWaitEvent<TScEvent>>(new ScWaitEvent<TScEvent>(*this, subscriptionElementAddr));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

template <class TScEvent>
std::shared_ptr<ScWait> ScAgentContext::CreateEventWaitWithCondition(
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback,
    std::function<sc_bool(TScEvent const &)> const & checkCallback) const
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event waiter with condition due subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create sc-event waiter with condition of changing link content due subscription sc-element is "
          "not "
          "sc-link.");
  }

  auto eventWait = std::shared_ptr<ScWaitCondition<TScEvent>>(
      new ScWaitCondition<TScEvent>(*this, subscriptionElementAddr, checkCallback));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

template <class TScEvent>
std::shared_ptr<ScWait> ScAgentContext::CreateEventWaitWithCondition(
    ScAddr const & subscriptionElementAddr,
    std::function<sc_bool(TScEvent const &)> const & checkCallback) const
{
  return CreateEventWaitWithCondition(subscriptionElementAddr, {}, checkCallback);
}
