/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"
#include "sc_utils.hpp"
#include "sc_addr.hpp"
#include "sc_memory.hpp"

#include "utils/sc_log.hpp"

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
  return SC_EVENT_UNKNOWN;
}

}  // namespace

ScEvent::ScEvent(
    const ScMemoryContext & ctx,
    const ScAddr & addr,
    Type eventType,
    ScEvent::DelegateFunc func /*= DelegateFunc()*/)
{
  m_delegate = func;
  m_event = sc_event_new_ex(
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

sc_result ScEvent::Handler(sc_event const * evt, sc_addr edge, sc_addr other_el)
{
  ScEvent * eventObj = (ScEvent *)sc_event_get_data(evt);
  SC_ASSERT(eventObj != nullptr, ());

  if (eventObj->m_delegate)
  {
    return eventObj->m_delegate(ScAddr(sc_event_get_element(evt)), ScAddr(edge), ScAddr(other_el)) ? SC_RESULT_OK
                                                                                                   : SC_RESULT_ERROR;
  }

  return SC_RESULT_ERROR;
}

sc_result ScEvent::HandlerDelete(sc_event const * evt)
{
  ScEvent * eventObj = (ScEvent *)sc_event_get_data(evt);
  SC_ASSERT(eventObj != nullptr, ());

  utils::ScLockScope(eventObj->m_lock);
  if (eventObj->m_event)
    eventObj->m_event = nullptr;

  return SC_RESULT_OK;
}
