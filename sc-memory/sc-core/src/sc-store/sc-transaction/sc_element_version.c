#include "sc_element_version.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_element_version *sc_element_create_new_version(
    sc_element_version *parent,
    const sc_element *new_element_data,
    const sc_uint64 version_id,
    const sc_uint64 transaction_id,
    const SC_ELEMENT_MODIFIED_FLAGS modified_fields)
{
  if (new_element_data == NULL)
    return NULL;

  sc_element_version *new_version = sc_mem_new(sc_element_version, 1);
  if (new_version == NULL)
    return NULL;

  new_version->data = new_element_data;
  new_version->version_id = version_id;
  new_version->transaction_id = transaction_id;
  new_version->parent_version = parent;
  new_version->is_committed = SC_FALSE;
  new_version->modified_fields = modified_fields;

  return new_version;
}
