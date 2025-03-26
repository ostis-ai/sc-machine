#include "sc_transaction_buffer.h"

#include <sc-store/sc-transaction/sc_element_version.h>

#include <sc-core/sc-base/sc_allocator.h>
#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc_stream.h>

sc_bool sc_transaction_buffer_initialize(sc_transaction_buffer * buffer)
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

sc_bool sc_transaction_buffer_created_add(sc_transaction_buffer * buffer, sc_addr addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
  {
    return SC_FALSE;
  }

  if (SC_ADDR_IS_EMPTY(addr))
  {
    return SC_FALSE;
  }

  if (transaction_buffer_contains_created(buffer, addr))
  {
    return SC_TRUE;
  }

  sc_addr * addr_copy = sc_mem_new(sc_addr, 1);
  if (addr_copy == null_ptr)
  {
    return SC_FALSE;
  }

  *addr_copy = addr;

  if (sc_list_push_back(buffer->new_elements, addr_copy) == null_ptr)
  {
    sc_mem_free(addr_copy);
    return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_modified_add(
    sc_transaction_buffer * buf,
    sc_addr addr,
    sc_element_version * version,
    SC_ELEMENT_MODIFIED_FLAGS flags)
{
}

sc_bool transaction_buffer_contains_created(sc_transaction_buffer const * buffer, sc_addr addr)
{
  sc_iterator * it = sc_list_iterator(buffer->new_elements);
  while (sc_iterator_next(it))
  {
    sc_addr * stored_addr = (sc_addr *)sc_iterator_get(it);
    if (SC_ADDR_IS_EQUAL(*stored_addr, addr))
      return SC_TRUE;
  }
  return SC_FALSE;
}

sc_bool sc_transaction_buffer_removed_add(sc_transaction_buffer * buffer, sc_addr addr) {}

sc_bool sc_transaction_buffer_content_change_add(sc_transaction_buffer * buffer, sc_addr addr, sc_stream * content) {}
