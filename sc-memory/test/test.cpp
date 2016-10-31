/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
extern "C"
{
#include "sc-memory/sc_memory_headers.h"
#include "sc-memory/sc-store/sc_store.h"
#include "sc-memory/sc_helper.h"
}
#include <iostream>
#include <sstream>
#include <vector>
#include <limits>
#include <glib.h>
#include <cstdint>

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
    sc_stream * stream1 = sc_stream_memory_new(data, (sc_uint)strlen(data), SC_STREAM_FLAG_READ, SC_FALSE);
    sc_stream * stream2 = sc_stream_memory_new(data, (sc_uint)strlen(data), SC_STREAM_FLAG_READ, SC_FALSE);
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

    sc_stream * stream3 = sc_stream_memory_new(data, (sc_uint)strlen(data), SC_STREAM_FLAG_READ, SC_FALSE);
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

    // iterators
    addr1 = sc_memory_node_new(ctx2, 0);
    addr2 = sc_memory_node_new(ctx2, 0);
    el_addr = sc_memory_node_new(ctx1, 0);

    arc1 = sc_memory_arc_new(ctx2, sc_type_arc_pos_const_perm, addr1, addr2);
    arc2 = sc_memory_arc_new(ctx1, sc_type_arc_pos_const_perm, el_addr, arc1);

    sc_iterator3 *it3 = sc_iterator3_a_a_f_new(ctx2, 0, 0, addr2);
    g_assert(it3 != 0);
    g_assert(sc_iterator3_next(it3) == SC_TRUE);
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 0), addr1));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 1), arc1));
    g_assert(sc_iterator3_next(it3) == SC_FALSE);
    sc_iterator3_free(it3);

    it3 = sc_iterator3_f_a_a_new(ctx1, addr1, 0, 0);
    g_assert(it3 != 0);
    g_assert(sc_iterator3_next(it3) == SC_TRUE);
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 1), arc1));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 2), addr2));
    g_assert(sc_iterator3_next(it3) == SC_FALSE);
    sc_iterator3_free(it3);

    it3 = sc_iterator3_f_a_f_new(ctx1, addr1, 0, addr2);
    g_assert(it3 != 0);
    g_assert(sc_iterator3_next(it3) == SC_TRUE);
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 1), arc1));
    g_assert(sc_iterator3_next(it3) == SC_FALSE);
    sc_iterator3_free(it3);

    it3 = sc_iterator3_f_a_a_new(ctx2, el_addr, 0, 0);
    g_assert(it3 == 0);

    it3 = sc_iterator3_f_a_a_new(ctx1, el_addr, 0, 0);
    g_assert(it3 != 0);
    g_assert(sc_iterator3_next(it3) == SC_TRUE);
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 1), arc2));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 2), arc1));
    g_assert(sc_iterator3_next(it3) == SC_FALSE);
    sc_iterator3_free(it3);

    it3 = sc_iterator3_a_a_f_new(ctx2, 0, 0, arc1);
    g_assert(it3 != 0);
    g_assert(sc_iterator3_next(it3) == SC_FALSE);
    sc_iterator3_free(it3);

    sc_iterator5 *it5 = sc_iterator5_a_a_f_a_a_new(ctx2, 0, 0, addr2, 0, 0);
    g_assert(it5 != 0);
    g_assert(sc_iterator5_next(it5) == SC_FALSE);
    sc_iterator5_free(it5);

    it5 = sc_iterator5_a_a_f_a_a_new(ctx1, 0, 0, addr2, 0, 0);
    g_assert(it5 != 0);
    g_assert(sc_iterator5_next(it5) == SC_TRUE);
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 0), addr1));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 1), arc1));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 3), arc2));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 4), el_addr));
    g_assert(sc_iterator5_next(it5) == SC_FALSE);
    sc_iterator5_free(it5);

    it5 = sc_iterator5_a_a_f_a_f_new(ctx2, 0, 0, addr2, 0, el_addr);
    g_assert(sc_iterator5_next(it5) == SC_FALSE);
    sc_iterator5_free(it5);

    it5 = sc_iterator5_a_a_f_a_f_new(ctx1, 0, 0, addr2, 0, el_addr);
    g_assert(sc_iterator5_next(it5) == SC_TRUE);
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 0), addr1));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 1), arc1));
    g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 3), arc2));
    g_assert(sc_iterator5_next(it5) == SC_FALSE);
    sc_iterator5_free(it5);

    /// @todo add test for access levels in events

    sc_memory_context_free(ctx1);
    sc_memory_context_free(ctx2);

    shutdown_memory();
}

