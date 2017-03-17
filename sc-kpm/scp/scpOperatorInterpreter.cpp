/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "scpOperator.hpp"
#include "scpOperatorInterpreter.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{
ScAddr ASCPOperatorInterpreter::msAgentKeynode;

SC_AGENT_IMPLEMENTATION(ASCPOperatorInterpreter)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr scp_operator = ms_context->GetArcEnd(edgeAddr);
    ScAddr type;

    if (SC_TRUE != Utils::resolveOperatorType((ScMemoryContext&)ms_context, scp_operator, type))
    {
        Utils::logUnknownOperatorTypeError((ScMemoryContext&)ms_context, scp_operator);
        return SC_RESULT_ERROR_INVALID_TYPE;
    }

    return SC_RESULT_OK;
}

}
