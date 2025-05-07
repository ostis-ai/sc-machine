#include "sc_element_version.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_element_version * sc_element_create_new_version(
    sc_element_version * parent,
    sc_element_data const * new_element_data,
    sc_uint64 const version_id,
    sc_uint64 const transaction_id)
{
  if (new_element_data == null_ptr)
    return null_ptr;

  sc_element_version * new_version = sc_mem_new(sc_element_version, 1);
  if (new_version == null_ptr)
    return null_ptr;

  new_version->data = new_element_data;
  new_version->version_id = version_id;
  new_version->transaction_id = transaction_id;
  new_version->parent_version = parent;
  new_version->is_committed = SC_FALSE;

  return new_version;
}
