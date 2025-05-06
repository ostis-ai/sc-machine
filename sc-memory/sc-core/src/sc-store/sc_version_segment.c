#include "sc_version_segment.h"
#include <stdlib.h>
#include <string.h>
#include <sc-core/sc-base/sc_allocator.h>

sc_version_segment * sc_version_segment_new(void)
{
  sc_version_segment * segment = sc_mem_new(sc_version_segment, 1);
  if (!segment)
    return NULL;
  sc_mem_set(segment, 0, sizeof(*segment));
  sc_monitor_init(&segment->monitor);
  return segment;
}

void sc_version_segment_free(sc_version_segment * segment)
{
  if (!segment)
    return;
  sc_monitor_destroy(&segment->monitor);
  sc_mem_free(segment);
}

sc_element_version * sc_version_segment_add(
    sc_version_segment * segment,
    sc_element const * data,
    sc_uint64 const version_id,
    sc_uint64 const transaction_id,
    sc_element_version * parent)
{
  if (!segment)
    return NULL;

  sc_monitor_acquire_write(&segment->monitor);

  if (segment->count >= SC_VERSION_SEGMENT_SIZE)
  {
    sc_monitor_release_write(&segment->monitor);
    return NULL;
  }

  sc_element_version * ver = &segment->versions[segment->count++];

  ver->data = data;
  ver->version_id = version_id;
  ver->transaction_id = transaction_id;
  ver->parent_version = parent;
  ver->is_committed = SC_FALSE;

  segment->latest_version = ver;

  sc_monitor_release_write(&segment->monitor);
  return ver;
}
