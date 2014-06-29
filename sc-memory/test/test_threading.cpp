#include <stdio.h>
extern "C"
{
#include "sc_memory_headers.h"
#include "sc-store/sc_store.h"
}
#include <vector>
#include <limits>
#include <glib.h>

sc_memory_context * s_default_ctx = 0;

namespace
{
    typedef std::vector<GThread*> tGThreadVector;
}


void print_storage_statistics()
{
    sc_stat stat;

    sc_memory_stat(s_default_ctx, &stat);

    sc_uint64 nodes = stat.node_count;
    sc_uint64 nodes_del = nodes - stat.node_live_count;
    sc_uint64 arcs = stat.arc_count;
    sc_uint64 arcs_del = arcs - stat.arc_live_count;
    sc_uint64 links = stat.link_count;
    sc_uint64 links_del = links - stat.link_live_count;

    printf("--- Storage statistics: ---\n \tNodes: %llu (%llu deleted)\n\tArcs: %llu (%llu deleted)\n\tLinks: %llu (%llu deleted)\n\tEmpty: %llu\n---\n",
           nodes, nodes_del, arcs, arcs_del, links, links_del, stat.empty_count);
}

// simple test on node creation
gpointer create_node_thread(gpointer data)
{
    int count = GPOINTER_TO_INT(data);
    for (int i = 0; i < count; ++i)
    {
        sc_addr addr = sc_memory_node_new(s_default_ctx, 0);
        if (SC_ADDR_IS_EMPTY(addr))
            return GINT_TO_POINTER(i + 1);
    }
    return GINT_TO_POINTER(count);
}

void test_node_creation()
{
    static const int g_test_node_count = 100000;
    static const int g_test_thread_count = 16;

    tGThreadVector threads;
    threads.reserve(g_test_thread_count);

    g_test_timer_start();

    for (size_t i = 0; i < g_test_thread_count; ++i)
    {
        GThread * thread = g_thread_try_new(0, create_node_thread, GINT_TO_POINTER(g_test_node_count), 0);
        if (thread == 0)
            continue;
        threads.push_back(thread);
    }

    for (size_t i = 0; i < g_test_thread_count; ++i)
        g_assert_true(GPOINTER_TO_INT(g_thread_join(threads[i])) == g_test_node_count);

    printf("Time: %lf", g_test_timer_elapsed());
}


// ---------------------------
int main(int argc, char *argv[])
{
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.config_file = "sc-memory.ini";
    params.ext_path = 0;

    printf("sc_element: %d, sc_addr: %d, sc_arc: %d, sc_content: %d\n", sizeof(sc_element), sizeof(sc_addr), sizeof(sc_arc_info), sizeof(sc_content));

    s_default_ctx = sc_memory_initialize(&params);
    print_storage_statistics();

    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/threading/create_nodes", test_node_creation);
    g_test_run();

    print_storage_statistics();
    sc_memory_shutdown();
    return 0;
}