void test_deletion()
{
    initialize_memory();

    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make_max);

    // dest deletion of elements in iterator range (not current position of iterator)
    {
        sc_addr node = sc_memory_node_new(ctx, 0);

        sc_addr arcs[5], nodes[5];

        for (sc_uint32 i = 0; i < 5; ++i)
        {
            nodes[i] = sc_memory_node_new(ctx, 0);
            arcs[i] = sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, node, nodes[i]);
        }

        sc_iterator3 *it = sc_iterator3_f_a_a_new(ctx,
                                                  node,
                                                  sc_type_arc_pos_const_perm,
                                                  sc_type_node);
        /// @todo reqrite test to don't depend on order of iterator results.
        /// Now memory stores arcs in reverse to creation order
        sc_uint32 i = 5;
        while (sc_iterator3_next(it))
        {
            --i;
            g_assert(SC_ADDR_IS_EQUAL(nodes[i], sc_iterator3_value(it, 2)));
        }
        g_assert(i == 0);

        sc_iterator3_free(it);

        // delete on of arcs, and check iterator
        sc_memory_element_free(ctx, nodes[1]);
        i = 5;
        it = sc_iterator3_f_a_a_new(ctx,
                                    node,
                                    sc_type_arc_pos_const_perm,
                                    sc_type_node);
        while (sc_iterator3_next(it))
        {
            i--;
            if (i == 1)
                i--;
            g_assert(SC_ADDR_IS_EQUAL(nodes[i], sc_iterator3_value(it, 2)));
        }
        g_assert(i == 0);
    }

    // test deletion of currently pointed iterator elements
    {
        sc_addr node = sc_memory_node_new(ctx, 0);
        sc_addr nodes[5];

        for (sc_uint32 i = 0; i < 5; ++i)
        {
            nodes[i] = sc_memory_node_new(ctx, 0);
            sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, node, nodes[i]);
        }

        // remove arc that used in iteration
        sc_iterator3 *it = sc_iterator3_f_a_a_new(ctx,
                                                  node,
                                                  sc_type_arc_pos_const_perm,
                                                  sc_type_node);

        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), nodes[4]));
        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), nodes[3]));

        g_assert(sc_memory_element_free(ctx, sc_iterator3_value(it, 2)) == SC_RESULT_OK);
        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), nodes[2]));
        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), nodes[1]));
        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), nodes[0]));
        g_assert(sc_iterator3_next(it) == SC_FALSE);

        sc_iterator3_free(it);

        node = sc_memory_node_new(ctx, 0);
        for (sc_uint32 i = 0; i < 5; ++i)
        {
            nodes[i] = sc_memory_node_new(ctx, 0);
            sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, nodes[i], node);
        }

        it = sc_iterator3_a_a_f_new(ctx,
                                    sc_type_node,
                                    sc_type_arc_pos_const_perm,
                                    node);

        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), nodes[4]));
        g_assert(sc_memory_element_free(ctx, sc_iterator3_value(it, 0)) == SC_RESULT_OK);
        g_assert(sc_memory_element_free(ctx, nodes[0]) == SC_RESULT_OK);

        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), nodes[3]));
        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), nodes[2]));
        g_assert(sc_iterator3_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), nodes[1]));
        g_assert(sc_iterator3_next(it) == SC_FALSE);

        sc_iterator3_free(it);

        // remove source node
        node = sc_memory_node_new(ctx, 0);
        for (sc_uint32 i = 0; i < 5; ++i)
        {
            nodes[i] = sc_memory_node_new(ctx, 0);
            sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, node, nodes[i]);
        }

        it = sc_iterator3_f_a_a_new(ctx,
                                    node,
                                    sc_type_arc_pos_const_perm,
                                    0);

        g_assert(sc_memory_element_free(ctx, node) == SC_RESULT_OK);
        g_assert(sc_iterator3_next(it) == SC_FALSE);

        sc_iterator3_free(it);
    }

    // iterator 5
    {
        sc_addr node = sc_memory_node_new(ctx, 0);
        sc_addr n1[3], n2[3];

        for (sc_uint32 i = 0; i < 3; ++i)
        {
            n1[i] = sc_memory_node_new(ctx, 0);
            n2[i] = sc_memory_node_new(ctx, 0);

            sc_addr a = sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, node, n1[i]);
            sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, n2[i], a);
        }

        sc_iterator5 *it = sc_iterator5_f_a_a_a_a_new(ctx, node, 0, 0, 0, 0);

        g_assert(sc_iterator5_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), n1[2]));
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), n2[2]));

        g_assert(sc_memory_element_free(ctx, n1[1]) == SC_RESULT_OK);
        g_assert(sc_iterator5_next(it) == SC_TRUE);
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), n1[0]));
        g_assert(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), n2[0]));
        g_assert(sc_iterator5_next(it) == SC_FALSE);

        sc_iterator5_free(it);
    }

    // links
    {
        sc_addr link = sc_memory_link_new(ctx);

        char const *data = "test content";
        sc_stream *stream = sc_stream_memory_new(data, (sc_uint)strlen(data), SC_STREAM_FLAG_READ, SC_FALSE);
        g_assert(sc_memory_set_link_content(ctx, link, stream) == SC_RESULT_OK);

        sc_addr *results = 0;
        sc_uint32 count = 0;
        g_assert(sc_memory_find_links_with_content(ctx, stream, &results, &count) == SC_RESULT_OK);
        g_assert(count == 1);
        g_assert(SC_ADDR_IS_EQUAL(link, results[0]));

        g_assert(sc_memory_element_free(ctx, link) == SC_RESULT_OK);
        g_assert(sc_memory_find_links_with_content(ctx, stream, &results, &count) == SC_RESULT_ERROR_NOT_FOUND);
        g_assert(count == 0);

        sc_stream_free(stream);
    }

    sc_memory_context_free(ctx);
    shutdown_memory();
}

