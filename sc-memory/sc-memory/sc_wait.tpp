/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_wait.hpp"

#include "sc_keynodes.hpp"

#include "sc_timer.hpp"

template <class TScEvent>
ScWaitEvent<TScEvent>::ScWaitEvent(ScMemoryContext const & ctx, ScAddr const & subscriptionAddr)
  : m_event(
        ctx,
        subscriptionAddr,
        [this](TScEvent const & event)
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
    ScAddr const & subscriptionAddr,
    DelegateCheckFunc func)
  : ScWaitEvent<TScEvent>(ctx, subscriptionAddr)
  , m_checkFunc(std::move(func))
{
}

template <class TScEvent>
sc_bool ScWaitCondition<TScEvent>::OnEvent(TScEvent const & event)
{
  return m_checkFunc(event);
}
