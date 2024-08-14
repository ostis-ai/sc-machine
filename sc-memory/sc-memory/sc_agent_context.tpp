/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_context.hpp"

#include "sc_wait.hpp"

template <class TScEvent>
std::shared_ptr<ScElementaryEventSubscription<TScEvent>> ScAgentContext::CreateEventSubscription(
    ScAddr const & subscriptionAddr,
    std::function<void(TScEvent const &)> const & eventCallback) const
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription due subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create sc-event subscription of changing link content due subscription sc-element is not "
          "sc-link.");
  }

  return std::shared_ptr<ScElementaryEventSubscription<TScEvent>>(
      new ScElementaryEventSubscription<TScEvent>(*this, subscriptionAddr, eventCallback));
}
