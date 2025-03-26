#ifndef SC_ELEMENT_VERSION_H
#define SC_ELEMENT_VERSION_H

#include <sc-core/sc_types.h>

typedef struct sc_element_version
{
  sc_element * data;
  sc_uint64 version_id;
  struct sc_element_version * prev_version;
  struct sc_element_version * next_version;
  sc_bool is_committed;
  sc_type modified_fields;
} sc_element_version;

typedef struct sc_version_chain
{
  sc_element_version * head;
  sc_element_version * tail;
} sc_version_chain;

typedef enum
{
  SC_ELEMENT_FLAGS_MODIFIED = 1 << 0,
  SC_ELEMENT_ARCS_MODIFIED = 1 << 1,
  SC_ELEMENT_CONTENT_MODIFIED = 1 << 2,
  SC_ELEMENT_LINKS_MODIFIED = 1 << 3
} SC_ELEMENT_MODIFIED_FLAGS;

#endif
