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

public:
    SC_PROPERTY(Keynode("sc_agent_of_scp_agents_activation"), ForceCreate)
    static ScAddr msAgentKeynode;

};

class ASCPAgentDeactivator : public ScAgent
{
    SC_CLASS(Agent, Event(Keynodes::active_sc_agent, ScEvent::Type::RemoveOutputEdge))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("sc_agent_of_scp_agents_deactivation"), ForceCreate)
    static ScAddr msAgentKeynode;

};

}

