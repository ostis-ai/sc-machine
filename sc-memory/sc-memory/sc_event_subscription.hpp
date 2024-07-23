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

  /*!
   * @brief Initializes an event subscription.
   *
   * @param ctx Pointer to an sc-memory context.
   * @param initMemoryGeneratedStructureAddr Address of the initial memory generated structure.
   * @return Result of the initialization.
   */
  void Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) final;

  /*!
   * @brief Shuts down an event subscription.
   *
   * @param ctx Pointer to an sc-memory context.
   * @return Result of the shutdown.
   */
  void Shutdown(ScMemoryContext * ctx) final;
};

template <class TScEvent>
class _SC_EXTERN ScElementaryEventSubscription : public ScEventSubscription
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

public:
  using DelegateFunc = std::function<void(TScEvent const & event)>;

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

class _SC_EXTERN ScEventSubscriptionAddOutputArc final : public ScElementaryEventSubscription<ScEventAddOutputArc>
{
public:
  _SC_EXTERN ScEventSubscriptionAddOutputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventAddOutputArc const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionAddInputArc final : public ScElementaryEventSubscription<ScEventAddInputArc>
{
public:
  _SC_EXTERN ScEventSubscriptionAddInputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventAddInputArc const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionAddEdge final : public ScElementaryEventSubscription<ScEventAddEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionAddEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventAddEdge const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionRemoveOutputArc final : public ScElementaryEventSubscription<ScEventRemoveOutputArc>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveOutputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventRemoveOutputArc const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionRemoveInputArc final : public ScElementaryEventSubscription<ScEventRemoveInputArc>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveInputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventRemoveInputArc const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionRemoveEdge final : public ScElementaryEventSubscription<ScEventRemoveEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventRemoveEdge const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionEraseElement final : public ScElementaryEventSubscription<ScEventEraseElement>
{
public:
  _SC_EXTERN ScEventSubscriptionEraseElement(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventEraseElement const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionChangeContent final : public ScElementaryEventSubscription<ScEventChangeContent>
{
public:
  _SC_EXTERN ScEventSubscriptionChangeContent(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventChangeContent const &)> const & func);
};

/*!
 * @brief Factory class for creating sc-event subscriptions.
 *
 * This class provides methods to create subscriptions for different types of sc-events.
 */
class _SC_EXTERN ScEventSubscriptionFactory
{
public:
  using ScEventCallback = std::function<void(ScElementaryEvent const &)>;

  /*!
   * @brief Creates an event subscription using the event type address.
   *
   * @param context Pointer to an sc-memory context.
   * @param evenTypeAddr Address of the event type.
   * @param subscriptionAddr Address of a subscription.
   * @param onEventCallback Callback function to be called on event emission.
   * @return Pointer to the created event subscription.
   */
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

  template <sc_event_type eventType>
  struct _SC_EXTERN ScEventTypeConverter;

  template <sc_event_type eventType>
  using _SC_EXTERN ScEventSubscriptionType = typename ScEventTypeConverter<eventType>::SubscriptionType;

  template <sc_event_type eventType>
  using _SC_EXTERN ScEventTypeClass = typename ScEventTypeConverter<eventType>::EventTypeClass;

  using ScCreateEventSubscriptionCallback =
      std::function<ScEventSubscription *(ScMemoryContext *, ScAddr const &, ScEventCallback const &)>;

  template <sc_event_type eventType>
  static ScEventSubscription * CreateEventSubscription(
      ScMemoryContext * context,
      ScAddr const & subscriptionAddr,
      ScEventCallback const & onEventFunc);

  template <sc_event_type eventType>
  static ScCreateEventSubscriptionCallback CreateEventSubscriptionWrapper();

  static void Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr);
  static void Shutdown(ScMemoryContext * ctx);

private:
  static std::
      unordered_map<ScAddr, ScEventSubscriptionFactory::ScCreateEventSubscriptionCallback, ScAddrHashFunc>
          m_eventTypesToCreateSubscriptionCallbacks;
};

#include "sc_event_subscription.tpp"
