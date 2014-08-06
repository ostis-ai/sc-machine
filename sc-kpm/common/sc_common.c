#include "sc_common.h"
#include "sc_agent_dispatcher.h"

sc_result initialize()
{
    return sc_kpm_agent_dispatcher_init();
}

sc_result shutdown()
{
    return sc_kpm_agent_dispatcher_shutdown();
}
