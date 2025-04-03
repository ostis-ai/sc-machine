#ifndef SC_ELEMENT_VERSION_H
#define SC_ELEMENT_VERSION_H

#include <sc-core/sc_types.h>

typedef enum
{
  SC_ELEMENT_FLAGS_MODIFIED = 1 << 0,
  SC_ELEMENT_ARCS_MODIFIED = 1 << 1,
  SC_ELEMENT_CONTENT_MODIFIED = 1 << 2,
  SC_ELEMENT_LINKS_MODIFIED = 1 << 3
} SC_ELEMENT_MODIFIED_FLAGS;

typedef struct sc_element_version
{
  const sc_element* data;
  sc_uint64 version_id;
  sc_uint64 transaction_id;
  struct sc_element_version *parent_version;
  sc_bool is_committed;
  SC_ELEMENT_MODIFIED_FLAGS modified_fields;
} sc_element_version;

typedef struct sc_version_history
{
  sc_element_version *latest_version;
} sc_version_history;

sc_element_version * sc_element_create_new_version(
  sc_element_version *parent,
  const sc_element *new_element_data,
  sc_uint64 version_id,
  sc_uint64 transaction_id,
  SC_ELEMENT_MODIFIED_FLAGS modified_fields);

#endif
