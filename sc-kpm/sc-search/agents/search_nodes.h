

#ifndef _search_nodes_h
#define _search_nodes_h

#include "sc-core/sc_memory.h"

/*!
 * Function that implement sc-agent to search all nodes in set represented by specified sc-element
 */
sc_result agent_search_all_nodes_in_set(const sc_event * event, sc_addr arg);

#endif
