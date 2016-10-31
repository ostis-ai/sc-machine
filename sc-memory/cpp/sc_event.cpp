/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"
#include "sc_utils.hpp"
#include "sc_addr.hpp"
#include "sc_memory.hpp"

ScEvent::Type ScEvent::AddOutputEdge = SC_EVENT_ADD_OUTPUT_ARC;
ScEvent::Type ScEvent::AddInputEdge = SC_EVENT_ADD_INPUT_ARC;
ScEvent::Type ScEvent::RemoveOutputEdge = SC_EVENT_REMOVE_OUTPUT_ARC;
ScEvent::Type ScEvent::RemoveInputEdge = SC_EVENT_ADD_INPUT_ARC;
ScEvent::Type ScEvent::EraseElement = SC_EVENT_REMOVE_ELEMENT;
ScEvent::Type ScEvent::ContentChanged = SC_EVENT_CONTENT_CHANGED;

ScEvent::ScEvent(const ScMemoryContext & ctx, const ScAddr & addr, Type eventType)
{
	mEvent = sc_event_new(*ctx, *addr, (sc_event_type)eventType, (sc_pointer)this, &ScEvent::_handler, nullptr);
}

ScEvent::ScEvent(const ScMemoryContext & ctx, const ScAddr & addr, Type eventType, ScEvent::tDelegateFunc func)
{
	mDelegate = func;
	mEvent = sc_event_new(*ctx, *addr, (sc_event_type)eventType, (sc_pointer)this, &ScEvent::_handler, &ScEvent::_handlerDelete);
}

ScEvent::~ScEvent()
{
	if (mEvent)
		sc_event_destroy(mEvent);
}

void ScEvent::removeDelegate()
{
	mDelegate = tDelegateFunc();
}

sc_result ScEvent::_handler(sc_event const * evt, sc_addr arg)
{
	ScEvent * eventObj = (ScEvent*)sc_event_get_data(evt);
	check_expr(eventObj != nullptr);

	if (eventObj->mDelegate)
	{
		return eventObj->mDelegate(ScAddr(sc_event_get_element(evt)), ScAddr(arg)) ? SC_RESULT_OK : SC_RESULT_ERROR;
	}

	return SC_RESULT_ERROR;
}

sc_result ScEvent::_handlerDelete(sc_event const * evt)
{
	ScEvent * eventObj = (ScEvent*)sc_event_get_data(evt);
	check_expr(eventObj != nullptr);

	if (eventObj->mEvent)
	{
		sc_event_destroy(eventObj->mEvent);
		eventObj->mEvent = nullptr;
	}

	return SC_RESULT_OK;
}