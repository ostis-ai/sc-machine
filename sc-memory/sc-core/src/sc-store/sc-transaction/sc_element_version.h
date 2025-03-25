#ifndef SC_ELEMENT_VERSION_H
#define SC_ELEMENT_VERSION_H

#include "sc-core/sc_types.h"

typedef struct sc_element_version {
  void* data;
  sc_uint64 version_id;
  struct sc_element_version* prev_version;
  struct sc_element_version* next_version;
  sc_bool is_committed;
} sc_element_version;

typedef struct sc_version_chain {
  sc_element_version* head;
  sc_element_version* tail;
} sc_version_chain;

#endif
