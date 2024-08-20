/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _search_incoming_arcs_h_
#define _search_incoming_arcs_h_

#include "sc-core/sc_memory.h"

/*!
 * Function that implement sc-agent to search all positive, constant and permanent
 * incoming sc-arcs for specified sc-element
 */
sc_result agent_search_all_const_pos_incoming_arc(sc_event_subscription const * event, sc_addr arg);

/*!
 * Function that implement sc-agent to search all positive, constant and permanent
 * incoming sc-arcs with relations for specified sc-element
 */
sc_result agent_search_all_const_pos_incoming_arc_with_rel(sc_event_subscription const * event, sc_addr arg);

#endif
