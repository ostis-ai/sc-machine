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

#ifndef _search_operations_h_
#define _search_operations_h_

#include "sc_memory.h"

/*! SC-operation to search all constant positive output arcs for given element
 * @return Returns SC_RESULT_ERROR, if system error appeared. Otherwise returns SC_RESULT_OK.
 */
sc_result operation_search_all_const_pos_output_arc(sc_event *event, sc_addr arg);

/*! SC-operation to search all constant positive input arcs for given element
 * @return Returns SC_RESULT_ERROR, if system error appeared. Otherwise returns SC_RESULT_OK.
 */
sc_result operation_search_all_const_pos_input_arc(sc_event *event, sc_addr arg);

/*! SC-operation to search all constant positive output arcs for given element with
 * relations, in which these arcs are members
 * @return Returns SC_RESULT_ERROR, if system error appeared. Otherwise returns SC_RESULT_OK.
 */
sc_result operation_search_all_const_pos_output_arc_with_rel(sc_event *event, sc_addr arg);

/*! SC-operation to search all constant positive input arcs for given element with
 * relations, in which these arcs are members
 * @return Returns SC_RESULT_ERROR, if system error appeared. Otherwise returns SC_RESULT_OK.
 */
sc_result operation_search_all_const_pos_input_arc_with_rel(sc_event *event, sc_addr arg);

/*! SC-operation to search full semantic neighbourhood of given element
 * @return Returns SC_RESULT_ERROR, if system error appeared. Otherwise returns SC_RESULT_OK.
 */
sc_result operation_search_full_semantic_neighbourhood(sc_event *event, sc_addr arg);

#endif
