/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "scpAgentProcessor.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{
ScAddr ASCPAgentActivator::msAgentKeynode;
concurrent_queue<SCPAgentEvent*> ASCPAgentActivator::scp_agent_events;

void ASCPAgentActivator::unregister_all_scp_agents()
{
    while (!scp_agent_events.empty())
    {
        SCPAgentEvent *event = scp_agent_events.front();
        scp_agent_events.pop();
        delete event;
    }
}

ScEvent::Type ASCPAgentActivator::resolve_event_type(ScMemoryContext & ctx, ScAddr const & event_type_node)
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

void ASCPAgentActivator::register_all_scp_agents(ScMemoryContext & ctx)
{
    ScIterator3Ptr iter_agent = ctx.Iterator3(Keynodes::active_sc_agent, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    while (iter_agent->Next())
    {
        ScAddr agent = iter_agent->Get(2);
        register_scp_agent(ctx, agent);
    }
}

void ASCPAgentActivator::register_scp_agent(ScMemoryContext & ctx, ScAddr &agent_node)
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

    SCPAgentEvent* event = new SCPAgentEvent(ctx, event_node, resolve_event_type(ctx, event_type_node), agent_proc);
    scp_agent_events.push(event);
}

SC_AGENT_IMPLEMENTATION(ASCPAgentActivator)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr action = ms_context->GetArcEnd(edgeAddr);


    return SC_RESULT_OK;
}

}
