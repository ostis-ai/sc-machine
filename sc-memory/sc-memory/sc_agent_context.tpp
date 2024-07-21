/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_agent_context.hpp"

template <class TScEvent>
std::shared_ptr<ScWaitCondition<TScEvent>> ScAgentContext::InitializeEvent(
    ScAddr const & subscriptionAddr,
    std::function<void(void)> const & cause,
    std::function<sc_result(TScEvent const &)> check) const
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  cause();
  return std::make_shared<ScWaitCondition<TScEvent>>(*this, subscriptionAddr, check);
}
