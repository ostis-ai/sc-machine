#ifndef SC_ELEMENT_VERSION_H
#define SC_ELEMENT_VERSION_H

#include "sc-core/sc_types.h"
#include "sc-store/sc_element.h"

typedef struct sc_element_data
{
  sc_element_flags flags;

  sc_addr first_out_arc;
  sc_addr first_in_arc;
#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
  sc_addr first_in_arc_from_structure;
#endif

  sc_arc_info arc;

  sc_uint32 incoming_arcs_count;
  sc_uint32 outgoing_arcs_count;
} sc_element_data;

typedef struct sc_element_version
{
  sc_element_data const * data;
  sc_uint64 version_id;
  sc_uint64 transaction_id;
  struct sc_element_version * parent_version;
  sc_bool is_committed;  // redundant?
} sc_element_version;

sc_element_version * sc_element_create_new_version(
    sc_element_version * parent,
    sc_element_data const * new_element_data,
    sc_uint64 version_id,
    sc_uint64 transaction_id);

#endif
