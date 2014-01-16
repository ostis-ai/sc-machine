/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "sc_segment.h"
#include "sc_element.h"
#include "sc_storage.h"

#include <glib.h>

sc_segment* sc_segment_new(sc_addr_seg num)
{
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    sc_uint idx;
#endif

    sc_segment *segment = g_new0(sc_segment, 1);
    //  segment->type = type;

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    for (idx = (num == 0) ? 1 : 0; idx < SEGMENT_EMPTY_BUFFER_SIZE; idx++)
        segment->empty_slot_buff[idx] = idx;
    segment->empty_slot_buff_head = SEGMENT_EMPTY_BUFFER_SIZE;
    segment->has_empty_slots = SC_TRUE;
#else
    segment->empty_slot = (num == 0) ? 1 : 0;
#endif
    segment->num = num;

#if SC_INTERNAL_THREADS_SUPPORT
    g_rw_lock_init(&segment->rw_lock);
#endif

    return segment;
}

void sc_segment_free(sc_segment *segment)
{
#if SC_INTERNAL_THREADS_SUPPORT
    g_rw_lock_clear(&segment->rw_lock);
#endif

    g_assert( segment != 0);

    g_free(segment);
}

sc_element* sc_segment_append_element(sc_segment *segment,
                                      sc_element *element,
                                      sc_uint16 *offset)
{
    sc_uint slot = SEGMENT_SIZE;
    g_assert( segment != 0 );
    g_assert( element != 0 );

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    if (segment->empty_slot_buff_head > 0)
        slot = segment->empty_slot_buff[--segment->empty_slot_buff_head];
    if (segment->empty_slot_buff_head == 0)
        sc_segment_update_empty_slot(segment);
#else
    slot = segment->empty_slot;
    sc_segment_update_empty_slot(segment);
#endif

    g_assert(slot < SEGMENT_SIZE);

    segment->elements[slot] = *element;
    *offset = slot;

#if USE_SEGMENT_EMPTY_SLOT
    g_print("\n%u", segment->empty_slot_buff_head);
    if (segment->empty_slot_buff_head == 0)
        sc_segment_update_empty_slot(segment);
#endif

    return &(segment->elements[slot]);
}

sc_element* sc_segment_get_element(sc_segment *seg, sc_uint id)
{
    g_assert(id < SEGMENT_SIZE && seg != 0);
    return &(seg->elements[id]);
}

void sc_segment_remove_element(sc_segment *segment,
                               sc_uint el_id)
{
    g_assert( segment != (sc_segment*)0 );
    g_assert( el_id < SEGMENT_SIZE );
    segment->elements[el_id].type = 0;
}


void sc_segment_write_lock(sc_segment *segment)
{
    g_assert(segment != nullptr);
#if SC_INTERNAL_THREADS_SUPPORT
    g_rw_lock_writer_lock(&segment->rw_lock);
#endif
}

void sc_segment_write_unlock(sc_segment *segment)
{
    g_assert(segment != nullptr);
#if SC_INTERNAL_THREADS_SUPPORT
    g_rw_lock_writer_unlock(&segment->rw_lock);
#endif
}

void sc_segment_read_lock(sc_segment *segment)
{
    g_assert(segment != nullptr);
#if SC_INTERNAL_THREADS_SUPPORT
    g_rw_lock_reader_lock(&segment->rw_lock);
#endif

}

void sc_segment_read_unlock(sc_segment *segment)
{
    g_assert(segment != nullptr);
#if SC_INTERNAL_THREADS_SUPPORT
    g_rw_lock_reader_unlock(&segment->rw_lock);
#endif
}


void sc_segment_update_empty_slot(sc_segment *segment)
{
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    sc_segment_update_empty_slot_buffer(segment);
#else
    sc_segment_update_empty_slot_value(segment);
#endif
}

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
void sc_segment_update_empty_slot_buffer(sc_segment *segment)
{
    sc_uint idx = 0;
    sc_uint v = 0;

    segment->empty_slot_buff_head = 0;
    // forward search
    while ((idx < SEGMENT_SIZE) && (segment->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE))
    {
        if (segment->elements[idx].type == 0)
            segment->empty_slot_buff[segment->empty_slot_buff_head++] = idx;
        g_assert(idx < SEGMENT_SIZE);
        idx++;
    }
}

#else

void sc_segment_update_empty_slot_value(sc_segment *segment)
{
    sc_uint idx;
    sc_uint v;

#if BOUND_EMPTY_SLOT_SEARCH
    sc_uint len;
#endif

    g_assert( segment != 0);
    //segment->empty_slot = SEGMENT_SIZE;
    // search forward
#if BOUND_EMPTY_SLOT_SEARCH
    len = 0;
    for (idx = segment->empty_slot + 1; (idx < SEGMENT_SIZE) && (len < SEGMENT_EMPTY_SEARCH_LEN); ++idx, ++len)
#else
    for (idx = segment->empty_slot + 1; idx < SEGMENT_SIZE; ++idx)
#endif
        if (segment->elements[idx].type == 0)
        {
            segment->empty_slot = idx;
            return;
        }

    // search back
    v = (segment->num == 0) ? 1 : G_MAXUINT;
#if BOUND_EMPTY_SLOT_SEARCH
    len = 0;
    for (idx = v; (idx < segment->empty_slot) && (len < SEGMENT_EMPTY_SEARCH_LEN); ++idx, ++len)
#else
    for (idx = v; idx < segment->empty_slot; ++idx)
#endif
        if (segment->elements[idx].type == 0)
        {
            segment->empty_slot = idx;
            return;
        }

    segment->empty_slot = SEGMENT_SIZE;
}
#endif // USE_SEGMENT_EMPTY_SLOT_BUFFER

