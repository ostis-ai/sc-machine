/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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
    segment->have_empty_slots = SC_TRUE;
#else
    segment->empty_slot = (num == 0) ? 1 : 0;
#endif
    segment->num = num;

    return segment;
}

void sc_segment_free(sc_segment *segment)
{
    g_assert( segment != 0);

    g_free(segment);
}

sc_bool sc_segment_have_empty_slot(sc_segment *segment)
{
    g_assert( segment != (sc_segment*)0 );
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    return segment->have_empty_slots;
#else
    return segment->empty_slot < SEGMENT_SIZE;
#endif
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
    else
    {
        g_assert (segment->have_empty_slots == SC_TRUE);

        sc_segment_update_empty_slot(segment);
        if (segment->empty_slot_buff_head > 0)
            slot = segment->empty_slot_buff[--segment->empty_slot_buff_head];
    }
#else
    slot = segment->empty_slot;
#endif

    g_assert(slot < SEGMENT_SIZE);

    segment->elements[slot] = *element;
    *offset = slot;

#if USE_SEGMENT_EMPTY_SLOT
    g_print("\n%u", segment->empty_slot_buff_head);
    if (segment->empty_slot_buff_head == 0)
#endif
        sc_segment_update_empty_slot(segment);

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

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    if (segment->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE)
        segment->empty_slot_buff[segment->empty_slot_buff_head++] = el_id;
    segment->have_empty_slots = SC_TRUE;
#else
    segment->empty_slot = el_id;
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
    sc_uint idx = segment->empty_slot_buff[0];
    sc_uint v = 0;

    // forward search
    while ((idx < SEGMENT_SIZE) && (segment->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE))
    {
        if (segment->elements[idx].type == 0)
            segment->empty_slot_buff[segment->empty_slot_buff_head++] = idx;
        idx++;
    }

    if (idx > 0)
    {
        // backward search
        v = (segment->num == 0) ? 1 : G_MAXUINT;
        while ((idx != v) && (segment->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE))
        {
            if (segment->elements[idx].type == 0)
                segment->empty_slot_buff[segment->empty_slot_buff_head++] = idx;
            idx--;
        }
    }

    if (segment->empty_slot_buff_head == 0)
        segment->have_empty_slots = SC_FALSE;
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
    sc_uint32 newest_time_stamp = sc_storage_get_time_stamp();
    sc_element *el = 0;
    sc_addr prev_arc, current_arc;

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    segment->empty_slot_buff_head = 0;
#endif

    for (idx = 0; idx < SEGMENT_SIZE; ++idx)
    {
        el = &(seg->elements[idx]);

        // skip element that wasn't deleted
        if (el->delete_time_stamp > 0)
        {
            // delete arcs from output and intpu lists
            // @todo two oriented lists support
            if (el->type & sc_type_arc_mask)
            {

            }

        }


        // collect empty cells
        if (el->type == 0)
        {
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
            seg->empty_slot_buff[seg->empty_slot_buff_head++] = idx;
#else
            seg->empty_slot = idx;
#endif
        }
    }
}


