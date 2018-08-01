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
ScAddr ASCPAgentDeactivator::msAgentKeynode;

SC_AGENT_IMPLEMENTATION(ASCPAgentActivator)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr agent = ms_context->GetArcEnd(edgeAddr);

    SCPAgentEvent::register_scp_agent((ScMemoryContext&)ms_context, agent);

    return SC_RESULT_OK;
}

SC_AGENT_IMPLEMENTATION(ASCPAgentDeactivator)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    //!TODO Fix crush error
    //ScAddr agent = ms_context->GetArcEnd(edgeAddr);
    //SCPAgentEvent::unregister_scp_agent((ScMemoryContext&)ms_context, agent);

    return SC_RESULT_OK;
}

}
