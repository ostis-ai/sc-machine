/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_segment.h"
#include "sc_element.h"
#include "sc_storage.h"
#include "../sc_memory_private.h"

#include <glib.h>

#define MAX_LOCK_SLEEP      10 // microseconds
#define LOCK_SLEEP() //{ g_usleep(g_random_int() % MAX_LOCK_SLEEP); }

sc_segment* sc_segment_new(sc_addr_seg num)
{
    sc_segment *segment = g_new0(sc_segment, 1);

    // initialize empty count for sections
    sc_uint16 count = SC_SEGMENT_ELEMENTS_COUNT / SC_CONCURRENCY_LEVEL;
    sc_uint16 i, c = count * SC_CONCURRENCY_LEVEL;
    for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
    {
        sc_segment_section *section = &(segment->sections[i]);
        section->empty_offset = i;
        section->empty_count = ((c + i) <= SC_SEGMENT_ELEMENTS_COUNT) ? count + 1 : count;
    }

    if (num == 0)
    {
        segment->sections[0].empty_offset += SC_CONCURRENCY_LEVEL;
        segment->sections[0].empty_count--;
    }

    segment->num = num;

    return segment;
}

void sc_segment_loaded(sc_segment * seg)
{
    sc_uint32 i;
    seg->elements_count = 0;

    for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
    {
        sc_segment_section * section = &seg->sections[i];
        sc_uint32 idx = i;

        section->empty_count = 0;
        while (idx < SC_SEGMENT_ELEMENTS_COUNT)
        {
            if (seg->elements[idx].flags.type == 0)
            {
                ++section->empty_count;
                section->empty_offset = idx;
            }
            else
                ++seg->elements_count;
            idx += SC_CONCURRENCY_LEVEL;
        }
    }
}

void sc_segment_free(sc_segment *segment)
{
    g_assert( segment != 0);

    g_free(segment);
}

void sc_segment_erase_element(sc_segment *seg, sc_uint16 offset)
{
    g_assert(g_atomic_pointer_get(&seg->sections[offset % SC_CONCURRENCY_LEVEL].ctx_lock) != 0);
    g_atomic_int_dec_and_test(&seg->elements_count);

    g_assert( seg != (sc_segment*)0 );
    g_assert( offset < SC_SEGMENT_ELEMENTS_COUNT );
    memset(&seg->elements[offset], 0, sizeof(sc_element));

    sc_segment_section *section = &(seg->sections[offset % SC_CONCURRENCY_LEVEL]);
    g_atomic_int_inc(&section->empty_count);
    g_atomic_int_set(&section->empty_offset, offset);

    g_assert(offset != 0 || seg->num != 0);
}

sc_uint32 sc_segment_get_elements_count(sc_segment *seg)
{
    g_assert(seg != null_ptr);

    return g_atomic_int_get(&seg->elements_count);
}

sc_bool sc_segment_has_empty_slot(sc_segment *segment)
{
    g_assert(segment != null_ptr);
    if (segment->num == 0)
        return g_atomic_int_get(&segment->elements_count) < SC_SEGMENT_ELEMENTS_COUNT - 1;

    return g_atomic_int_get(&segment->elements_count) < SC_SEGMENT_ELEMENTS_COUNT;
}

void sc_segment_collect_elements_stat(const sc_memory_context *ctx, sc_segment *seg, sc_stat *stat)
{
    sc_int32 i;
    for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
    {
        sc_segment_section * section = &seg->sections[i];
        sc_segment_section_lock(ctx, section);
        sc_int32 j = i;
        while (j < SC_SEGMENT_ELEMENTS_COUNT)
        {
            sc_type type = seg->elements[j].flags.type;
            if (type & sc_type_node)
                stat->node_count++;
            else
            {
                if (type & sc_type_link)
                    stat->link_count++;
                else
                {
                    if (type & sc_type_arc_mask)
                        stat->arc_count++;
                    else
                    {
                        // do not use empty count, because it can be changed
                        if (type == 0)
                            stat->empty_count++;
                    }
                }
            }

            j += SC_CONCURRENCY_LEVEL;
        }
        sc_segment_section_unlock(ctx, section);

    }
}

sc_element_meta* sc_segment_get_meta(const sc_memory_context *ctx, sc_segment * seg, sc_addr_offset offset)
{
    g_assert(seg != null_ptr);
    g_assert(seg->sections[offset % SC_CONCURRENCY_LEVEL].ctx_lock == ctx);
    g_assert(offset < SC_SEGMENT_ELEMENTS_COUNT);

    return &seg->meta[offset];
}

