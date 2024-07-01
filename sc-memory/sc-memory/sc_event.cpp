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

ScEvent::Type const ScEvent::Type::Unknown(sc_event_unknown);
ScEvent::Type const ScEvent::Type::AddInputEdge(sc_event_add_input_arc);
ScEvent::Type const ScEvent::Type::AddOutputEdge(sc_event_add_output_arc);
ScEvent::Type const ScEvent::Type::RemoveInputEdge(sc_event_remove_input_arc);
ScEvent::Type const ScEvent::Type::RemoveOutputEdge(sc_event_remove_output_arc);
ScEvent::Type const ScEvent::Type::RemoveElement(sc_event_remove_element);
ScEvent::Type const ScEvent::Type::ChangeContent(sc_event_change_content);

ScEvent::ScEvent(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    Type const & eventType,
    ScEvent::DelegateFunc const & func /*= DelegateFunc()*/)
{
  m_delegate = std::move(func);
  m_event = sc_event_new_ex(*ctx, *addr, eventType, (sc_pointer)this, &ScEvent::Handler, &ScEvent::HandlerDelete);
}

ScEvent::ScEvent(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    Type const & eventType,
    DelegateFuncWithUserAddr const & func /*= DelegateFuncWithUserAddr()*/)
{
  m_delegateExt = std::move(func);
  m_event =
      sc_event_with_user_new(*ctx, *addr, eventType, (sc_pointer)this, &ScEvent::Handler, &ScEvent::HandlerDelete);
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
