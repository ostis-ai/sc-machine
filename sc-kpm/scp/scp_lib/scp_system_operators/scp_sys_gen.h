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

#ifndef SCP_SYS_GEN_H
#define SCP_SYS_GEN_H

#include "../scp_types.h"

scp_result sys_gen(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand_pair *parameters, sc_uint32 param_count, scp_operand *param4);
scp_result sys_gen_for_variables(sc_memory_context *context, scp_operand *param1, scp_operand_pair *variables, sc_uint32 var_count, scp_operand_pair *parameters, sc_uint32 param_count, scp_operand *param4);

#endif // SCP_SYS_GEN_H
