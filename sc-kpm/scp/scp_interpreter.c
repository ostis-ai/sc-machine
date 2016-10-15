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

#include "scp_interpreter.h"
#include "scp_keynodes.h"
#include "scp_procedure_preprocessor.h"
#include "scp_procedure_verifier.h"
#include "scp_process_creator.h"
#include "scp_process_destroyer.h"
#include "scp_operator_interpreter_agents.h"
#include "scp_interpreter_utils.h"
#include "scp_operator_syncronizer.h"

#include <stdio.h>

_SC_EXT_EXTERN sc_result initialize()
{
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);
    if (SCP_RESULT_TRUE == scp_lib_init() &&
        SCP_RESULT_TRUE == scp_keynodes_init() &&
        SCP_RESULT_TRUE == scp_process_destroyer_init() &&
        SCP_RESULT_TRUE == scp_operator_interpreter_agents_init() &&
        SCP_RESULT_TRUE == scp_procedure_preprocessor_init() &&
        SCP_RESULT_TRUE == scp_program_verifier_init() &&
        SCP_RESULT_TRUE == scp_operator_syncronizer_init() &&
        SCP_RESULT_TRUE == scp_process_creator_init())
        return SCP_RESULT_TRUE;
    else
        return SCP_RESULT_ERROR;
}

_SC_EXT_EXTERN sc_result shutdown()
{
    sc_memory_context_free(s_default_ctx);
    if (SCP_RESULT_TRUE == scp_lib_shutdown() &&
        SCP_RESULT_TRUE == scp_process_destroyer_shutdown() &&
        SCP_RESULT_TRUE == scp_operator_interpreter_agents_shutdown() &&
        SCP_RESULT_TRUE == scp_procedure_preprocessor_shutdown() &&
        SCP_RESULT_TRUE == scp_program_verifier_shutdown() &&
        SCP_RESULT_TRUE == scp_operator_syncronizer_shutdown() &&
        SCP_RESULT_TRUE == scp_process_creator_shutdown())
        return SCP_RESULT_TRUE;
    else
        return SCP_RESULT_ERROR;
}

_SC_EXT_EXTERN sc_uint32 load_priority()
{
	return 1;
}
