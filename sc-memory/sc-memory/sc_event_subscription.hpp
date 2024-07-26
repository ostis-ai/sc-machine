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
  _SC_EXTERN ~ScEventSubscription() override;

  /* Set specified function as a delegate that will be calls on event emit */
  template <typename FuncT>
  _SC_EXTERN void SetDelegate(FuncT &&);

  _SC_EXTERN virtual void RemoveDelegate() = 0;

protected:
  static sc_result Handler(sc_event const *, sc_addr, sc_addr, sc_type, sc_addr);

  /*!
   * @brief Handles the deletion of an event.
   *
   * @param event Pointer to an sc-event.
   * @return Result of the event deletion handling.
   */
  static sc_result HandlerDelete(sc_event const * event);
};

class _SC_EXTERN ScElementaryEventSubscription : public ScEventSubscription
{
public:
  using DelegateFunc = std::function<void(ScElementaryEvent const & event)>;

  explicit _SC_EXTERN ScElementaryEventSubscription(
      ScMemoryContext const & ctx,
      ScAddr const & eventTypeAddr,
      ScType const & elementType,
      ScAddr const & subscriptionAddr,
      DelegateFunc const & func = DelegateFunc());

  _SC_EXTERN ~ScElementaryEventSubscription() override;

  // Don't allow copying of events
  _SC_EXTERN ScElementaryEventSubscription(ScElementaryEventSubscription const & other) = delete;

  /* Set specified function as a delegate that will be calls on event emit */
  _SC_EXTERN void SetDelegate(DelegateFunc && func);

  _SC_EXTERN void RemoveDelegate() override;

protected:
  friend class ScMemoryContext;

  explicit _SC_EXTERN ScElementaryEventSubscription();

  _SC_EXTERN static sc_result Handler(
      sc_event const * event,
      sc_addr userAddr,
      sc_addr connectorAddr,
      sc_type connectorType,
      sc_addr otherAddr);

  _SC_EXTERN static sc_result HandlerDelete(sc_event const * event);

protected:
  sc_event * m_event;

private:
  DelegateFunc m_delegate;
  utils::ScLock m_lock;
};

template <class TScEvent>
class _SC_EXTERN TScElementaryEventSubscription : public ScElementaryEventSubscription
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

public:
  using DelegateFunc = std::function<void(TScEvent const & event)>;

  explicit _SC_EXTERN TScElementaryEventSubscription(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func = DelegateFunc());

  _SC_EXTERN ~TScElementaryEventSubscription() override;

  // Don't allow copying of events
  _SC_EXTERN TScElementaryEventSubscription(TScElementaryEventSubscription const & other) = delete;

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
  DelegateFunc m_delegate;
  utils::ScLock m_lock;
};

// Specific event subscription classes follow the same pattern as ScElementaryEventSubscription.
// They are specialized for different types of sc-events.

template <ScType const & arcType>
class _SC_EXTERN ScEventSubscriptionAddOutputArc final
  : public TScElementaryEventSubscription<ScEventAddOutputArc<arcType>>
{
public:
  _SC_EXTERN ScEventSubscriptionAddOutputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventAddOutputArc<arcType> const &)> const & func);
};

template <ScType const & arcType>
class _SC_EXTERN ScEventSubscriptionAddInputArc final
  : public TScElementaryEventSubscription<ScEventAddInputArc<arcType>>
{
public:
  _SC_EXTERN ScEventSubscriptionAddInputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventAddInputArc<arcType> const &)> const & func);
};

template <ScType const & edgeType>
class _SC_EXTERN ScEventSubscriptionAddEdge final : public TScElementaryEventSubscription<ScEventAddEdge<edgeType>>
{
public:
  _SC_EXTERN ScEventSubscriptionAddEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventAddEdge<edgeType> const &)> const & func);
};

template <ScType const & arcType>
class _SC_EXTERN ScEventSubscriptionRemoveOutputArc final
  : public TScElementaryEventSubscription<ScEventRemoveOutputArc<arcType>>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveOutputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventRemoveOutputArc<arcType> const &)> const & func);
};

template <ScType const & arcType>
class _SC_EXTERN ScEventSubscriptionRemoveInputArc final
  : public TScElementaryEventSubscription<ScEventRemoveInputArc<arcType>>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveInputArc(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventRemoveInputArc<arcType> const &)> const & func);
};

template <ScType const & edgeType>
class _SC_EXTERN ScEventSubscriptionRemoveEdge final
  : public TScElementaryEventSubscription<ScEventRemoveEdge<edgeType>>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventRemoveEdge<edgeType> const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionRemoveElement final : public TScElementaryEventSubscription<ScEventRemoveElement>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveElement(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventRemoveElement const &)> const & func);
};

class _SC_EXTERN ScEventSubscriptionChangeLinkContent final
  : public TScElementaryEventSubscription<ScEventChangeLinkContent>
{
public:
  _SC_EXTERN ScEventSubscriptionChangeLinkContent(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<void(ScEventChangeLinkContent const &)> const & func);
};

#include "sc_event_subscription.tpp"
