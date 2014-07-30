#ifndef _sc_agent_dispatcher_h_
#define _sc_agent_dispatcher_h_

#include "../sc_common_prerequest.h"

//! Struct to store whole info about sc-agent
struct _sc_kpm_agent
{
    sc_addr addr;
    char *name;
    fEventCallback callback;
    fDeleteCallback callback_del;
};


#endif