sc_uint32 sc_segment_get_elements_count(sc_segment *seg)
{
    sc_uint32 count = 0;
    sc_uint32 idx = 0;

    for (idx = 0; idx < SEGMENT_SIZE; ++idx)
    {
        if (seg->elements[idx].type != 0)
            count++;
    }

    return count;
}

sc_uint32 sc_segment_free_garbage(sc_segment *seg, sc_uint32 oldest_time_stamp)
{
    sc_uint32 free_count = 0;
    sc_uint32 idx = 0;
    //sc_uint32 newest_time_stamp = sc_storage_get_time_stamp();
#if USE_TWO_ORIENTED_ARC_LIST
    sc_element *el = 0, *el2 = 0, *el_arc = 0, *next_el_arc = 0, *prev_el_arc = 0, *b_el = 0, *e_el = 0;
    sc_addr prev_arc, next_arc;
    sc_addr self_addr;
#else
    sc_element *el = 0, *el2 = 0, *el_arc = 0, *prev_el_arc = 0;
    sc_addr prev_arc, current_arc;
    sc_addr self_addr;
#endif

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    seg->empty_slot_buff_head = 0;
#endif
    self_addr.seg = seg->num;

    for (idx = 0; idx < SEGMENT_SIZE; ++idx)
    {
        el = &(seg->elements[idx]);
        self_addr.offset = idx;

        // skip element that wasn't deleted
        if (el->delete_time_stamp <= oldest_time_stamp && el->delete_time_stamp != 0)
        {
            // delete arcs from output and intpu lists
            // @todo two oriented lists support
            if (el->type & sc_type_arc_mask)
            {
#if USE_TWO_ORIENTED_ARC_LIST
                prev_arc = el->arc.prev_out_arc;
                next_arc = el->arc.next_out_arc;

                if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
                {
                    prev_el_arc = sc_storage_get_element(prev_arc, SC_TRUE);
                    prev_el_arc->arc.next_out_arc = next_arc;
                }

                if (SC_ADDR_IS_NOT_EMPTY(next_arc))
                {
                    next_el_arc = sc_storage_get_element(next_arc, SC_TRUE);
                    next_el_arc->arc.prev_out_arc = prev_arc;
                }

                b_el = sc_storage_get_element(el->arc.begin, SC_TRUE);
                if (SC_ADDR_IS_EQUAL(self_addr, b_el->first_out_arc))
                    b_el->first_out_arc = next_arc;

                prev_arc = el->arc.prev_in_arc;
                next_arc = el->arc.next_in_arc;

                if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
                {
                    prev_el_arc = sc_storage_get_element(prev_arc, SC_TRUE);
                    prev_el_arc->arc.next_in_arc = next_arc;
                }

                if (SC_ADDR_IS_NOT_EMPTY(next_arc))
                {
                    next_el_arc = sc_storage_get_element(next_arc, SC_TRUE);
                    next_el_arc->arc.prev_in_arc = prev_arc;
                }

                e_el = sc_storage_get_element(el->arc.end, SC_TRUE);
                if (SC_ADDR_IS_EQUAL(self_addr, b_el->first_in_arc))
                    e_el->first_in_arc = next_arc;
#else
                SC_ADDR_MAKE_EMPTY(prev_arc);
                // output list
                el2 = sc_storage_get_element(el->arc.begin, SC_TRUE);
                current_arc = el2->first_out_arc;
                while (SC_ADDR_IS_NOT_EMPTY(current_arc) && SC_ADDR_IS_NOT_EQUAL(self_addr, current_arc))
                {
                    prev_arc = current_arc;
                    prev_el_arc = el_arc;
                    el_arc = sc_storage_get_element(current_arc, SC_TRUE);
                    current_arc = el->arc.next_out_arc;
                }

                if (SC_ADDR_IS_NOT_EMPTY(prev_arc) && SC_ADDR_IS_NOT_EMPTY(current_arc))
                    prev_el_arc->arc.next_out_arc = el_arc->arc.next_out_arc;

                prev_el_arc = 0;
                el_arc = 0;
                SC_ADDR_MAKE_EMPTY(prev_arc);

                // input list
                el2 = sc_storage_get_element(el->arc.end, SC_TRUE);
                current_arc = el2->first_in_arc;
                while (SC_ADDR_IS_NOT_EMPTY(current_arc) && SC_ADDR_IS_NOT_EQUAL(self_addr, current_arc))
                {
                    prev_arc = current_arc;
                    prev_el_arc = el_arc;

                    el_arc = sc_storage_get_element(current_arc, SC_TRUE);
                    current_arc = el->arc.next_in_arc;
                }

                if (SC_ADDR_IS_NOT_EMPTY(prev_arc) && SC_ADDR_IS_NOT_EMPTY(current_arc))
                    prev_el_arc->arc.next_in_arc = el_arc->arc.next_in_arc;
#endif
            }

            free_count ++;
        }


        // collect empty cells
        if (el->type == 0 && !(idx == 0 && seg->num == 0))
        {
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
            if (seg->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE)
                seg->empty_slot_buff[seg->empty_slot_buff_head++] = idx;
#else
            seg->empty_slot = idx;
#endif
        }
    }

    return free_count;
}

sc_bool sc_segment_has_empty_slot(sc_segment *segment)
{
    g_assert(segment != nullptr);

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    return segment->empty_slot_buff_head > 0 ? SC_TRUE : SC_FALSE;
#else
    return segment->empty_slot < SEGMENT_SIZE ? SC_TRUE : SC_FALSE;
#endif
}


