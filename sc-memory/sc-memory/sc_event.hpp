/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <functional>

#include "sc_memory.hpp"
#include "sc_object.hpp"

#include "sc_type.hpp"
#include "sc_utils.hpp"

#include "utils/sc_lock.hpp"
#include "sc-core/sc-store/sc-event/sc_event_types.h"

template <class TScEvent>
concept ScEventClass = std::derived_from<TScEvent, class ScEvent>;

class _SC_EXTERN ScEvent : public ScObject
{
public:
  _SC_EXTERN virtual ~ScEvent() = default;

  _SC_EXTERN virtual ScAddr GetUser() const = 0;

  _SC_EXTERN virtual ScAddr GetSubscriptionElement() const = 0;

protected:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

public:
  class Type
  {
  public:
    sc_event_type m_realType;

    explicit constexpr Type(sc_event_type type)
      : m_realType(type)
    {
    }

    constexpr Type(Type const & otherType)
      : Type(otherType.m_realType)
    {
    }

    constexpr ~Type() = default;

    inline constexpr sc_event_type operator*() const
    {
      return m_realType;
    }

    Type & operator()(Type const & other)
    {
      m_realType |= other.m_realType;
      return *this;
    }

    inline sc_bool operator==(Type const & other) const
    {
      return (m_realType == other.m_realType);
    }

    inline sc_bool operator!=(Type const & other) const
    {
      return (m_realType != other.m_realType);
    }

    inline Type operator|(Type const & other) const
    {
      return Type(m_realType | other.m_realType);
    }

    inline Type operator&(Type const & other) const
    {
      return Type(m_realType & other.m_realType);
    }

    inline Type & operator|=(Type const & other)
    {
      m_realType |= other.m_realType;
      return *this;
    }

    inline Type & operator&=(Type const & other)
    {
      m_realType &= other.m_realType;
      return *this;
    }

    constexpr operator sc_event_type() const
    {
      return m_realType;
    }

    static Type const Unknown;
    static Type const AddInputEdge;
    static Type const AddOutputEdge;
    static Type const RemoveInputEdge;
    static Type const RemoveOutputEdge;
    static Type const RemoveElement;
    static Type const ChangeContent;
  };

protected:
  static inline Type const type = Type::Unknown;

  _SC_EXTERN sc_result Initialize(ScMemoryContext *, ScAddr const &) override
  {
    return SC_RESULT_OK;
  }

  _SC_EXTERN sc_result Shutdown(ScMemoryContext *) override
  {
    return SC_RESULT_OK;
  }
};

class _SC_EXTERN ScElementaryEvent : public ScEvent
{
public:
  _SC_EXTERN ScAddr GetUser() const override
  {
    return m_userAddr;
  }

  _SC_EXTERN ScAddr GetSubscriptionElement() const override
  {
    return m_subscriptionAddr;
  }

  _SC_EXTERN std::array<ScAddr, 3> GetTriple() const
  {
    return {m_subscriptionAddr, m_connectorAddr, m_otherAddr};
  }

protected:
  _SC_EXTERN ScElementaryEvent(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : m_userAddr(userAddr)
    , m_subscriptionAddr(subscriptionAddr)
    , m_connectorAddr(connectorAddr)
    , m_connectorType(connectorType)
    , m_otherAddr(otherAddr) {};

  ScAddr m_userAddr;
  ScAddr m_subscriptionAddr;
  ScAddr m_connectorAddr;
  ScType m_connectorType;
  ScAddr m_otherAddr;

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::Unknown;
};

class _SC_EXTERN ScEventAddEdge : public ScElementaryEvent
{
public:
  _SC_EXTERN virtual ScAddr GetAddedConnector() const
  {
    return m_connectorAddr;
  }

  _SC_EXTERN virtual ScType GetAddedConnectorType() const
  {
    return m_connectorType;
  }

  _SC_EXTERN virtual ScAddr GetOtherElement() const
  {
    return m_otherAddr;
  }

protected:
  _SC_EXTERN ScEventAddEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};
};

