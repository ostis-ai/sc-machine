/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "scpPrintOperatorInterpreter.hpp"
#include "print_operators/SCPOperatorPrintEl.hpp"
#include "print_operators/SCPOperatorPrint.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{
ScAddr ASCPPrintOperatorInterpreter::msAgentKeynode;

SC_AGENT_IMPLEMENTATION(ASCPPrintOperatorInterpreter)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr scp_operator = ms_context->GetArcEnd(edgeAddr);

    ScAddr type;
    if (SC_TRUE != Utils::resolveOperatorType((ScMemoryContext&)ms_context, scp_operator, type))
        return SC_RESULT_ERROR_INVALID_TYPE;

    SCPOperator* oper = nullptr;
    if (type == Keynodes::op_printEl)
    {
        oper = new SCPOperatorPrintEl((ScMemoryContext&)ms_context, scp_operator);
    }
    if (type == Keynodes::op_print || type == Keynodes::op_printNl)
    {
        sc_bool newline = (type == Keynodes::op_printNl);
        oper = new SCPOperatorPrint((ScMemoryContext&)ms_context, scp_operator, newline);
    }

    if (oper == nullptr)
        return SC_RESULT_ERROR_INVALID_PARAMS;

#ifdef SCP_DEBUG
    std::cout << oper->GetTypeName() << std::endl;
#endif
    sc_result parse_result = oper->Parse();
    if (parse_result != SC_RESULT_OK)
    {
        delete oper;
        return parse_result;
    }
    else
    {
        sc_result execute_result;
        execute_result = oper->Execute();
        delete oper;
        return execute_result;
    }
}

}
