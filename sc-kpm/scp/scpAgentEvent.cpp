/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scp.hpp"
#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "scpAgentEvent.hpp"

#include <iostream>

namespace scp
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

    SC_THROW_EXCEPTION(utils::ExceptionNotImplemented,
                       "Unsupported event type " + std::to_string(int(type)));
    return SC_EVENT_UNKNOWN;
}

sc_addr SCPAgentEvent::resolve_sc_addr_from_pointer(sc_pointer data)
{
    sc_addr elem;
    elem.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT((int)(long)(data));
    elem.seg = SC_ADDR_LOCAL_SEG_FROM_INT((int)(long)(data));
    return elem;
}

sc_result SCPAgentEvent::runSCPAgent(sc_event const * evt, sc_addr edge, sc_addr other_el)
{
    ScMemoryContext & ctx = (ScMemoryContext &)scpModule::s_default_ctx;
    ScAddr quest(other_el);

    ScIterator5Ptr iter_quest = ctx.Iterator5(quest, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, Keynodes::nrel_authors);
    if (iter_quest->Next())
    {
        if (iter_quest->Get(2) == Keynodes::abstract_scp_machine)
            return SC_RESULT_OK;
    }

    ScAddr proc_addr = ScAddr(resolve_sc_addr_from_pointer(sc_event_get_data(evt)));
    ScAddr inp_arc(edge);

    ScAddr scp_quest = ctx.CreateNode(ScType::NodeConst);
    ScAddr arc1 = ctx.CreateArc(ScType::EdgeAccessConstPosPerm, scp_quest, proc_addr);
    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::rrel_1, arc1);

    ScAddr scp_params = ctx.CreateNode(ScType::NodeConst);
    arc1 = ctx.CreateArc(ScType::EdgeAccessConstPosPerm, scp_params, proc_addr);
    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::rrel_1, arc1);

    arc1 = ctx.CreateArc(ScType::EdgeAccessConstPosPerm, scp_params, inp_arc);
    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::rrel_2, arc1);

    arc1 = ctx.CreateArc(ScType::EdgeAccessConstPosPerm, scp_quest, scp_params);
    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::rrel_2, arc1);

    arc1 = ctx.CreateArc(ScType::EdgeDCommonConst, scp_quest, Keynodes::abstract_scp_machine);
    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_authors, arc1);

    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_scp_interpretation_request, scp_quest);
    ctx.CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_initiated, scp_quest);

    return SC_RESULT_OK;
}

SCPAgentEvent::SCPAgentEvent(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::Type eventType, const ScAddr & proc_addr)
{
    m_event = sc_event_new_ex(*ctx, *addr, ConvertEventType(eventType), (sc_pointer)SC_ADDR_LOCAL_TO_INT(*proc_addr), runSCPAgent, NULL);
}

SCPAgentEvent::~SCPAgentEvent()
{
    if (m_event)
        sc_event_destroy(m_event);
}

}

