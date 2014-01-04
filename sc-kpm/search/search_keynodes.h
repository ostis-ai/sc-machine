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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */

#ifndef _search_keynodes_h_
#define _search_keynodes_h_

#include "sc_memory.h"

extern sc_addr keynode_question_all_output_const_pos_arc;
extern sc_addr keynode_question_all_input_const_pos_arc;
extern sc_addr keynode_question_all_output_const_pos_arc_with_rel;
extern sc_addr keynode_question_all_input_const_pos_arc_with_rel;
extern sc_addr keynode_question_full_semantic_neighborhood;
extern sc_addr keynode_question_decomposition;
extern sc_addr keynode_question_all_identifiers;

extern sc_addr keynode_nrel_answer;
extern sc_addr keynode_quasybinary_relation;
extern sc_addr keynode_decomposition_relation;
extern sc_addr keynode_identification_relation;
extern sc_addr keynode_order_relation;
extern sc_addr keynode_question_finished;
extern sc_addr keynode_question_initiated;
extern sc_addr keynode_nrel_identification;
extern sc_addr keynode_set_nrel_inclusion;
extern sc_addr keynode_system_element;

//! Initialie keynodes that used by search module
sc_result search_keynodes_initialize();

#endif
