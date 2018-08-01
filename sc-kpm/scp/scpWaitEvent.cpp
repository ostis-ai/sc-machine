/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scp.hpp"
#include "scpWaitEvent.hpp"
#include "scpKeynodes.hpp"
#include "scpUtils.hpp"

#include <iostream>

namespace scp
{

concurrent_deque<SCPWaitEvent*> SCPWaitEvent::sys_wait_events;

void SCPWaitEvent::unregister_all_sys_wait()
{
    while (!sys_wait_events.empty())
    {
        SCPWaitEvent *event = sys_wait_events.front();
        sys_wait_events.pop();
        delete event;
    }
}

sc_event_type SCPWaitEvent::ConvertEventType(ScEvent::Type type)
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

    SC_THROW_EXCEPTION(utils::ExceptionNotImplemented,
                       "Unsupported event type " + std::to_string(int(type)));
    return SC_EVENT_UNKNOWN;
}

sc_addr SCPWaitEvent::resolve_sc_addr_from_pointer(sc_pointer data)
{
    sc_addr elem;
    elem.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT((int)(long)(data));
    elem.seg = SC_ADDR_LOCAL_SEG_FROM_INT((int)(long)(data));
    return elem;
}

sc_result SCPWaitEvent::Run(sc_event const * evt, sc_addr edge, sc_addr other_el)
{
    ScMemoryContext & ctx = (ScMemoryContext &)scpModule::s_default_ctx;
    ScAddr oper_node = ScAddr(resolve_sc_addr_from_pointer(sc_event_get_data(evt)));

    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_finished_successfully, oper_node);
    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_finished, oper_node);

    auto checker = [&oper_node](SCPWaitEvent * event)
    {
        return event->GetParamAddr() == oper_node;
    };

    SCPWaitEvent *event;
    if (sys_wait_events.extract(checker, event))
    {
        delete event;
    }

    return SC_RESULT_OK;
}

SCPWaitEvent::SCPWaitEvent(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::Type eventType, const ScAddr & param_addr)
{
    m_event = sc_event_new_ex(*ctx, *addr, ConvertEventType(eventType), (sc_pointer)SC_ADDR_LOCAL_TO_INT(*param_addr), Run, NULL);
}

ScAddr SCPWaitEvent::GetParamAddr()
{
    return ScAddr(resolve_sc_addr_from_pointer(sc_event_get_data(m_event)));
}

SCPWaitEvent::~SCPWaitEvent()
{
    if (m_event)
        sc_event_destroy(m_event);
}

}