void test_states()
{
    initialize_memory();
    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make_max);

    sc_addr n1 = sc_memory_node_new(ctx, 0);
    sc_addr n2 = sc_memory_node_new(ctx, 0);
    sc_addr a = sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, n1, n2);

    g_assert(SC_ADDR_IS_NOT_EMPTY(a));
    g_assert(sc_memory_element_free(ctx, n1));
    a = sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, n1, n2);
    g_assert(SC_ADDR_IS_EMPTY(a));

    sc_memory_context_free(ctx);
    shutdown_memory();
}

sc_bool test_stream_equal(sc_stream const *s1, sc_stream const *s2)
{
    sc_uint32 l1, l2;
    g_assert(sc_stream_get_length(s1, &l1) == SC_RESULT_OK);
    g_assert(sc_stream_get_length(s2, &l2) == SC_RESULT_OK);

    if (l1 != l2)
        return SC_FALSE;

    sc_uint32 i, read;
    char b1, b2;
    sc_bool res = SC_TRUE;
    for (i = 0; i < l1; ++i)
    {
        g_assert(sc_stream_read_data(s1, &b1, 1, &read) == SC_RESULT_OK);
        g_assert(sc_stream_read_data(s2, &b2, 1, &read) == SC_RESULT_OK);
        if (b1 != b2)
        {
            res = SC_FALSE;
            break;
        }
    }

    return res;
}

