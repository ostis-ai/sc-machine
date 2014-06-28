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

#ifndef _sc_segment_h_
#define _sc_segment_h_

#include "sc_types.h"
#include "sc_defines.h"
#include "sc_element.h"

#include <glib.h>

/*! Structure for segment storing
 */
//typedef struct _sc_array sc_array;
struct _sc_segment
{
    sc_element elements[SEGMENT_SIZE];
    sc_bool has_empty_slots;
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
    sc_uint empty_slot_buff[SEGMENT_EMPTY_BUFFER_SIZE]; // works like a stack
    sc_uint empty_slot_buff_head;
#else
    sc_uint empty_slot; // index empty slot in segment
#endif
    sc_addr_seg num; // number of this segment in memory

#ifdef G_ATOMIC_LOCK_FREE
    sc_uint32 locks[SC_CONCURRENCY_LEVEL];
#else
    GMutex locks[SC_CONCURRENCY_LEVEL];
#endif

};

/*! Create new segment with specified size.
 * @param num Number of created intance in sc-memory
 */
sc_segment* sc_segment_new(sc_addr_seg num);

void sc_segment_free(sc_segment *segment);


/*! Append element into segment at first empty position.
 * @param segment Pointer to segment, that will be contains element
 * @param element Pointer to sc-element data (will be just copied)
 * @param offset Pointer that used to return offset in segment for appended element
 * @return Return pointer to created sc-element data. If element wasn't append into segment, then return 0.
 */
sc_element* sc_segment_append_element(sc_segment *segment,
                                      sc_element *element,
                                      sc_uint16 *offset);

/*! Get sc-element pointer by id
 * @param seg Pointer to segment where we need to get element
 * @param id sc-element id in segment
 * @return Pointer to sc-element with specified id
 */
sc_element* sc_segment_get_element(sc_segment *seg, sc_uint id);

/*! Remove element from specified segment
 */
void sc_segment_remove_element(sc_segment *segment,
                               sc_uint el_id);

//! Returns number of stored sc-elements in segment
sc_uint32 sc_segment_get_elements_count(sc_segment *seg);

/*! Deletes garbage in specified segment
 * @param oldet_time_stamp Oldest timestamp, that can be used
 * @param seg Poitnet to segment to delete garbage
 * @returns Returns number of freed cells
 */
sc_uint32 sc_segment_free_garbage(sc_segment *seg, sc_uint32 oldest_time_stamp);

/*! Check if segment has any empty slots
 * @param segment Pointer to segment for check
 * @returns If \p segment has any empty slots, then return SC_TRUE; otherwise return SC_FALSE
 */
sc_bool sc_segment_has_empty_slot(sc_segment *segment);


//! Update information in segment about first empty slot
void sc_segment_update_empty_slot(sc_segment *segment);

// ---------------------- locks --------------------------
/*! Function to lock specified element in segment
 * @param seg Pointer to segment to lock element
 * @param offset Offset of element to lock
 * @param lock_write Flag to lock element for write. It it has true value, then trying to lock for writing;
 * otherwise locking for reading
 */
void sc_segment_lock_element(sc_segment *seg, sc_uint16 offset, sc_bool lock_write);

/*! Function to unlock specified element in segment
 * @param seg Pointer to segment for element unlocking
 * @param offset Offset of sc-element in segment
 */
void sc_segment_unlock_element(sc_segment *seg, sc_uint16 offset);

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
void sc_segment_update_empty_slot_buffer(sc_segment *segment);
#else
void sc_segment_update_empty_slot_value(sc_segment *segment);
#endif


#endif