// ---------------------------
sc_element* sc_segment_lock_empty_element(const sc_memory_context *ctx, sc_segment *seg, sc_addr_offset *offset)
{
    sc_uint16 max_attempts = 1;
    while (sc_segment_has_empty_slot(seg) == SC_TRUE)
    {
        sc_uint32 i;
        for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
        {
            sc_uint32 sec_id = (ctx->id + i) % SC_CONCURRENCY_LEVEL;
            sc_segment_section * section = &seg->sections[sec_id];

            if (g_atomic_int_get(&section->empty_count) == 0)
                continue;

            sc_bool locked = sc_segment_section_lock_try(ctx, section, 1);

            if (locked == SC_TRUE)
            {
                sc_int32 idx = (sc_int32)section->empty_offset;

                if (section->empty_count > 0)
                {
                    g_atomic_int_inc(&seg->elements_count);
                    g_atomic_int_add(&section->empty_count, -1);

                    // trying to find empty element in section
                    g_assert(idx >= 0 && idx < SC_SEGMENT_ELEMENTS_COUNT);
                    g_assert(seg->num + idx > 0);   // not empty addr

                    // need to find new empty element
                    sc_int32 j = idx + SC_CONCURRENCY_LEVEL;
                    while (j < SC_SEGMENT_ELEMENTS_COUNT)
                    {
                        if (seg->elements[j].flags.type == 0)
                        {
                            g_atomic_int_set(&section->empty_offset, j);
                            g_assert(seg->num + j > 0);
                            goto result;
                        }
                        j += SC_CONCURRENCY_LEVEL;
                    }
                    j = idx - SC_CONCURRENCY_LEVEL;
                    sc_int left = (i == 0 && seg->num == 0) ? SC_CONCURRENCY_LEVEL : idx;
                    while (j >= left)
                    {
                        if (seg->elements[j].flags.type == 0)
                        {
                            g_atomic_int_set(&section->empty_offset, j);
                            g_assert(seg->num + j > 0);
                            goto result;
                        }

                        j -= SC_CONCURRENCY_LEVEL;
                    }

                }

                sc_segment_section_unlock(ctx, section);
                continue;

                result:
                {
                    g_assert(g_atomic_int_get(&section->empty_count) >= 0);
                    *offset = idx;
                    return &seg->elements[*offset];
                }
            }

        }

        if (max_attempts < SC_CONCURRENCY_LEVEL)
            ++max_attempts;
        else
            return null_ptr;
    }

    return null_ptr;
}

sc_element* sc_segment_lock_element(const sc_memory_context *ctx, sc_segment *seg, sc_addr_offset offset)
{
    g_assert(offset < SC_SEGMENT_ELEMENTS_COUNT && seg != null_ptr);
    sc_segment_section *section = &seg->sections[offset % SC_CONCURRENCY_LEVEL];
    sc_segment_section_lock(ctx, section);
    return &seg->elements[offset];
}

sc_element* sc_segment_lock_element_try(const sc_memory_context *ctx, sc_segment *seg, sc_addr_offset offset, sc_uint16 max_attempts)
{
    g_assert(offset < SC_SEGMENT_ELEMENTS_COUNT && seg != null_ptr);
    sc_segment_section *section = &seg->sections[offset % SC_CONCURRENCY_LEVEL];

    if (sc_segment_section_lock_try(ctx, section, max_attempts) == SC_TRUE)
        return &seg->elements[offset];

    return (sc_element*)0;
}

void sc_segment_unlock_element(const sc_memory_context *ctx, sc_segment *seg, sc_addr_offset offset)
{
    g_assert(offset < SC_SEGMENT_ELEMENTS_COUNT && seg != null_ptr);
    sc_segment_section *section = &seg->sections[offset % SC_CONCURRENCY_LEVEL];
    sc_segment_section_unlock(ctx, section);
}

void sc_segment_section_lock(const sc_memory_context *ctx, sc_segment_section *section)
{
    lock:
    {
        while (g_atomic_int_compare_and_exchange(&section->internal_lock, 0, 1) == FALSE)
        {
            LOCK_SLEEP();
        }
    }

    if (g_atomic_pointer_get(&section->ctx_lock) != 0 && g_atomic_pointer_get(&section->ctx_lock) != ctx)
    {
        g_atomic_int_set(&section->internal_lock, 0);
        goto lock;
    }

    g_atomic_pointer_set(&section->ctx_lock, ctx);
    g_atomic_int_inc(&section->lock_count);

    g_atomic_int_set(&section->internal_lock, 0);
}

sc_bool sc_segment_section_lock_try(const sc_memory_context *ctx, sc_segment_section *section, sc_uint16 max_attempts)
{
    g_assert(section != null_ptr);
    sc_uint16 attempts = 0;

    lock:
    {
        while (g_atomic_int_compare_and_exchange(&section->internal_lock, 0, 1) == FALSE)
        {
            LOCK_SLEEP();
            if (max_attempts < attempts++)
                return SC_FALSE;
        }
    }

    if (g_atomic_pointer_get(&section->ctx_lock) != 0 && g_atomic_pointer_get(&section->ctx_lock) != ctx)
    {
        g_atomic_int_set(&section->internal_lock, 0);
        if (++attempts >= max_attempts)
            return SC_FALSE;
        goto lock;
    }

    g_atomic_pointer_set(&section->ctx_lock, ctx);
    g_atomic_int_inc(&section->lock_count);

    g_atomic_int_set(&section->internal_lock, 0);

    return SC_TRUE;
}

void sc_segment_section_unlock(const sc_memory_context *ctx, sc_segment_section *section)
{
    g_assert(section != null_ptr);

    while (g_atomic_int_compare_and_exchange(&section->internal_lock, 0, 1) == FALSE)
    {
        LOCK_SLEEP();
    }

    g_assert(g_atomic_pointer_get(&section->ctx_lock) == ctx);

    if (g_atomic_int_dec_and_test(&section->lock_count) == TRUE)
        g_atomic_pointer_set(&section->ctx_lock, 0);

    g_atomic_int_set(&section->internal_lock, 0);
}

void sc_segment_lock(sc_segment * seg, sc_memory_context const * ctx)
{
    sc_uint32 i;
    for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
        sc_segment_section_lock(ctx, &seg->sections[i]);
}

void sc_segment_unlock(sc_segment * seg, sc_memory_context const * ctx)
{
    sc_uint32 i;
    for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
        sc_segment_section_unlock(ctx, &seg->sections[i]);
}

