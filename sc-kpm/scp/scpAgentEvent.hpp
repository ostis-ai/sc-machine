/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_event.hpp"
#include "concurrent_queue.hpp"

namespace scp
{

class SCPAgentEvent
{

private:
    sc_result static runSCPAgent(sc_event const * evt, sc_addr edge, sc_addr other_el);
    sc_event_type ConvertEventType(ScEvent::Type type);
    sc_addr static resolve_sc_addr_from_pointer(sc_pointer data);
    ScEvent::Type static resolve_event_type(ScAddr const & event_type_node);
    sc_event * m_event;

public:
    static concurrent_deque<SCPAgentEvent*> scp_agent_events;
    void static unregister_all_scp_agents();
    void static register_all_scp_agents(ScMemoryContext & ctx);
    void static register_scp_agent(ScMemoryContext & ctx, ScAddr &agent_node);
    void static unregister_scp_agent(ScMemoryContext & ctx, ScAddr &agent_node);
    SCPAgentEvent(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::Type eventType, const ScAddr & proc_addr);
    ScAddr GetProcAddr();
    ~SCPAgentEvent();
};

}

