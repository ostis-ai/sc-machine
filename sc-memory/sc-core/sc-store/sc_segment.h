/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_segment_h_
#define _sc_segment_h_

#include "sc_types.h"
#include "sc_defines.h"
#include "sc_element.h"

#include <glib.h>

#define SC_SEG_ELEMENTS_SIZE_BYTE (sizeof(sc_element) * SC_SEGMENT_ELEMENTS_COUNT)

/*! Structure for segment storing
 */
struct _sc_segment
{
  sc_element elements[SC_SEGMENT_ELEMENTS_COUNT];
  sc_addr_seg num;  // number of this segment in memory
  sc_uint32 elements_count;  // number of sc-element in the segment
};

/*! Create new segment with specified size.
 * @param num Number of created instance in sc-memory
 */
sc_segment * sc_segment_new(sc_addr_seg num);

void sc_segment_free(sc_segment * segment);

//! Collects segment elements statistics
void sc_segment_collect_elements_stat(sc_segment * seg, sc_stat * stat);

// ---------------------- locks --------------------------
/*! Function to lock specified element in segment
 * @param seg Pointer to segment to lock element
 * @param offset Offset of element to lock
 * @returns Returns pointer to locked sc-element
 */
sc_element * sc_segment_lock_element(sc_segment * seg, sc_addr_offset offset);

//! Try to lock sc-element with maximum attempts
sc_element * sc_segment_lock_element_try(sc_segment * seg, sc_addr_offset offset, sc_uint16 max_attempts);

/*! Function to unlock specified element in segment
 * @param seg Pointer to segment for element unlocking
 * @param offset Offset of sc-element in segment
 */
void sc_segment_unlock_element(sc_segment * seg, sc_addr_offset offset);

#endif
