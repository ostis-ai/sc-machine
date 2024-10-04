/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_segment_h_
#define _sc_segment_h_

#include "sc-core/sc-container/sc_list.h"
#include "sc-core/sc-base/sc_monitor.h"

#include "sc-core/sc_defines.h"
#include "sc-core/sc_types.h"

#include "sc_element.h"

#define SC_SEG_ELEMENTS_SIZE_BYTE (sizeof(sc_element) * SC_SEGMENT_ELEMENTS_COUNT)

/*! Structure for segment storing
 */
struct _sc_segment
{
  sc_element elements[SC_SEGMENT_ELEMENTS_COUNT];
  sc_addr_seg num;                     // number of this segment in memory
  sc_addr_offset last_engaged_offset;  // number of sc-element in the segment
  sc_addr_offset last_released_offset;
  sc_monitor monitor;
};

/*! Create new segment with specified size.
 * @param num Number of created instance in sc-memory
 */
sc_segment * sc_segment_new(sc_addr_seg num);

void sc_segment_free(sc_segment * segment);

//! Collects segment elements statistics
void sc_segment_collect_elements_stat(sc_segment * seg, sc_stat * stat);

#endif
