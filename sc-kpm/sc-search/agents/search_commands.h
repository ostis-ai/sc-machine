

#ifndef _search_commands_h
#define _search_commands_h

#include "sc-core/sc_memory.h"

/*!
 * Function that implement sc-agent to search all atomic commands
 */
sc_result agent_search_atomic_commands(const sc_event * event, sc_addr arg);

#endif
