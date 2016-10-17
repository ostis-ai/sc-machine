/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "sc_memory_headers.h"
#include "scp_keynodes.h"
#include "scp_procedure_verifier.h"
#include "scp_interpreter_utils.h"
#include <glib.h>

#include <stdio.h>

sc_event *event_procedure_verifying;

sc_result verify_scp_program(const sc_event *event, sc_addr arg)
{
    scp_operand arc1, arc2, node1, program_node;
    MAKE_DEFAULT_OPERAND_FIXED(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);

    arc1.addr = arg;
    arc1.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(program_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &arc1))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &arc1))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &arc1, &program_node))
    {
        return SCP_RESULT_ERROR;
    }
    program_node.param_type = SCP_FIXED;
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &program_node))
    {
        return SC_RESULT_ERROR;

    }
    program_node.param_type = SCP_FIXED;

    //!TODO Add scp-program verifying
    if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &correct_scp_program, &arc2, &program_node))
    {
        return SC_RESULT_OK;
    }
    if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &incorrect_scp_program, &arc2, &program_node))
    {
        return SC_RESULT_OK;
    }

    genElStr3(s_default_ctx, &correct_scp_program, &arc2, &program_node);

	return SC_RESULT_OK;
}

scp_result verify_all_scp_programs(sc_memory_context *context)
{
    scp_operand proc, arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_DEFAULT_OPERAND_ASSIGN(proc);
    scp_iterator3 *it;
    it = scp_iterator3_new(context, &scp_program, &arc, &proc);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &scp_program, &arc, &proc))
    {
        proc.param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE == searchElStr3(context, &formed_scp_program, &arc, &proc))
        {
            proc.param_type = SCP_ASSIGN;
            continue;
        }
        genElStr3(context, &formed_scp_program, &arc, &proc);
        proc.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    return SC_RESULT_OK;
}

scp_result scp_program_verifier_init()
{
    event_procedure_verifying = sc_event_new(s_default_ctx, formed_scp_program.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)verify_scp_program, 0);
    if (event_procedure_verifying == null_ptr)
        return SCP_RESULT_ERROR;
    verify_all_scp_programs(s_default_ctx);
    return SCP_RESULT_TRUE;
}

scp_result scp_program_verifier_shutdown()
{
    sc_event_destroy(event_procedure_verifying);
    return SCP_RESULT_TRUE;
}
