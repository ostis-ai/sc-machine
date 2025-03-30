#include "sc_transaction_buffer.h"

#include "sc-store/sc-base/sc_monitor_private.h"

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

sc_bool sc_transaction_buffer_initialize(sc_transaction_buffer * transaction_buffer)
{
  if (transaction_buffer == null_ptr)
    return SC_FALSE;

  if (!sc_list_init(&transaction_buffer->new_elements))
    return SC_FALSE;

  if (!sc_list_init(&transaction_buffer->modified_elements))
  {
    sc_list_destroy(transaction_buffer->new_elements);
    return SC_FALSE;
  }

  if (!sc_list_init(&transaction_buffer->deleted_elements))
  {
    sc_list_destroy(transaction_buffer->new_elements);
    sc_list_destroy(transaction_buffer->modified_elements);
    return SC_FALSE;
  }

  return SC_TRUE;
}


sc_bool sc_transaction_buffer_created_add(const sc_transaction_buffer * buffer, const sc_addr addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(addr))
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  if (transaction_buffer_contains_created(buffer, addr))
    return SC_TRUE;

  if (sc_list_push_back(buffer->new_elements, (void*)(uintptr_t)addr_hash) == null_ptr)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_modified_add(
    const sc_transaction_buffer * buffer,
    const sc_addr addr,
    sc_element * element,
    sc_stream * content,
    const SC_ELEMENT_MODIFIED_FLAGS flags)
{
  if (buffer == null_ptr || buffer->modified_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(addr))
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(addr);

  sc_iterator * it = sc_list_iterator(buffer->modified_elements);
  while (sc_iterator_next(it))
  {
    sc_modified_element * mod = sc_iterator_get(it);
    if (SC_ADDR_LOCAL_TO_INT(mod->addr) == addr_hash)
    {
      sc_element_version * new_version = sc_element_create_new_version(element, mod->version->version_id + 1, flags);
      if (new_version == NULL)
      {
        sc_iterator_destroy(it);
        return SC_FALSE;
      }

      new_version->prev_version = mod->version;
      mod->version->next_version = new_version;

      mod->version = new_version;
      mod->content = content;
      mod->flags |= flags; // merge field edit flags
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  sc_modified_element * new_mod = sc_mem_new(sc_modified_element, 1);
  if (new_mod == null_ptr)
    return SC_FALSE;

  new_mod->addr = addr;
  new_mod->version = sc_element_create_new_version(element, 0, flags);
  if (new_mod->version == NULL)
  {
    sc_mem_free(new_mod);
    return SC_FALSE;
  }

  new_mod->content = content;
  new_mod->flags = flags;

  if (sc_list_push_back(buffer->modified_elements, new_mod) == null_ptr)
  {
    sc_mem_free(new_mod->version);
    sc_mem_free(new_mod);
    return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_removed_add(const sc_transaction_buffer * buffer, const sc_addr addr)
{
  if (buffer == null_ptr || buffer->deleted_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(addr))
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(addr);

  sc_iterator * it = sc_list_iterator(buffer->deleted_elements);
  while (sc_iterator_next(it))
  {
    const sc_uint32 stored_hash = (uintptr_t)sc_iterator_get(it);
    if (stored_hash == addr_hash)
    {
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  if (sc_list_push_back(buffer->deleted_elements, (void*)(uintptr_t)addr_hash) == null_ptr)
    return SC_FALSE;

  return SC_TRUE;
}


sc_bool transaction_buffer_contains_created(sc_transaction_buffer const * buffer, sc_addr addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  sc_iterator * it = sc_list_iterator(buffer->new_elements);
  while (sc_iterator_next(it))
  {
    const sc_uint32 stored_hash = (uintptr_t)sc_iterator_get(it);
    if (stored_hash == addr_hash)
    {
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  return SC_FALSE;
}