void test_links()
{
    initialize_memory();
    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make_max);

    // check change of content
    {
        char const *data = "test content";
        sc_stream *stream = sc_stream_memory_new(data, (sc_uint)strlen(data), SC_STREAM_FLAG_READ, SC_FALSE);
        char const *data2 = "test content 2";
        sc_stream *stream2 = sc_stream_memory_new(data2, (sc_uint)strlen(data2), SC_STREAM_FLAG_READ, SC_FALSE);

        sc_addr link = sc_memory_link_new(ctx);
        g_assert(sc_memory_set_link_content(ctx, link, stream) == SC_RESULT_OK);
        g_assert(sc_memory_set_link_content(ctx, link, stream2) == SC_RESULT_OK);

        sc_addr *result;
        sc_uint32 count;
        g_assert(sc_memory_find_links_with_content(ctx, stream, &result, &count) == SC_RESULT_ERROR_NOT_FOUND);
        g_assert(count == 0);

        g_assert(sc_memory_find_links_with_content(ctx, stream2, &result, &count) == SC_RESULT_OK);
        g_assert(count == 1);

        sc_stream *rstream;
        g_assert(sc_memory_get_link_content(ctx, link, &rstream) == SC_RESULT_OK);
        g_assert(rstream != null_ptr);
        g_assert(test_stream_equal(stream2, rstream) == SC_TRUE);

        sc_stream_free(stream);
        sc_stream_free(stream2);
    }

    // test links store in memory
    {
        char const *data = "short content";
        char const *data2 = "very large content, that will be store in file memory";

        g_assert(strlen(data) < SC_CHECKSUM_LEN);
        g_assert(strlen(data2) >= SC_CHECKSUM_LEN);

        sc_addr link1 = sc_memory_link_new(ctx);
        sc_addr link2 = sc_memory_link_new(ctx);

        sc_stream *stream1 = sc_stream_memory_new(data, (sc_uint)strlen(data), SC_STREAM_FLAG_READ, SC_FALSE);
        sc_stream *stream2 = sc_stream_memory_new(data2, (sc_uint)strlen(data2), SC_STREAM_FLAG_READ, SC_FALSE);

        g_assert(sc_memory_set_link_content(ctx, link1, stream1) == SC_RESULT_OK);
        g_assert(sc_memory_set_link_content(ctx, link2, stream2) == SC_RESULT_OK);

        sc_stream *s1 = 0, *s2 = 0;
        g_assert(sc_memory_get_link_content(ctx, link1, &s1) == SC_RESULT_OK);
        g_assert(sc_memory_get_link_content(ctx, link2, &s2) == SC_RESULT_OK);

        g_assert(test_stream_equal(s1, stream1) == SC_TRUE);
        g_assert(test_stream_equal(s2, stream2) == SC_TRUE);

        sc_stream_free(s1);
        sc_stream_free(s2);

        g_assert(sc_stream_seek(stream1, SC_STREAM_SEEK_SET, 0) == SC_RESULT_OK);
        g_assert(sc_stream_seek(stream2, SC_STREAM_SEEK_SET, 0) == SC_RESULT_OK);

        // find links
        sc_addr *result;
        sc_uint32 count;
        g_assert(sc_memory_find_links_with_content(ctx, stream1, &result, &count) == SC_RESULT_OK);
        g_assert(count == 1);
        g_assert(SC_ADDR_IS_EQUAL(result[0], link1));

        g_assert(sc_memory_find_links_with_content(ctx, stream2, &result, &count) == SC_RESULT_OK);
        g_assert(count == 1);
        g_assert(SC_ADDR_IS_EQUAL(result[0], link2));

        g_assert(sc_memory_element_free(ctx, link1));
        g_assert(sc_memory_element_free(ctx, link2));

        g_assert(sc_memory_find_links_with_content(ctx, stream1, &result, &count) == SC_RESULT_ERROR_NOT_FOUND);
        g_assert(count == 0);

        g_assert(sc_memory_find_links_with_content(ctx, stream2, &result, &count) == SC_RESULT_ERROR_NOT_FOUND);
        g_assert(count == 0);

        sc_stream_free(stream1);
        sc_stream_free(stream2);
    }

    sc_memory_context_free(ctx);
    shutdown_memory();
}

// --------------------
namespace
{

std::string genIdtf(int idx)
{
    std::stringstream ss;
    ss << idx;
    return ss.str();
}

}

void test_save()
{
    sc_memory_params p;
    p.clear = SC_TRUE;
    p.repo_path = "repo";
    p.config_file = "sc-memory.ini";
    p.ext_path = 0;
    std::vector<sc_addr> addrs;

    static sc_uint32 const ADDRS_COUNT = 3000;
    addrs.reserve(ADDRS_COUNT);

    sc_memory_initialize(&p);
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_max);

    for (uint32_t i = 0; i < ADDRS_COUNT; ++i)
    {
        std::string const s = genIdtf(i);

        sc_addr addr = sc_memory_node_new(s_default_ctx, sc_type_node | sc_type_const);
        sc_helper_set_system_identifier(s_default_ctx, addr, s.c_str(), (sc_uint32)s.size());
        addrs.push_back(addr);
    }

    print_storage_statistics();

    sc_memory_context_free(s_default_ctx);
    sc_memory_shutdown(SC_TRUE);

    p.clear = SC_FALSE;
    sc_memory_initialize(&p);
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_max);
    print_storage_statistics();

    for (uint32_t i = 0; i < ADDRS_COUNT; ++i)
    {
        std::string const s = genIdtf(i);

        //sc_result sc_helper_find_element_by_system_identifier(sc_memory_context const * ctx, const sc_char* data, sc_uint32 len, sc_addr *result_addr);
        sc_addr addr;
        g_assert(sc_helper_find_element_by_system_identifier(s_default_ctx, s.c_str(), (sc_uint32)s.size(), &addr) == SC_RESULT_OK);
        g_assert(SC_ADDR_IS_EQUAL(addr, addrs[i]));
    }

    sc_memory_context_free(s_default_ctx);
    sc_memory_shutdown(SC_TRUE);

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
    /// TODO: add test for verion utils

    g_test_add_func("/common/save", test_save);
    g_test_add_func("/common/context", test_context);
    g_test_add_func("/common/access", test_access_levels);
    g_test_add_func("/common/deletion", test_deletion);
	g_test_add_func("/common/states", test_states);
    g_test_add_func("/common/links", test_links);
    g_test_run();


    return 0;
}
