#include "sc_element_version.h"

#include <sc-store/sc_element.h>
#include <sc-core/sc-base/sc_allocator.h>

sc_element_version * sc_element_create_new_version(sc_element * element, sc_uint64 version_id, sc_type modified_fields)
{
  if (element == NULL)
    return NULL;

  sc_element_version * new_version = sc_mem_new(sc_element_version, 1);
  if (new_version == NULL)
    return NULL;

  new_version->data =  element;
  new_version->version_id = version_id;
  new_version->prev_version = element->versions.tail;
  new_version->next_version = NULL;
  new_version->is_committed = SC_FALSE;
  new_version->modified_fields = modified_fields;

  if (element->versions.tail)
    element->versions.tail->next_version = new_version;
  else
    element->versions.head = new_version;

  element->versions.tail = new_version;

  return new_version;
}
