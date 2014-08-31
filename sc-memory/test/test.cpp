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
    contexts.resize(SC_ACCESS_LVL_MAX_VALUE * SC_ACCESS_LVL_MAX_VALUE);
    for (sc_uint32 i = 0; i < SC_ACCESS_LVL_MAX_VALUE; ++i)
    {
        for (sc_uint32 j = 0; j < SC_ACCESS_LVL_MAX_VALUE; ++j)
        {
            contexts[i * SC_ACCESS_LVL_MAX_VALUE + j] = sc_memory_context_new(sc_access_lvl_make(i, j));
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
void test_access_levels()
{
    sc_uint8 a = sc_access_lvl_make(SC_ACCESS_LVL_MIN_VALUE, SC_ACCESS_LVL_MIN_VALUE);
    sc_uint8 b = sc_access_lvl_make(SC_ACCESS_LVL_MAX_VALUE, SC_ACCESS_LVL_MAX_VALUE);

    g_assert(sc_access_lvl_min(a, b) == a);
    g_assert(sc_access_lvl_max(a, b) == b);

    a = sc_access_lvl_make(SC_ACCESS_LVL_MIN_VALUE, SC_ACCESS_LVL_MAX_VALUE);
    g_assert(sc_access_lvl_min(a, b) == sc_access_lvl_make(SC_ACCESS_LVL_MIN_VALUE, SC_ACCESS_LVL_MAX_VALUE));
    g_assert(sc_access_lvl_max(a, b) == b);
    g_assert(!sc_access_lvl_check_read(a, b));
    g_assert(sc_access_lvl_check_write(a, b));

    a = sc_access_lvl_make(SC_ACCESS_LVL_MAX_VALUE, SC_ACCESS_LVL_MIN_VALUE);
    g_assert(sc_access_lvl_min(a, b) == sc_access_lvl_make(SC_ACCESS_LVL_MAX_VALUE, SC_ACCESS_LVL_MIN_VALUE));
    g_assert(sc_access_lvl_max(a, b) == b);
    g_assert(sc_access_lvl_check_read(a, b));
    g_assert(!sc_access_lvl_check_write(a, b));



    initialize_memory();

    sc_access_levels al1 = sc_access_lvl_make(5, 5);
    sc_access_levels al2 = sc_access_lvl_make(SC_ACCESS_LVL_MIN_VALUE, SC_ACCESS_LVL_MIN_VALUE);
    sc_access_levels al_max = sc_access_lvl_make(SC_ACCESS_LVL_MAX_VALUE, SC_ACCESS_LVL_MAX_VALUE);

    sc_memory_context *ctx1 = sc_memory_context_new(al1);
    sc_memory_context *ctx2 = sc_memory_context_new(al2);

    sc_addr addr1 = sc_memory_node_new(ctx1, sc_type_node_abstract);
    sc_addr addr2 = sc_memory_node_new(ctx2, sc_type_node_class);

    // types
    sc_type type;
    sc_result r = sc_memory_get_element_type(ctx1, addr1, &type);
    g_assert(r == SC_RESULT_OK && (type | sc_type_node_abstract));

    r = sc_memory_get_element_type(ctx2, addr1, &type);
    g_assert(r == SC_RESULT_ERROR_NO_READ_RIGHTS);


    // get rights
    sc_access_levels al;
    r = sc_memory_get_element_access_levels(ctx1, addr1, &al);
    g_assert(r == SC_RESULT_OK && (al == al1));

    r = sc_memory_get_element_access_levels(ctx2, addr1, &al);
    g_assert(r == SC_RESULT_ERROR_NO_READ_RIGHTS);

    // set rights
    sc_access_levels alr;
    r = sc_memory_get_element_access_levels(ctx2, addr2, &al);
    g_assert(r == SC_RESULT_OK && (al == al2));

    r = sc_memory_set_element_access_levels(ctx1, addr2, al1, &alr);
    g_assert(r == SC_RESULT_OK && (alr == al1));

    r = sc_memory_set_element_access_levels(ctx1, addr2, al_max, &alr);
    g_assert(r == SC_RESULT_OK && (alr == al1));

    r = sc_memory_set_element_access_levels(ctx2, addr2, al_max, &alr);
    g_assert(r == SC_RESULT_ERROR_NO_WRITE_RIGHTS);

    // links
    const char * data = "Test";
    sc_stream * stream1 = sc_stream_memory_new(data, strlen(data), SC_STREAM_READ, SC_FALSE);
    sc_stream * stream2 = sc_stream_memory_new(data, strlen(data), SC_STREAM_READ, SC_FALSE);
    sc_addr link1 = sc_memory_link_new(ctx1);
    sc_addr link2 = sc_memory_link_new(ctx2);
    sc_stream * stream = 0;

    r = sc_memory_set_link_content(ctx2, link1, stream2);
    g_assert(r == SC_RESULT_ERROR_NO_WRITE_RIGHTS);

    r = sc_memory_set_link_content(ctx1, link1, stream1);
    g_assert(r == SC_RESULT_OK);

    r = sc_memory_set_link_content(ctx1, link2, stream2);
    g_assert(r == SC_RESULT_OK);

    r = sc_memory_get_link_content(ctx1, link2, &stream);
    g_assert(r == SC_RESULT_OK && stream);
    sc_stream_free(stream);

    r = sc_memory_get_link_content(ctx2, link1, &stream);
    g_assert(r == SC_RESULT_ERROR_NO_READ_RIGHTS);

    sc_stream * stream3 = sc_stream_memory_new(data, strlen(data), SC_STREAM_READ, SC_FALSE);
    sc_addr * result = 0;
    sc_uint32 count = 0;
    r = sc_memory_find_links_with_content(ctx1, stream3, &result, &count);
    g_assert(r == SC_RESULT_OK && (count == 2));
    g_assert((SC_ADDR_IS_EQUAL(result[0], link1) && SC_ADDR_IS_EQUAL(result[1], link2)) ||
            (SC_ADDR_IS_EQUAL(result[1], link1) && SC_ADDR_IS_EQUAL(result[0], link2)));

    r = sc_memory_find_links_with_content(ctx2, stream3, &result, &count);
    g_assert(r == SC_RESULT_OK && (count == 1) && (SC_ADDR_IS_EQUAL(result[0], link2)));


    // arcs
    sc_addr arc1 = sc_memory_arc_new(ctx1, 0, addr1, link1);
    sc_addr arc2 = sc_memory_arc_new(ctx2, 0, addr2, link2);

    sc_addr el_addr;

    r = sc_memory_get_arc_begin(ctx1, arc1, &el_addr);
    g_assert(r == SC_RESULT_OK && SC_ADDR_IS_EQUAL(el_addr, addr1));

    r = sc_memory_get_arc_end(ctx1, arc2, &el_addr);
    g_assert(r == SC_RESULT_OK && SC_ADDR_IS_EQUAL(el_addr, link2));

    r = sc_memory_get_arc_end(ctx2, arc1, &el_addr);
    g_assert(r == SC_RESULT_ERROR_NO_READ_RIGHTS);

    r = sc_memory_get_arc_begin(ctx2, arc1, &el_addr);
    g_assert(r == SC_RESULT_ERROR_NO_READ_RIGHTS);

    // deletion
    sc_addr arc3 = sc_memory_arc_new(ctx1, 0, addr2, link1);

    r = sc_memory_element_free(ctx2, addr2);
    g_assert(r == SC_RESULT_ERROR_NO_WRITE_RIGHTS);

    r = sc_memory_element_free(ctx1, addr2);
    g_assert(r == SC_RESULT_OK);

    g_assert(!sc_memory_is_element(ctx2, arc3));



    sc_memory_context_free(ctx1);
    sc_memory_context_free(ctx2);

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

    g_test_add_func("/common/access_levels", test_access_levels);
    g_test_run();


    return 0;
}
