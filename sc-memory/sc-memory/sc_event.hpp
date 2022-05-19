/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_utils.hpp"

#include "utils/sc_lock.hpp"

#include <functional>

/* Base class for sc-events
 */
class ScEvent
{
public:
  using DelegateFunc = std::function<bool(ScAddr const &, ScAddr const &, ScAddr const &)>;

  enum class Type : uint8_t
  {
    AddOutputEdge = 0,
    AddInputEdge,
    RemoveOutputEdge,
    RemoveInputEdge,
    EraseElement,
    ContentChanged
  };

  explicit _SC_EXTERN ScEvent(
      class ScMemoryContext const & ctx,
      const ScAddr & addr,
      Type eventType,
      DelegateFunc func = DelegateFunc());
  virtual _SC_EXTERN ~ScEvent();

  // Don't allow copying of events
  ScEvent(const ScEvent & other) = delete;

  /* Set specified function as a delegate that will be calls on event emit */
  template <typename FuncT>
  void SetDelegate(FuncT && func)
  {
    m_delegate = func;
  }

  void RemoveDelegate();

protected:
  static sc_result Handler(sc_event const * evt, sc_addr edge, sc_addr other_el);
  static sc_result HandlerDelete(sc_event const * evt);

private:
  sc_event * m_event;
  DelegateFunc m_delegate;
  utils::ScLock m_lock;
};

SHARED_PTR_TYPE(ScEvent);

class ScEventAddOutputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventAddOutputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::DelegateFunc func)
    : ScEvent(ctx, addr, ScEvent::Type::AddOutputEdge, func)
  {
  }
};

class ScEventAddInputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventAddInputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::DelegateFunc func)
    : ScEvent(ctx, addr, ScEvent::Type::AddInputEdge, func)
  {
  }
};

class ScEventRemoveOutputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventRemoveOutputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::DelegateFunc func)
    : ScEvent(ctx, addr, ScEvent::Type::RemoveOutputEdge, func)
  {
  }
};

class ScEventRemoveInputEdge final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventRemoveInputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::DelegateFunc func)
    : ScEvent(ctx, addr, ScEvent::Type::RemoveInputEdge, func)
  {
  }
};

class ScEventEraseElement final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventEraseElement(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::DelegateFunc func)
    : ScEvent(ctx, addr, ScEvent::Type::EraseElement, func)
  {
  }
};

class ScEventContentChanged final : public ScEvent
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN ScEventContentChanged(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::DelegateFunc func)
    : ScEvent(ctx, addr, ScEvent::Type::ContentChanged, func)
  {
  }
};
