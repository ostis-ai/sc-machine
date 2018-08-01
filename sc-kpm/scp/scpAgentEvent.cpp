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

concurrent_deque<SCPAgentEvent*> SCPAgentEvent::scp_agent_events;

void SCPAgentEvent::unregister_all_scp_agents()
{
    while (!scp_agent_events.empty())
    {
        SCPAgentEvent *event = scp_agent_events.front();
        scp_agent_events.pop();
        delete event;
    }
}

sc_event_type SCPAgentEvent::ConvertEventType(ScEvent::Type type)
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

ScEvent::Type SCPAgentEvent::resolve_event_type(ScAddr const & event_type_node)
{
    if (event_type_node == Keynodes::sc_event_add_output_arc)
        return ScEvent::Type::AddOutputEdge;
    if (event_type_node == Keynodes::sc_event_add_input_arc)
        return ScEvent::Type::AddInputEdge;

    if (event_type_node == Keynodes::sc_event_remove_output_arc)
        return ScEvent::Type::RemoveOutputEdge;
    if (event_type_node == Keynodes::sc_event_remove_input_arc)
        return ScEvent::Type::RemoveInputEdge;

    if (event_type_node == Keynodes::sc_event_content_changed)
        return ScEvent::Type::ContentChanged;

    if (event_type_node == Keynodes::sc_event_remove_element)
        return ScEvent::Type::EraseElement;

    return ScEvent::Type::AddOutputEdge;
}

sc_addr SCPAgentEvent::resolve_sc_addr_from_pointer(sc_pointer data)
{
    sc_addr elem;
    elem.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT((int)(long)(data));
    elem.seg = SC_ADDR_LOCAL_SEG_FROM_INT((int)(long)(data));
    return elem;
}

void SCPAgentEvent::register_all_scp_agents(ScMemoryContext & ctx)
{
    ScIterator3Ptr iter_agent = ctx.Iterator3(Keynodes::active_sc_agent, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    while (iter_agent->Next())
    {
        ScAddr agent = iter_agent->Get(2);
        register_scp_agent(ctx, agent);
    }
}

void SCPAgentEvent::register_scp_agent(ScMemoryContext & ctx, ScAddr &agent_node)
{
    ScAddr abstract_agent, scp_agent;
    ScIterator3Ptr iter_impl = ctx.Iterator3(ScType::NodeConst, ScType::EdgeAccessConstPosPerm, agent_node);
    while (iter_impl->Next())
    {
        ScIterator5Ptr iter_agent = ctx.Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, iter_impl->Get(0), ScType::EdgeAccessConstPosPerm, Keynodes::nrel_inclusion);
        while (iter_agent->Next())
        {
            if (ctx.HelperCheckArc(Keynodes::abstract_sc_agent, iter_agent->Get(0), ScType::EdgeAccessConstPosPerm))
            {
                abstract_agent = iter_agent->Get(0);
                scp_agent = iter_impl->Get(0);
                break;
            }
        }
    }
    if (!scp_agent.IsValid())
        return;
    if (!abstract_agent.IsValid())
        return;

    //Find program
    ScAddr agent_proc;
    ScIterator5Ptr iter_proc = ctx.Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, scp_agent, ScType::EdgeAccessConstPosPerm, Keynodes::nrel_sc_agent_program);
    while (iter_proc->Next())
    {
        ScIterator3Ptr iter_proc2 = ctx.Iterator3(iter_proc->Get(0), ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
        while (iter_proc2->Next())
        {
            if (ctx.HelperCheckArc(Keynodes::agent_scp_program, iter_proc2->Get(2), ScType::EdgeAccessConstPosPerm))
            {
                agent_proc = iter_proc2->Get(2);
                break;
            }
        }
    }
    if (!agent_proc.IsValid())
        return;

    //Old SCP program check
    iter_proc = ctx.Iterator5(agent_proc, ScType::EdgeAccessConstPosPerm, ScType::NodeVar, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_key_sc_element);
    if (!iter_proc->Next())
        return;

    //Find event
    ScAddr event_type_node, event_node;
    ScIterator5Ptr iter_event = ctx.Iterator5(abstract_agent, ScType::EdgeDCommonConst, ScType::Const, ScType::EdgeAccessConstPosPerm, Keynodes::nrel_primary_initiation_condition);
    if (iter_event->Next())
    {
        event_type_node = ctx.GetArcBegin(iter_event->Get(2));
        event_node = ctx.GetArcEnd(iter_event->Get(2));
    }
    else
        return;

    SCPAgentEvent* event = new SCPAgentEvent(ctx, event_node, resolve_event_type(event_type_node), agent_proc);
    scp_agent_events.push(event);

    std::cout << "REGISTERED SCP AGENT: " << ctx.HelperGetSystemIdtf(abstract_agent) << std::endl;
}

void SCPAgentEvent::unregister_scp_agent(ScMemoryContext & ctx, ScAddr &agent_node)
{
    ScAddr abstract_agent, scp_agent;
    ScIterator3Ptr iter_impl = ctx.Iterator3(ScType::NodeConst, ScType::EdgeAccessConstPosPerm, agent_node);
    while (iter_impl->Next())
    {
        ScIterator5Ptr iter_agent = ctx.Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, iter_impl->Get(0), ScType::EdgeAccessConstPosPerm, Keynodes::nrel_inclusion);
        while (iter_agent->Next())
        {
            if (ctx.HelperCheckArc(Keynodes::abstract_sc_agent, iter_agent->Get(0), ScType::EdgeAccessConstPosPerm))
            {
                abstract_agent = iter_agent->Get(0);
                scp_agent = iter_impl->Get(0);
                break;
            }
        }
    }
    if (!scp_agent.IsValid())
        return;
    if (!abstract_agent.IsValid())
        return;

    //Find program
    ScAddr agent_proc;
    ScIterator5Ptr iter_proc = ctx.Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, scp_agent, ScType::EdgeAccessConstPosPerm, Keynodes::nrel_sc_agent_program);
    while (iter_proc->Next())
    {
        ScIterator3Ptr iter_proc2 = ctx.Iterator3(iter_proc->Get(0), ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
        while (iter_proc2->Next())
        {
            if (ctx.HelperCheckArc(Keynodes::agent_scp_program, iter_proc2->Get(2), ScType::EdgeAccessConstPosPerm))
            {
                agent_proc = iter_proc2->Get(2);
                break;
            }
        }
    }
    if (!agent_proc.IsValid())
        return;

    auto checker = [&agent_proc](SCPAgentEvent * event)
    {
        return event->GetProcAddr() == agent_proc;
    };

    SCPAgentEvent *event;
    if (scp_agent_events.extract(checker, event))
    {
        delete event;
        std::cout << "UNREGISTERED SCP AGENT: " << ctx.HelperGetSystemIdtf(abstract_agent) << std::endl;
    }
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

ScAddr SCPAgentEvent::GetProcAddr()
{
    return ScAddr(resolve_sc_addr_from_pointer(sc_event_get_data(m_event)));
}

SCPAgentEvent::~SCPAgentEvent()
{
    if (m_event)
        sc_event_destroy(m_event);
}

}

