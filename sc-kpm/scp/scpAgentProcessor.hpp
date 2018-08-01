/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "concurrent_queue.hpp"
#include "scpAgentEvent.hpp"
#include "scpKeynodes.hpp"

#include "scpAgentProcessor.generated.hpp"

namespace scp
{

class ASCPAgentActivator : public ScAgent
{
    SC_CLASS(Agent, Event(Keynodes::active_sc_agent, ScEvent::Type::AddOutputEdge))
    SC_GENERATED_BODY()

private:
    static concurrent_queue<SCPAgentEvent*> scp_agent_events;

    ScEvent::Type static resolve_event_type(ScMemoryContext & ctx, ScAddr const & event_type_node);

public:
    SC_PROPERTY(Keynode("sc_agent_of_scp_agents_activation"), ForceCreate)
    static ScAddr msAgentKeynode;

    void static register_all_scp_agents(ScMemoryContext & ctx);
    void static register_scp_agent(ScMemoryContext & ctx, ScAddr &agent_node);
    void static unregister_all_scp_agents();

};

}

