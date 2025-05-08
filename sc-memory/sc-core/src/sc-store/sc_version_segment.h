#ifndef SC_VERSION_SEGMENT_H
#define SC_VERSION_SEGMENT_H

#include "sc-base/sc_monitor_private.h"
#include "sc-transaction/sc_element_version.h"

#include <sc-core/sc-base/sc_monitor.h>

#define SC_VERSION_SEGMENT_SIZE 512

typedef struct sc_version_segment
{
  sc_element_version versions[SC_VERSION_SEGMENT_SIZE];
  sc_addr_offset count;
  sc_monitor monitor;
  sc_element_version * latest_version;
} sc_version_segment;

sc_version_segment * sc_version_segment_new(void);

void sc_version_segment_free(sc_version_segment * segment);

sc_element_version * sc_version_segment_add(
    sc_version_segment * segment,
    sc_element const * data,
    sc_uint64 version_id,
    sc_uint64 transaction_id,
    sc_element_version * parent);

#endif  // SC_VERSION_SEGMENT_H
