/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <functional>

#include "sc_addr.hpp"
#include "sc_utils.hpp"

#include "utils/sc_lock.hpp"
#include "sc-core/sc-store/sc-event/sc_event_types.h"

/* Base class for sc-events
 */
class ScEvent
{
public:
  using DelegateFunc = std::function<sc_result(ScAddr const &, ScAddr const &, ScAddr const &)>;
  using DelegateFuncWithUserAddr =
      std::function<sc_result(ScAddr const &, ScAddr const &, ScAddr const &, ScType const &, ScAddr const &)>;

  class Type
  {
  public:
    sc_event_type m_realType;

    explicit Type(sc_event_type type)
      : m_realType(type)
    {
    }

    Type(Type const & otherType)
      : Type(otherType.m_realType)
    {
    }

    inline sc_event_type operator*() const
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

    operator sc_event_type() const
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

  explicit _SC_EXTERN ScEvent(
      class ScMemoryContext const & ctx,
      ScAddr const & addr,
      Type const & eventType,
      DelegateFunc const & func = DelegateFunc());
  explicit _SC_EXTERN ScEvent(
      class ScMemoryContext const & ctx,
      ScAddr const & addr,
      Type const &  eventType,
      DelegateFuncWithUserAddr const & func = DelegateFuncWithUserAddr());
  virtual _SC_EXTERN ~ScEvent();

  // Don't allow copying of events
  ScEvent(ScEvent const & other) = delete;

  /* Set specified function as a delegate that will be calls on event emit */
  template <typename FuncT>
  void SetDelegate(FuncT && func)
  {
    m_delegate = func;
  }

  void RemoveDelegate();

protected:
  static sc_result Handler(sc_event const * event, sc_addr connector_addr, sc_addr other_addr);
  static sc_result Handler(
      sc_event const * event,
      sc_addr user_addr,
      sc_addr connector_addr,
      sc_type connector_type,
      sc_addr other_addr);
  static sc_result HandlerDelete(sc_event const * event);

private:
  sc_event * m_event;
  DelegateFunc m_delegate;
  DelegateFuncWithUserAddr m_delegateExt;
  utils::ScLock m_lock;
};

SHARED_PTR_TYPE(ScEvent);

class ScEventAddOutputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventAddOutputEdge(ScMemoryContext const & ctx, ScAddr const & addr, ScEvent::DelegateFunc const & func)
    : ScEvent(ctx, addr, ScEvent::Type::AddOutputEdge, func)
  {
  }
};

class ScEventAddInputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventAddInputEdge(ScMemoryContext const & ctx, ScAddr const & addr, ScEvent::DelegateFunc const & func)
    : ScEvent(ctx, addr, ScEvent::Type::AddInputEdge, func)
  {
  }
};

class ScEventRemoveOutputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventRemoveOutputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      ScEvent::DelegateFunc const & func)
    : ScEvent(ctx, addr, ScEvent::Type::RemoveOutputEdge, func)
  {
  }
};

class ScEventRemoveInputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventRemoveInputEdge(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      ScEvent::DelegateFunc const & func)
    : ScEvent(ctx, addr, ScEvent::Type::RemoveInputEdge, func)
  {
  }
};

class ScEventEraseElement final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventEraseElement(ScMemoryContext const & ctx, ScAddr const & addr, ScEvent::DelegateFunc const & func)
    : ScEvent(ctx, addr, ScEvent::Type::RemoveElement, func)
  {
  }
};

class ScEventContentChanged final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventContentChanged(ScMemoryContext const & ctx, ScAddr const & addr, ScEvent::DelegateFunc const & func)
    : ScEvent(ctx, addr, ScEvent::Type::ChangeContent, func)
  {
  }
};
