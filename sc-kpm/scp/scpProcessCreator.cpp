/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "scpProcessCreator.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{
ScAddr ASCPProcessCreator::msAgentKeynode;

SC_AGENT_IMPLEMENTATION(ASCPProcessCreator)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr action = ms_context->GetArcEnd(edgeAddr);

    if (!ms_context->HelperCheckArc(Keynodes::question_scp_interpretation_request, action, ScType::EdgeAccessConstPosPerm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr program, params;
    ScIterator5Ptr iter_param = ms_context->Iterator5(action, ScType::EdgeAccessConstPosPerm, ScType::Node, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_1);
    if (iter_param->Next())
        program = iter_param->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    iter_param = ms_context->Iterator5(action, ScType::EdgeAccessConstPosPerm, ScType::Node, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_2);
    if (iter_param->Next())
        params = iter_param->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScTemplate program_templ;
    ms_context->HelperBuildTemplate(program_templ, program);
    ScTemplateGenParams gen_params;

    ScAddr process_node;
    ScIterator5Ptr iter_temp = ms_context->Iterator5(Keynodes::scp_process, ScType::EdgeAccessConstPosPerm, ScType::NodeVar, ScType::EdgeAccessConstPosPerm, program);
    if (iter_temp->IsValid() && iter_temp->Next())
        process_node = iter_temp->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    iter_temp = ms_context->Iterator5(process_node, ScType::EdgeAccessConstPosPerm, ScType::NodeVar, ScType::EdgeAccessConstPosPerm, program);
    if (!iter_temp->IsValid())
        return SC_RESULT_ERROR_INVALID_PARAMS;

    while (iter_temp->Next())
    {
        ScAddr order;
        if (Utils::resolveOrderRoleRelation((ScMemoryContext&)ms_context, iter_temp->Get(1), order));
        {
            iter_param = ms_context->Iterator5(params, ScType::EdgeAccessConstPosPerm, ScType(0), ScType::EdgeAccessConstPosPerm, order);
            if (!iter_param->Next())
            {
#ifdef SCP_DEBUG
                Utils::logSCPError((ScMemoryContext&)ms_context, "Missed scp-program parameter", order);
#endif
                continue;
            }

            //!TODO gen_params.add(iter_temp->Get(2),iter_param->Get(2));
            //Utils::printSystemIdentifier((ScMemoryContext&)ms_context, iter_temp->Get(2));
            //std::cout << "-->" << std::endl;
            //Utils::printSystemIdentifier((ScMemoryContext&)ms_context, iter_param->Get(2));
            //std::cout << std::endl;
        }
    }

    return SC_RESULT_OK;
}

}
