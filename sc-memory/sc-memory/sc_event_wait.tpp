/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_event_wait.hpp"

#include "sc_keynodes.hpp"

#include "sc_timer.hpp"

template <class TScEvent>
ScWaitEvent<TScEvent>::ScWaitEvent(ScMemoryContext const & ctx, ScAddr const & subscriptionElementAddr)
  : m_event(
        ctx,
        subscriptionElementAddr,
        [this](TScEvent const & event)
        {
          if (OnEvent(event))
            ScWait::Resolve();
        })
{
}

template <class TScEvent>
ScWaitEvent<TScEvent>::ScWaitEvent(
    ScMemoryContext const & ctx,
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr)
  : m_event(
        ctx,
        eventClassAddr,
        subscriptionElementAddr,
        [this](ScElementaryEvent const & event)
        {
          if (OnEvent(event))
            ScWait::Resolve();
        })
{
}

template <class TScEvent>
sc_bool ScWaitEvent<TScEvent>::OnEvent(TScEvent const &)
{
  return SC_TRUE;
}

template <class TScEvent>
ScWaitCondition<TScEvent>::ScWaitCondition(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionElementAddr,
    DelegateCheckFunc const & func)
  : ScWaitEvent<TScEvent>(ctx, subscriptionElementAddr)
  , m_checkFunc(std::move(func))
{
}

template <class TScEvent>
ScWaitCondition<TScEvent>::ScWaitCondition(
    ScMemoryContext const & ctx,
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionElementAddr,
    DelegateCheckFunc const & func)
  : ScWaitEvent<TScEvent>(ctx, eventClassAddr, subscriptionElementAddr)
  , m_checkFunc(std::move(func))
{
}

template <class TScEvent>
sc_bool ScWaitCondition<TScEvent>::OnEvent(TScEvent const & event)
{
  return m_checkFunc(event);
}
