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
    if (!edgeAddr.isValid())
        return SC_RESULT_ERROR;

    ScAddr action = msContext->getArcEnd(edgeAddr);

    if (!msContext->helperCheckArc(Keynodes::question_scp_interpretation_request, action, sc_type_arc_pos_const_perm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr program, params;
    ScIterator5Ptr iter_param = msContext->iterator5(action, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, Keynodes::rrel_1);
    if (iter_param->next())
        program = iter_param->value(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    iter_param = msContext->iterator5(action, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, Keynodes::rrel_2);
    if (iter_param->next())
        params = iter_param->value(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScTemplate program_templ;
    msContext->helperBuildTemplate(program_templ, program);
    ScTemplateGenParams gen_params;

    ScAddr process_node;
    ScIterator5Ptr iter_temp = msContext->iterator5(Keynodes::scp_process, sc_type_arc_pos_var_perm, SC_TYPE(sc_type_var | sc_type_node), sc_type_arc_pos_const_perm, program);
    if (iter_temp->isValid() && iter_temp->next())
        process_node = iter_temp->value(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    iter_temp = msContext->iterator5(process_node, sc_type_arc_pos_var_perm, SC_TYPE(sc_type_var | sc_type_node), sc_type_arc_pos_const_perm, program);
    if (!iter_temp->isValid())
        return SC_RESULT_ERROR_INVALID_PARAMS;

    while (iter_temp->next())
    {
        ScAddr order;
        if (Utils::resolveOrderRoleRelation((ScMemoryContext&)msContext, iter_temp->value(1), order));
        {
            iter_param = msContext->iterator5(params, sc_type_arc_pos_const_perm, SC_TYPE(0), sc_type_arc_pos_const_perm, order);
            if (!iter_param->next())
                Utils::logMissedParameterError((ScMemoryContext&)msContext, order);

            //!TODO gen_params.add(iter_temp->value(2),iter_param->value(2));
            //Utils::printSystemIdentifier((ScMemoryContext&)msContext, iter_temp->value(2));
            //std::cout << "-->" << std::endl;
            //Utils::printSystemIdentifier((ScMemoryContext&)msContext, iter_param->value(2));
            //std::cout << std::endl;
        }
    }

    return SC_RESULT_OK;
}

}
