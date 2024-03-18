/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

#include <utility>
#include "sc_utils.hpp"
#include "sc_addr.hpp"
#include "sc_memory.hpp"

#include "utils/sc_log.hpp"

extern "C"
{
#include "sc-core/sc-store/sc-base/sc_monitor.h"
}

// Should be equal to C values

namespace
{
sc_event_type ConvertEventType(ScEvent::Type type)
{
  switch (type)
  {
  case ScEvent::Type::AddOutputEdge:
    return SC_EVENT_ADD_OUTPUT_ARC;

  case ScEvent::Type::AddInputEdge:
    return SC_EVENT_ADD_INPUT_ARC;

  case ScEvent::Type::RemoveOutputEdge:
    return SC_EVENT_REMOVE_OUTPUT_ARC;

  case ScEvent::Type::RemoveInputEdge:
    return SC_EVENT_REMOVE_INPUT_ARC;

  case ScEvent::Type::EraseElement:
    return SC_EVENT_REMOVE_ELEMENT;

  case ScEvent::Type::ContentChanged:
    return SC_EVENT_CONTENT_CHANGED;
  }

  SC_THROW_EXCEPTION(utils::ExceptionNotImplemented, "Unsupported event type " + std::to_string(int(type)));
}

}  // namespace

ScEvent::ScEvent(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    Type eventType,
    ScEvent::DelegateFunc func /*= DelegateFunc()*/)
{
  m_delegate = std::move(func);
  m_event = sc_event_new_ex(
      *ctx, *addr, ConvertEventType(eventType), (sc_pointer)this, &ScEvent::Handler, &ScEvent::HandlerDelete);
}

ScEvent::ScEvent(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    Type eventType,
    DelegateFuncWithUserAddr func /*= DelegateFuncWithUserAddr()*/)
{
  m_delegateExt = std::move(func);
  m_event = sc_event_with_user_new(
      *ctx, *addr, ConvertEventType(eventType), (sc_pointer)this, &ScEvent::Handler, &ScEvent::HandlerDelete);
}

ScEvent::~ScEvent()
{
  if (m_event)
    sc_event_destroy(m_event);
}

void ScEvent::RemoveDelegate()
{
  m_delegate = DelegateFunc();
}

sc_result ScEvent::Handler(sc_event const * event, sc_addr connector_addr, sc_addr other_addr)
{
  sc_result result = SC_RESULT_ERROR;

  auto * eventObj = (ScEvent *)sc_event_get_data(event);

  if (eventObj == nullptr)
    return result;

  DelegateFunc delegateFunc = eventObj->m_delegate;
  if (delegateFunc == nullptr)
    goto result;

  try
  {
    result = delegateFunc(ScAddr(sc_event_get_element(event)), ScAddr(connector_addr), ScAddr(other_addr))
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

sc_result ScEvent::Handler(
    sc_event const * event,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr)
{
  sc_result result = SC_RESULT_ERROR;

  auto * eventObj = (ScEvent *)sc_event_get_data(event);

  if (eventObj == nullptr)
    return result;

  DelegateFuncWithUserAddr delegateFunc = eventObj->m_delegateExt;
  if (delegateFunc == nullptr)
    goto result;

  try
  {
    result = delegateFunc(
                 ScAddr(user_addr),
                 ScAddr(sc_event_get_element(event)),
                 ScAddr(connector_addr),
                 ScType(connector_type),
                 ScAddr(other_addr))
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

sc_result ScEvent::HandlerDelete(sc_event const * evt)
{
  auto * eventObj = (ScEvent *)sc_event_get_data(evt);

  utils::ScLockScope lock(eventObj->m_lock);
  if (eventObj->m_event)
  {
    eventObj->m_delegate = nullptr;
    eventObj->m_event = nullptr;
  }

  return SC_RESULT_OK;
}
