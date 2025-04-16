#ifndef SC_ELEMENT_VERSION_H
#define SC_ELEMENT_VERSION_H
#include "sc-core/sc_types.h"

typedef struct sc_element_version
{
  sc_element const * data;
  sc_uint64 version_id;
  sc_uint64 transaction_id;
  struct sc_element_version * parent_version;
  sc_bool is_committed;
} sc_element_version;

sc_element_version * sc_element_create_new_version(
    sc_element_version * parent,
    sc_element const * new_element_data,
    sc_uint64 version_id,
    sc_uint64 transaction_id);

#endif
