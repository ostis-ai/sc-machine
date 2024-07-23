/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <functional>

#include "sc_event.hpp"

#include "utils/sc_lock.hpp"

/* Base class for sc-events
 */
class _SC_EXTERN ScEventSubscription : public ScObject
{
public:
  _SC_EXTERN virtual ~ScEventSubscription();

  /* Set specified function as a delegate that will be calls on event emit */
  template <typename FuncT>
  _SC_EXTERN void SetDelegate(FuncT &&);

  _SC_EXTERN virtual void RemoveDelegate() = 0;

protected:
  static sc_result Handler(sc_event const *, sc_addr, sc_addr, sc_type, sc_addr);

  static sc_result HandlerDelete(sc_event const *);

  sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) final;

  sc_result Shutdown(ScMemoryContext * ctx) final;
};

template <class TScEvent>
class _SC_EXTERN ScElementaryEventSubscription : public ScEventSubscription
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

public:
  using DelegateFunc = std::function<sc_result(TScEvent const & event)>;

  explicit _SC_EXTERN ScElementaryEventSubscription(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func = DelegateFunc());

  virtual _SC_EXTERN ~ScElementaryEventSubscription();

  // Don't allow copying of events
  _SC_EXTERN ScElementaryEventSubscription(ScElementaryEventSubscription const & other) = delete;

  /* Set specified function as a delegate that will be calls on event emit */
  _SC_EXTERN void SetDelegate(DelegateFunc && func);

  _SC_EXTERN void RemoveDelegate();

protected:
  friend class ScMemoryContext;

  _SC_EXTERN static sc_result Handler(
      sc_event const * event,
      sc_addr userAddr,
      sc_addr connectorAddr,
      sc_type connectorType,
      sc_addr otherAddr);

  _SC_EXTERN static sc_result HandlerDelete(sc_event const * event);

private:
  sc_event * m_event;
  DelegateFunc m_delegate;
  utils::ScLock m_lock;
};

class _SC_EXTERN ScEventSubscriptionAddOutputEdge final : public ScElementaryEventSubscription<ScEventAddOutputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionAddOutputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<sc_result(ScEventAddOutputEdge const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionAddInputEdge final : public ScElementaryEventSubscription<ScEventAddInputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionAddInputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<sc_result(ScEventAddInputEdge const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionRemoveOutputEdge final
  : public ScElementaryEventSubscription<ScEventRemoveOutputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveOutputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<sc_result(ScEventRemoveOutputEdge const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionRemoveInputEdge final : public ScElementaryEventSubscription<ScEventRemoveInputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveInputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<sc_result(ScEventRemoveInputEdge const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionEraseElement final : public ScElementaryEventSubscription<ScEventRemoveElement>
{
public:
  _SC_EXTERN ScEventSubscriptionEraseElement(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<sc_result(ScEventRemoveElement const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionContentChanged final : public ScElementaryEventSubscription<ScEventChangeContent>
{
public:
  _SC_EXTERN ScEventSubscriptionContentChanged(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<sc_result(ScEventChangeContent const &)> const & func);
};

template <sc_event_type eventType>
struct _SC_EXTERN ScEventTypeConverter;

template <sc_event_type eventType>
using _SC_EXTERN ScEventSubscriptionType = typename ScEventTypeConverter<eventType>::SubscriptionType;

template <sc_event_type eventType>
using _SC_EXTERN ScEventTypeClass = typename ScEventTypeConverter<eventType>::EventTypeClass;

class _SC_EXTERN ScEventSubscriptionFactory
{
public:
  using ScEventCallback = std::function<sc_result(ScElementaryEvent const &)>;

  static _SC_EXTERN ScEventSubscription * CreateSubscription(
      ScMemoryContext * context,
      ScEvent::Type const & eventType,
      ScAddr const & subscriptionAddr,
      ScEventCallback const & onEventCallback);

  static _SC_EXTERN ScEventSubscription * CreateSubscription(
      ScMemoryContext * context,
      ScAddr const & evenTypeAddr,
      ScAddr const & subscriptionAddr,
      ScEventCallback const & onEventCallback);

  static _SC_EXTERN ScEventSubscription * CreateSubscription(
      ScMemoryContext * context,
      std::string const & eventTypeSystemIdtf,
      ScAddr const & subscriptionAddr,
      ScEventCallback const & onEventCallback);

protected:
  friend class ScMemory;

  using ScCreateEventSubscriptionCallback =
      std::function<ScEventSubscription *(ScMemoryContext *, ScAddr const &, ScEventCallback const &)>;

  struct ScEventTypeHashFunc
  {
    sc_event_type operator()(ScEvent::Type const & eventType) const;
  };

  template <sc_event_type eventType>
  static ScEventSubscription * CreateEventSubscription(
      ScMemoryContext * context,
      ScAddr const & subscriptionAddr,
      ScEventCallback const & onEventFunc);

  template <sc_event_type eventType>
  static ScCreateEventSubscriptionCallback CreateEventSubscriptionWrapper();

  static sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr);

  static sc_result Shutdown(ScMemoryContext * ctx);

private:
  static std::unordered_map<ScEvent::Type, ScCreateEventSubscriptionCallback, ScEventTypeHashFunc>
      m_eventTypesToCreateSubscriptionCallbacks;

  static std::unordered_map<ScAddr, ScEvent::Type, ScAddrHashFunc<sc_uint32>> m_namesToEventTypes;
};

#include "sc_event_subscription.tpp"
