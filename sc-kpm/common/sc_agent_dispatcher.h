#ifndef _sc_agent_dispatcher_h_
#define _sc_agent_dispatcher_h_

#include "sc_common_prerequest.h"

//! Returns name of agent
const char* sc_kpm_agent_get_name(sc_kpm_agent *a);
//! Returns sc_addr of agent
sc_addr sc_kpm_agent_get_addr(sc_kpm_agent *a);

// ------------------------

sc_result sc_kpm_agent_dispatcher_init();
sc_result sc_kpm_agent_dispatcher_shutdown();

/*! Register implementation of specified sc-agent
 * @param name String that containe system identifier of sc-agent
 * @param callback Pointer to callback function, that will be calls on to run agent
 * @param callback_del Pointer to callback function, that calls, when agent need to be destroyed
 * @return If agent registered, then returns pointer to structure that cotains it info; otherwise returns null
 */
sc_kpm_agent* sc_kpm_agent_dispatcher_register(const char * name, fEventCallback callback, fDeleteCallback callback_del);


#endif
