/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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

#include "sc_memory.h"

#ifndef _search_functions_h_
#define _search_functions_h_

/*! Search all constant positive permanent arcs, which outcome from given element
 * @param addr sc-addr of element
 * @param answer sc-addr of set, which will contain all the elements of answer.
 * @return Returns SC_FALSE, if system error appeared. Otherwise returns SC_TRUE.
 */
sc_bool search_all_const_pos_output_arc(sc_addr node, sc_addr answer);

/*! Search all constant positive permanent arcs, which income in given element
 * @param addr sc-addr of element
 * @param answer sc-addr of set, which will contain all the elements of answer.
 * @return Returns SC_FALSE, if system error appeared. Otherwise returns SC_TRUE.
 */
sc_bool search_all_const_pos_input_arc(sc_addr node, sc_addr answer);

/*! Search all constant positive permanent arcs, which income in given element,
 * including signs of role relations, linked with these arcs.
 * @param addr sc-addr of element
 * @param answer sc-addr of set, which will contain all the elements of answer.
 * @return Returns SC_FALSE, if system error appeared. Otherwise returns SC_TRUE.
 */
sc_bool search_all_const_pos_input_arc_with_rel(sc_addr node, sc_addr answer);

/*! Search all constant positive permanent arcs, which outcome from given element,
 * including signs of role relations, linked with these arcs.
 * @param addr sc-addr of element
 * @param answer sc-addr of set, which will contain all the elements of answer.
 * @return Returns SC_FALSE, if system error appeared. Otherwise returns SC_TRUE.
 */
sc_bool search_all_const_pos_output_arc_with_rel(sc_addr node, sc_addr answer);

/*! Search full semantic neighbourhood of given element
 * @param addr sc-addr of element
 * @param answer sc-addr of set, which will contain all the elements of answer.
 * @return Returns SC_FALSE, if system error appeared. Otherwise returns SC_TRUE.
 */
sc_bool search_full_semantic_neighbourhood(sc_addr node, sc_addr answer);

/*! Function to find single parameter of given question
 * @param question_class sc-addr of element which is sign of question class
 * @param question_node sc-addr of element which is sign of question itself
 * @param param sc-addr of element which is single parameter of question
 * @return Returns SC_FALSE, if system error appeared. Otherwise returns SC_TRUE.
 */
sc_bool get_question_single_param(sc_addr question_class, sc_addr question_node, sc_addr *param);

#endif