class _SC_EXTERN ScEventAddOutputEdge final : public ScEventAddEdge
{
protected:
  _SC_EXTERN ScEventAddOutputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScEventAddEdge(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::AddOutputEdge;
};

class _SC_EXTERN ScEventAddInputEdge final : public ScEventAddEdge
{
protected:
  _SC_EXTERN ScEventAddInputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScEventAddEdge(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::AddInputEdge;
};

class _SC_EXTERN ScEventRemoveEdge : public ScElementaryEvent
{
public:
  _SC_EXTERN virtual ScType GetRemovedConnectorType() const
  {
    return m_connectorType;
  }

  _SC_EXTERN virtual ScAddr GetOtherElement() const
  {
    return m_otherAddr;
  }

protected:
  _SC_EXTERN ScEventRemoveEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};
};

class _SC_EXTERN ScEventRemoveOutputEdge final : public ScEventRemoveEdge
{
protected:
  _SC_EXTERN ScEventRemoveOutputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScEventRemoveEdge(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::RemoveOutputEdge;
};

class _SC_EXTERN ScEventRemoveInputEdge final : public ScEventRemoveEdge
{
protected:
  _SC_EXTERN ScEventRemoveInputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScEventRemoveEdge(userAddr, otherAddr, connectorAddr, connectorType, subscriptionAddr) {};

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::RemoveInputEdge;
};

class _SC_EXTERN ScEventRemoveElement final : public ScElementaryEvent
{
protected:
  _SC_EXTERN ScEventRemoveElement(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

private:
  _SC_EXTERN ScAddr GetSubscriptionElement() const override
  {
    return m_subscriptionAddr;
  }

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::RemoveElement;
};

class _SC_EXTERN ScEventChangeContent final : public ScElementaryEvent
{
protected:
  _SC_EXTERN ScEventChangeContent(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr)
    : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::ChangeContent;
};

/* Base class for sc-events
 */
class ScEventSubscription
{
public:
  _SC_EXTERN virtual ~ScEventSubscription() = default;

  /* Set specified function as a delegate that will be calls on event emit */
  template <typename FuncT>
  _SC_EXTERN void SetDelegate(FuncT &&)
  {
  }

  _SC_EXTERN virtual void RemoveDelegate() = 0;

protected:
  _SC_EXTERN static sc_result Handler(sc_event const *, sc_addr, sc_addr, sc_type, sc_addr)
  {
    return SC_RESULT_OK;
  }

  _SC_EXTERN static sc_result HandlerDelete(sc_event const *)
  {
    return SC_RESULT_OK;
  }
};

template <ScEventClass TScEvent>
class ScElementaryEventSubscription : public ScEventSubscription
{
public:
  using DelegateFunc = std::function<sc_result(TScEvent const & event)>;

  explicit _SC_EXTERN ScElementaryEventSubscription(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func = DelegateFunc())
  {
    m_delegate = std::move(func);
    m_event = sc_event_with_user_new(
        *ctx,
        *addr,
        TScEvent::type,
        (sc_pointer)this,
        &ScElementaryEventSubscription::Handler,
        &ScElementaryEventSubscription::HandlerDelete);
  }

  virtual _SC_EXTERN ~ScElementaryEventSubscription()
  {
    if (m_event)
      sc_event_destroy(m_event);
  }

  // Don't allow copying of events
  _SC_EXTERN ScElementaryEventSubscription(ScElementaryEventSubscription const & other) = delete;

  /* Set specified function as a delegate that will be calls on event emit */
  template <typename FuncT>
  _SC_EXTERN void SetDelegate(FuncT && func)
  {
    m_delegate = func;
  }

  _SC_EXTERN void RemoveDelegate()
  {
    m_delegate = DelegateFunc();
  }

protected:
  friend class ScMemoryContext;

