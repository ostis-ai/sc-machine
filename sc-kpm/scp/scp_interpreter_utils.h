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

#ifndef SCP_INTERPRETER_UTILS_H
#define SCP_INTERPRETER_UTILS_H

#include "scp_lib.h"
#include <glib.h>

scp_result resolve_operator_type(sc_memory_context *context, scp_operand *oper, scp_operand *type);
scp_result resolve_scp_event_type(sc_memory_context *context, scp_operand *element, sc_event_type *type);
scp_result resolve_ordinal_rrel(sc_memory_context *context, scp_operand *arc_param, scp_operand *result);
scp_result resolve_ordinal_set_rrel(sc_memory_context *context, scp_operand *arc_param, scp_operand *result);
scp_result get_set_power(sc_memory_context *context, scp_operand *set, scp_uint32 *result);

scp_result check_scp_interpreter_question(sc_memory_context *context, scp_operand *quest);

void finish_question_successfully(sc_memory_context *context, scp_operand *param);
void finish_question_unsuccessfully(sc_memory_context *context, scp_operand *param);

void set_author(sc_memory_context *context, scp_operand *quest, scp_operand *author);

void mark_scp_process_as_useless(sc_memory_context *context, scp_operand *param);

void set_active_operator(sc_memory_context *context, scp_operand *scp_operator_node);

void run_scp_program(sc_memory_context *context, scp_operand *scp_program);

void cantorize_set(sc_memory_context *context, scp_operand *set);
void erase_var_set_values(sc_memory_context *context, scp_operand *set);
void load_set_to_hash(sc_memory_context *context, scp_operand *set, GHashTable *table);
sc_addr resolve_sc_addr_from_pointer(gpointer data);
sc_addr resolve_sc_addr_from_int(scp_uint32 data);
#endif // SCP_INTERPRETER_UTILS_H
