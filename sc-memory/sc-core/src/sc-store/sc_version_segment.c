#include "sc_version_segment.h"
#include <stdlib.h>
#include <string.h>
#include <sc-core/sc-base/sc_allocator.h>

sc_version_segment * sc_version_segment_new(void)
{
  sc_version_segment * segment = sc_mem_new(sc_version_segment, 1);
  if (!segment)
    return null_ptr;
  sc_mem_set(segment, 0, sizeof(*segment));
  segment->monitor = _sc_mem_new(sizeof(sc_monitor));
  if (segment->monitor == null_ptr)
  {
    sc_mem_free(segment);
    return null_ptr;
  }
  sc_monitor_init(segment->monitor);
  return segment;
}

void sc_version_segment_free(sc_version_segment * segment)
{
  if (!segment)
    return;
  sc_monitor_destroy(segment->monitor);
  sc_mem_free(segment);
}

sc_uint64 sc_version_segment_get_next_version_id(sc_element const * element)
{
  sc_version_segment const * segment = element->version_history;
  sc_monitor * monitor = segment->monitor;

  sc_monitor_acquire_read(monitor);
  sc_uint64 const next_version_id = segment->count;
  sc_monitor_release_read(monitor);

  return next_version_id;
}

void sc_version_segment_add(sc_version_segment * segment, sc_element_version * new_version)
{
  if (segment == null_ptr || new_version == null_ptr)
    return;

  sc_monitor_acquire_write(segment->monitor);

  if (segment->count >= SC_VERSION_SEGMENT_SIZE)
  {
    sc_monitor_release_write(segment->monitor);
    return;
  }

  segment->versions[segment->count++] = *new_version->data;
  segment->current_version = new_version;

  sc_monitor_release_write(segment->monitor);
}
