/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _search_identification_h_
#define _search_identification_h_

#include <sc_memory.h>


/*!
 * Function that implement sc-agent to search all identifiers of specified sc-elements
 */
sc_result agent_search_all_identifiers(const sc_event *event, sc_addr arg);

/*!
 * Function that implement sc-agent to search all sc-elements identified with main identifier
 */
sc_result agent_search_all_identified_elements(const sc_event *event, sc_addr arg);

#endif
