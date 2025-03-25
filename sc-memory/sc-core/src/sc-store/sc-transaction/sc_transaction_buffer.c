#include "sc_transaction_buffer.h"

#include <sc-core/sc-base/sc_allocator.h>
#include <sc-core/sc-container/sc_list.h>

sc_bool sc_transaction_buffer_initialize(sc_transaction_buffer* buffer)
{
  if (buffer == null_ptr)
    return SC_FALSE;

  buffer->new_elements = sc_mem_new(sc_list, 1);
  if (buffer->new_elements == null_ptr)
    return SC_FALSE;
  sc_list_init(&buffer->new_elements);

  buffer->modified_elements = sc_mem_new(sc_list, 1);
  if (buffer->modified_elements == null_ptr)
  {
    sc_mem_free(buffer->new_elements);
    return SC_FALSE;
  }
  sc_list_init(&buffer->modified_elements);

  buffer->deleted_elements = sc_mem_new(sc_list, 1);
  if (buffer->deleted_elements == null_ptr)
  {
    sc_mem_free(buffer->new_elements);
    sc_mem_free(buffer->modified_elements);
    return SC_FALSE;
  }
  sc_list_init(&buffer->deleted_elements);

  return SC_TRUE;
}