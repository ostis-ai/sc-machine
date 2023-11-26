/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_segment.h"

#include "sc_element.h"

#include "sc-base/sc_allocator.h"
#include "sc-base/sc_atomic.h"

sc_segment * sc_segment_new(sc_addr_seg num)
{
  sc_segment * segment = sc_mem_new(sc_segment, 1);
  segment->num = num;
  segment->last_engaged_offset = 0;
  segment->last_released_offset = 0;
  sc_monitor_init(&segment->monitor);

  return segment;
}

void sc_segment_free(sc_segment * segment)
{
  sc_monitor_destroy(&segment->monitor);
  sc_mem_free(segment);
}

void sc_segment_collect_elements_stat(sc_segment * seg, sc_stat * stat)
{
  for (sc_addr_offset i = 0; i < seg->last_engaged_offset; ++i)
  {
    sc_element element = seg->elements[i];
    if ((element.flags.access_levels & SC_ACCESS_LVL_ELEMENT_EXIST) == 0)
      continue;

    sc_type type = element.flags.type;
    if (type & sc_type_node)
      stat->node_count++;
    else if (type & sc_type_link)
      stat->link_count++;
    else if (type & sc_type_arc_mask)
      stat->arc_count++;
  }
}
