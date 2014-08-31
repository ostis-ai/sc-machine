#include <stdio.h>
extern "C"
{
#include "sc_memory_headers.h"
#include "sc-store/sc_store.h"
}
#include <iostream>
#include <vector>
#include <limits>
#include <glib.h>

sc_memory_context * s_default_ctx = 0;
sc_memory_params params;

// ------
void initialize_memory()
{
    s_default_ctx = sc_memory_initialize(&params);
}

void shutdown_memory()
{
    sc_memory_shutdown(SC_FALSE);
    s_default_ctx = 0;
}

void print_storage_statistics()
{
    sc_stat stat;

    sc_memory_stat(s_default_ctx, &stat);

    sc_uint32 nodes = stat.node_count;
    sc_uint32 arcs = stat.arc_count;
    sc_uint32 links = stat.link_count;

    printf("--- Storage statistics: ---\n \tSegments: %u\n\tNodes: %u\n\tArcs: %u\n\tLinks: %u\n\tEmpty: %u\n---\n",
           stat.segments_count, nodes, arcs, links, stat.empty_count);
}

// -------------------------
gpointer create_context(gpointer data)
{
    std::vector<sc_memory_context*> contexts;
    contexts.resize(sc_access_level_max * sc_access_level_max);
    for (sc_uint32 i = 0; i < sc_access_level_max; ++i)
    {
        for (sc_uint32 j = 0; j < sc_access_level_max; ++j)
        {
            contexts[i * sc_access_level_max + j] = sc_memory_context_new(sc_access_levels_make(i, j));
            g_usleep(g_random_int() % 100);
        }
    }


    for (sc_uint32 i = 0; i < contexts.size(); ++i)
        sc_memory_context_free(contexts[i]);

    return 0;
}

void test_context()
{
    static const sc_uint32 thread_count = 128;

    initialize_memory();

    std::vector<GThread*> threads;
    threads.resize(thread_count);

    std::cout << "Threads: " << thread_count << std::endl;
    for (sc_uint32 i = 0; i < thread_count; ++i)
        threads[i] = g_thread_new("Context test", create_context, 0);

    for (sc_uint32 i = 0; i < thread_count; ++i)
        g_thread_join(threads[i]);

    print_storage_statistics();

    shutdown_memory();
}

// ---------------------------
int main(int argc, char *argv[])
{
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.config_file = "sc-memory.ini";
    params.ext_path = 0;

    printf("sc_element: %zd, sc_addr: %zd, sc_arc: %zd, sc_content: %zd", sizeof(sc_element), sizeof(sc_addr), sizeof(sc_arc_info), sizeof(sc_content));

    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/common/context", test_context);
    g_test_run();


    return 0;
}
