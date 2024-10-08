/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_event_wait.hpp"

#include "sc-memory/sc_keynodes.hpp"

#include "sc-memory/sc_timer.hpp"

template <class TScEvent>
ScEventWaiter<TScEvent>::ScEventWaiter(ScMemoryContext const & context, ScAddr const & subscriptionElementAddr) noexcept
  : m_eventSubscription(
        context,
        subscriptionElementAddr,
        [this](TScEvent const & event)
        {
          if (OnEvent(event))
            ScWaiter::Resolve();
        })
{
}

template <class TScEvent>
ScEventWaiter<TScEvent>::ScEventWaiter(
    ScMemoryContext const & context,
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr) noexcept
  : m_eventSubscription(
        context,
        eventClassAddr,
        subscriptionElementAddr,
        [this](ScElementaryEvent const & event)
        {
          if (OnEvent(event))
            ScWaiter::Resolve();
        })
{
}

template <class TScEvent>
bool ScEventWaiter<TScEvent>::OnEvent(TScEvent const &)
{
  return true;
}

template <class TScEvent>
ScConditionWaiter<TScEvent>::ScConditionWaiter(
    ScMemoryContext const & context,
    ScAddr const & subscriptionElementAddr,
    DelegateCheckFunc const & func) noexcept
  : ScEventWaiter<TScEvent>(context, subscriptionElementAddr)
  , m_checkFunc(std::move(func))
{
}

template <class TScEvent>
ScConditionWaiter<TScEvent>::ScConditionWaiter(
    ScMemoryContext const & context,
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr,
    DelegateCheckFunc const & func) noexcept
  : ScEventWaiter<TScEvent>(context, eventClassAddr, subscriptionElementAddr)
  , m_checkFunc(std::move(func))
{
}

template <class TScEvent>
bool ScConditionWaiter<TScEvent>::OnEvent(TScEvent const & event)
{
  return m_checkFunc(event);
}
