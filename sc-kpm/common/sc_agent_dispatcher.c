#include "private/sc_agent_dispatcher_p.h"

#include "sc_agent_dispatcher.h"
#include "sc_common_utils.h"


sc_kpm_agent* sc_kpm_agent_new(sc_addr addr, const char *name, fEventCallback callback, fDeleteCallback callback_del)
{
    sc_kpm_agent *agent = g_new0(sc_kpm_agent, 1);

    agent->addr = addr;
    agent->name = g_strdup(name);
    agent->callback = callback;
    agent->callback_del = callback_del;

    return agent;
}

void sc_kpm_agent_free(sc_kpm_agent *a)
{
    g_free(a->name);
    g_free(a);
}

const char* sc_kpm_agent_get_name(sc_kpm_agent *a)
{
    g_assert(a != 0);
    return a->name;
}

sc_addr sc_kpm_agent_get_addr(sc_kpm_agent *a)
{
    g_assert(a != 0);
    return a->addr;
}

// -----------------------------------
const char str_keynode_abstract_agent[] = "abstract_sc_agent";

sc_addr keynode_abstract_agent;

sc_memory_context *s_context = 0;
GThreadPool *s_thread_pool = 0;

// -----------------------------------
void _thread_worker(gpointer data, gpointer user_data)
{
    sc_memory_context * ctx = (sc_memory_context*)g_atomic_pointer_get(&user_data);
}


sc_result sc_kpm_agent_dispatcher_init()
{
    s_context = sc_memory_context_new(sc_access_lvl_make(SC_ACCESS_LVL_MAX_VALUE, SC_ACCESS_LVL_MAX_VALUE));

    resolve_keynode(s_context, keynode_abstract_agent);

    // initialize thread pool
    sc_int32 threads = sc_config_get_value_int("kpm", "max_threads");
    sc_int32 max_threads = MAX(1, MIN(512, threads));
    s_thread_pool = g_thread_pool_new(_thread_worker, s_context, max_threads, TRUE, 0);
    g_message("Start thread pool for agents - max_threads: %d", g_thread_pool_get_max_threads(s_thread_pool));

    if (s_thread_pool == 0)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_result sc_kpm_agent_dispatcher_shutdown()
{
    if (s_thread_pool)
    {
        g_thread_pool_free(s_thread_pool, FALSE, TRUE);
        s_thread_pool = 0;
    }

    if (s_context)
    {
        sc_memory_context_free(s_context);
        s_context = 0;
    }

    return SC_RESULT_OK;
}

sc_kpm_agent* sc_kpm_agent_dispatcher_register(const char * name, fEventCallback callback, fDeleteCallback callback_del)
{
    sc_addr agent_addr;

    if (sc_helper_find_element_by_system_identifier(s_context, name, strlen(name), &agent_addr) != SC_RESULT_OK)
        return 0;

    sc_kpm_agent *a = sc_kpm_agent_new(agent_addr, name, callback, callback_del);
    g_message("Agent %s registered: %d, %d", sc_kpm_agent_get_name(a), agent_addr.seg, agent_addr.offset);
    return a;
}
