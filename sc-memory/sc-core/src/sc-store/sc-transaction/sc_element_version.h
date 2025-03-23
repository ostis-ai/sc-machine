#ifndef SC_ELEMENT_VERSION_H
#define SC_ELEMENT_VERSION_H

#include <stdbool.h>
#include <stdint.h>

typedef struct sc_element_version {
  void* data;
  uint64_t version_id;
  struct sc_element_version* prev_version;
  struct sc_element_version* next_version;
  bool is_committed;
} sc_element_version;

#endif
