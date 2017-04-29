/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

#include "minecoPlanCreator.hpp"

namespace mineco
{
ScAddr AMinEcoPlanCreator::msAgentKeynode;
ScAddr AMinEcoPlanCreator::question_plan_creation;
ScAddr AMinEcoPlanCreator::question_initiated;
ScAddr AMinEcoPlanCreator::nrel_answer;

SC_AGENT_IMPLEMENTATION(AMinEcoPlanCreator)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr action = ms_context->GetArcEnd(edgeAddr);

    if (!ms_context->HelperCheckArc(question_plan_creation, action, sc_type_arc_pos_const_perm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr program;
    ScIterator3Ptr iter_param = ms_context->Iterator3(action, sc_type_arc_pos_const_perm, sc_type_node);
    if (iter_param->Next())
        program = iter_param->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScTemplate program_templ;
    ms_context->HelperBuildTemplate(program_templ, program);
    ScTemplateGenParams gen_params;

    /*ScAddr process_node;
    ScIterator5Ptr iter_temp = ms_context->Iterator5(Keynodes::scp_process, sc_type_arc_pos_var_perm, SC_TYPE(sc_type_var | sc_type_node), sc_type_arc_pos_const_perm, program);
    if (iter_temp->IsValid() && iter_temp->Next())
        process_node = iter_temp->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;*/

    /*iter_temp = ms_context->Iterator5(process_node, sc_type_arc_pos_var_perm, SC_TYPE(sc_type_var | sc_type_node), sc_type_arc_pos_const_perm, program);
    if (!iter_temp->IsValid())
        return SC_RESULT_ERROR_INVALID_PARAMS;*/

    /*while (iter_temp->Next())
    {
        ScAddr order;
        if (Utils::resolveOrderRoleRelation((ScMemoryContext&)ms_context, iter_temp->Get(1), order));
        {
            iter_param = ms_context->Iterator5(params, sc_type_arc_pos_const_perm, SC_TYPE(0), sc_type_arc_pos_const_perm, order);
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
    }*/

    ScTemplateGenResult res;
    ms_context->HelperGenTemplate(program_templ, res);

    ScAddr answer = ms_context->CreateNode(sc_type_const);

    for (int i = 0; i < res.GetSize(); i++)
    {
        ms_context->CreateArc(sc_type_arc_pos_const_perm, answer, res[i]);
    }
    ScAddr myarc = ms_context->CreateArc(SC_TYPE(sc_type_arc_common | sc_type_const), action, answer);
    ms_context->CreateArc(sc_type_arc_pos_const_perm, nrel_answer, myarc);

    return SC_RESULT_OK;
}

}
