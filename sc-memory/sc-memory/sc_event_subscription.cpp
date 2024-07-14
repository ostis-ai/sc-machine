/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_subscription.hpp"

ScEventSubscription::~ScEventSubscription() = default;

sc_result ScEventSubscription::Handler(sc_event const *, sc_addr, sc_addr, sc_type, sc_addr)
{
  return SC_RESULT_OK;
}

sc_result ScEventSubscription::HandlerDelete(sc_event const *)
{
  return SC_RESULT_OK;
}

sc_result ScEventSubscription::Initialize(ScMemoryContext *, ScAddr const &)
{
  return SC_RESULT_OK;
}

sc_result ScEventSubscription::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}

ScEventSubscriptionAddOutputEdge::ScEventSubscriptionAddOutputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventAddOutputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionAddInputEdge::ScEventSubscriptionAddInputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventAddInputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionRemoveOutputEdge::ScEventSubscriptionRemoveOutputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventRemoveOutputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionRemoveInputEdge::ScEventSubscriptionRemoveInputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventRemoveInputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionEraseElement::ScEventSubscriptionEraseElement(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventRemoveElement const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionContentChanged::ScEventSubscriptionContentChanged(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventChangeContent const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}