  _SC_EXTERN static sc_result Handler(
      sc_event const * event,
      sc_addr userAddr,
      sc_addr connectorAddr,
      sc_type connectorType,
      sc_addr otherAddr)
  {
    sc_result result = SC_RESULT_ERROR;

    auto * eventObj = (ScElementaryEventSubscription *)sc_event_get_data(event);

    if (eventObj == nullptr)
      return result;

    DelegateFunc delegateFunc = eventObj->m_delegate;
    if (delegateFunc == nullptr)
      goto result;

    try
    {
      result = delegateFunc(TScEvent(userAddr, sc_event_get_element(event), connectorAddr, connectorType, otherAddr))
                   ? SC_RESULT_OK
                   : SC_RESULT_ERROR;
    }
    catch (utils::ScException & e)
    {
      SC_LOG_ERROR("Uncaught exception: " << e.Message());
    }

  result:
    return result;
  }

  _SC_EXTERN static sc_result HandlerDelete(sc_event const * event)
  {
    auto * eventObj = (ScElementaryEventSubscription *)sc_event_get_data(event);

    utils::ScLockScope lock(eventObj->m_lock);
    if (eventObj->m_event)
    {
      eventObj->m_delegate = nullptr;
      eventObj->m_event = nullptr;
    }

    return SC_RESULT_OK;
  }

private:
  sc_event * m_event;
  DelegateFunc m_delegate;
  utils::ScLock m_lock;
};

class ScEventSubscriptionAddOutputEdge final : public ScElementaryEventSubscription<ScEventAddOutputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionAddOutputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func)
    : ScElementaryEventSubscription(ctx, addr, func)
  {
  }
};

class ScEventSubscriptionAddInputEdge final : public ScElementaryEventSubscription<ScEventAddInputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionAddInputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func)
    : ScElementaryEventSubscription(ctx, addr, func)
  {
  }
};

class ScEventSubscriptionRemoveOutputEdge final : public ScElementaryEventSubscription<ScEventRemoveOutputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveOutputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func)
    : ScElementaryEventSubscription(ctx, addr, func)
  {
  }
};

class ScEventSubscriptionRemoveInputEdge final : public ScElementaryEventSubscription<ScEventRemoveInputEdge>
{
public:
  _SC_EXTERN ScEventSubscriptionRemoveInputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func)
    : ScElementaryEventSubscription(ctx, addr, func)
  {
  }
};

class ScEventSubscriptionEraseElement final : public ScElementaryEventSubscription<ScEventRemoveElement>
{
public:
  _SC_EXTERN ScEventSubscriptionEraseElement(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func)
    : ScElementaryEventSubscription(ctx, addr, func)
  {
  }
};

class ScEventSubscriptionContentChanged final : public ScElementaryEventSubscription<ScEventChangeContent>
{
public:
  _SC_EXTERN ScEventSubscriptionContentChanged(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      DelegateFunc const & func)
    : ScElementaryEventSubscription(ctx, addr, func)
  {
  }
};

template <sc_event_type eventType>
struct ScEventTypeConverter;

template <sc_event_type eventType>
using ScEventSubscriptionType = typename ScEventTypeConverter<eventType>::SubscriptionType;

template <sc_event_type eventType>
using ScEventTypeClass = typename ScEventTypeConverter<eventType>::EventTypeClass;

template <>
struct ScEventTypeConverter<sc_event_add_input_arc>
{
  using SubscriptionType = ScEventSubscriptionAddInputEdge;
  using EventTypeClass = ScEventAddInputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_add_output_arc>
{
  using SubscriptionType = ScEventSubscriptionAddOutputEdge;
  using EventTypeClass = ScEventAddOutputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_remove_input_arc>
{
  using SubscriptionType = ScEventSubscriptionRemoveInputEdge;
  using EventTypeClass = ScEventRemoveInputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_remove_output_arc>
{
  using SubscriptionType = ScEventSubscriptionRemoveOutputEdge;
  using EventTypeClass = ScEventRemoveOutputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_remove_element>
{
  using SubscriptionType = ScEventSubscriptionEraseElement;
  using EventTypeClass = ScEventRemoveElement;
};

template <>
struct ScEventTypeConverter<sc_event_change_content>
{
  using SubscriptionType = ScEventSubscriptionContentChanged;
  using EventTypeClass = ScEventChangeContent;
};
