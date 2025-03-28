#include "sc_transaction_buffer.h"

#include <sc-store/sc-transaction/sc_element_version.h>
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc_stream.h>

typedef struct sc_modified_element {
    sc_addr addr;
    sc_element_version * version;
    sc_stream * content;
    SC_ELEMENT_MODIFIED_FLAGS flags;
} sc_modified_element;

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
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(addr))
    return SC_FALSE;

  if (transaction_buffer_contains_created(buffer, addr))
    return SC_TRUE;

  sc_addr * addr_copy = sc_mem_new(sc_addr, 1);
  if (addr_copy == null_ptr)
    return SC_FALSE;

  *addr_copy = addr;

  if (sc_list_push_back(buffer->new_elements, addr_copy) == null_ptr)
  {
    sc_mem_free(addr_copy);
    return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_modified_add(
    sc_transaction_buffer * buffer,
    sc_addr addr,
    sc_element * element,
    sc_stream * content,
    SC_ELEMENT_MODIFIED_FLAGS flags)
{
  if (buffer == null_ptr || buffer->modified_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(addr))
    return SC_FALSE;

  sc_element_version * new_version = sc_element_create_new_version(element, /* version_id */ 0, flags); // Add generation for id
  if (new_version == NULL)
    return SC_FALSE;

  sc_iterator * it = sc_list_iterator(buffer->modified_elements);
  while (sc_iterator_next(it))
  {
    sc_modified_element * mod = (sc_modified_element *) sc_iterator_get(it);
    if (SC_ADDR_IS_EQUAL(mod->addr, addr))
    {
      mod->version = new_version;
      mod->content = content;
      mod->flags = flags;
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  sc_modified_element * new_mod = sc_mem_new(sc_modified_element, 1);
  if (new_mod == null_ptr)
    return SC_FALSE;

  new_mod->addr = addr;
  new_mod->version = new_version;
  new_mod->content = content;
  new_mod->flags = flags;

  if (sc_list_push_back(buffer->modified_elements, new_mod) == null_ptr)
  {
    sc_mem_free(new_version);
    sc_mem_free(new_mod);
    return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_removed_add(sc_transaction_buffer * buffer, sc_addr addr)
{
  if (buffer == null_ptr || buffer->deleted_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(addr))
    return SC_FALSE;

  sc_iterator * it = sc_list_iterator(buffer->deleted_elements);
  while (sc_iterator_next(it))
  {
    sc_addr * stored_addr = (sc_addr *) sc_iterator_get(it);
    if (SC_ADDR_IS_EQUAL(*stored_addr, addr))
    {
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  sc_addr * addr_copy = sc_mem_new(sc_addr, 1);
  if (addr_copy == null_ptr)
    return SC_FALSE;

  *addr_copy = addr;
  if (sc_list_push_back(buffer->deleted_elements, addr_copy) == null_ptr)
  {
    sc_mem_free(addr_copy);
    return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool transaction_buffer_contains_created(sc_transaction_buffer const * buffer, sc_addr addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  sc_iterator * it = sc_list_iterator(buffer->new_elements);
  while (sc_iterator_next(it))
  {
    sc_addr * stored_addr = (sc_addr *) sc_iterator_get(it);
    if (SC_ADDR_IS_EQUAL(*stored_addr, addr))
    {
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  return SC_FALSE;
}

