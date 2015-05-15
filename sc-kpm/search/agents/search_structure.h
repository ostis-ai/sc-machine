/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _search_structure_h_
#define _search_structure_h_

#include <sc_memory.h>

/*!
 * Function, that implements sc-agent to search decomposition of specified sc-element
 */
sc_result agent_search_decomposition(const sc_event *event, sc_addr arg);

/*!
 * Function, that implements sc-agent to search all subclasses of specified sc-element in all quasybinary relations
 */
sc_result agent_search_all_subclasses_in_quasybinary_relation(const sc_event *event, sc_addr arg);

/*!
 * Function, that implements sc-agent to search all superclasses of specified sc-element in all quasybinary relations
 */
sc_result agent_search_all_superclasses_in_quasybinary_relation(const sc_event *event, sc_addr arg);

#endif
