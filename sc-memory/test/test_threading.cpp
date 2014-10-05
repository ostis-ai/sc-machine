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
sc_memory_params params;

namespace
{
    typedef std::vector<GThread*> tGThreadVector;
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

// simple test on node creation
gpointer create_node_thread(gpointer data)
{
    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make(8, 8));
    int count = GPOINTER_TO_INT(data);
    int result = count;
    for (int i = 0; i < count; ++i)
    {
        sc_addr addr = sc_memory_node_new(ctx, 0);
        if (SC_ADDR_IS_EMPTY(addr))
            result = i + 1;
    }

    result:
    {
        sc_memory_context_free(ctx);
    }
    return GINT_TO_POINTER(result);
}

// simple arc creation test
gpointer create_arc_thread(gpointer data)
{
    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make(8, 8));
    int count = GPOINTER_TO_INT(data);
    int result = count;
    for (int i = 0; i < count; ++i)
    {
        sc_addr addr = sc_memory_node_new(ctx, 0);
        if (SC_ADDR_IS_EMPTY(addr))
            goto result;
        sc_addr addr2 = sc_memory_node_new(ctx, 0);
        if (SC_ADDR_IS_EMPTY(addr2))
            goto result;
        sc_addr arc = sc_memory_arc_new(ctx, sc_type_arc_access, addr, addr);
        if (SC_ADDR_IS_EMPTY(arc))
            goto result;

        continue;

        error:
        {
            result = i + 1;
            break;
        }
    }

    result:
    {
        sc_memory_context_free(ctx);
    }
    return GINT_TO_POINTER(result);
}

// simple sc-links creation
gpointer create_link_thread(gpointer data)
{
    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make(8, 8));
    int count = GPOINTER_TO_INT(data);
    int result = count;
    for (int i = 0; i < count; ++i)
    {
        sc_addr addr = sc_memory_link_new(ctx);
        if (SC_ADDR_IS_EMPTY(addr))
            result = i + 1;
    }

    result:
    {
        sc_memory_context_free(ctx);
    }
    return GINT_TO_POINTER(result);
}


// -----------------------------------------

void test_creation(GThreadFunc f, sc_int32 count, sc_int thread_count)
{
    int test_count = count / thread_count;

    g_message("Threads count: %d, Test per thread: %d", thread_count, test_count);

    tGThreadVector threads;
    threads.reserve(thread_count);

    s_default_ctx = sc_memory_initialize(&params);
    print_storage_statistics();

    g_test_timer_start();
    for (size_t i = 0; i < thread_count; ++i)
    {
        GThread * thread = g_thread_try_new(0, f, GINT_TO_POINTER(test_count), 0);
        if (thread == 0)
            continue;
        threads.push_back(thread);
    }

    for (size_t i = 0; i < thread_count; ++i)
        g_assert_true(GPOINTER_TO_INT(g_thread_join(threads[i])) == test_count);

    printf("Time: %lf\n", g_test_timer_elapsed());

    print_storage_statistics();
    sc_memory_shutdown(SC_FALSE);
}

const sc_int32 g_thread_count = 64;
const sc_uint32 g_task_count = 1 << 23;

void test_node_creation()
{
    test_creation(create_node_thread, g_task_count, g_thread_count);
}

void test_arc_creation()
{
    test_creation(create_arc_thread, g_task_count, g_thread_count);
}

void test_link_creation()
{
    test_creation(create_link_thread, g_task_count, g_thread_count);
}

void test_combined_creation()
{
    int thread_count = g_thread_count;
    int test_count = (g_task_count) / thread_count;

    g_message("Threads count: %d, Test per thread: %d", thread_count, test_count);

    tGThreadVector threads;
    threads.reserve(thread_count);

    s_default_ctx = sc_memory_initialize(&params);
    print_storage_statistics();    

    g_test_timer_start();
    for (size_t i = 0; i < thread_count; ++i)
    {
        GThreadFunc f = create_node_thread;
        switch(g_random_int() % 3)
        {
        case 0:
            f = create_link_thread;
            break;
        case 1:
            f = create_arc_thread;
            break;
        default:
            break;
        }

        GThread * thread = g_thread_try_new(0, f, GINT_TO_POINTER(test_count), 0);
        if (thread == 0)
            continue;
        threads.push_back(thread);
    }

    for (size_t i = 0; i < thread_count; ++i)
        g_assert_true(GPOINTER_TO_INT(g_thread_join(threads[i])) == test_count);

    printf("Time: %lf\n", g_test_timer_elapsed());

    print_storage_statistics();
    sc_memory_shutdown(SC_FALSE);
}

// ---------------------------
int main(int argc, char *argv[])
{
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.config_file = "sc-memory.ini";
    params.ext_path = 0;

    printf("sc_element: %zd, sc_addr: %zd, sc_arc: %zd, sc_content: %zd\n", sizeof(sc_element), sizeof(sc_addr), sizeof(sc_arc_info), sizeof(sc_content));

    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/threading/create_nodes", test_node_creation);
    g_test_add_func("/threading/create_arcs", test_arc_creation);
    g_test_add_func("/threading/create_links", test_link_creation);
    g_test_add_func("/threading/create_combined", test_combined_creation);
    g_test_run();


    return 0;
}
