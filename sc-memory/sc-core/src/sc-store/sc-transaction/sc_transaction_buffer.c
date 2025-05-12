#include "sc_transaction_buffer.h"

#include "sc-store/sc_element.h"
#include "sc-store/sc_storage_private.h"
#include "sc-store/sc-base/sc_monitor_table.h"
#include "sc-store/sc-base/sc_monitor_table_private.h"
#include "sc-store/sc-container/sc_pair.h"

#include <sc-store/sc-transaction/sc_element_version.h>
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc_stream.h>

typedef struct sc_content_version
{
  sc_addr addr;
  sc_stream * content;
} sc_content_version;

void sc_transaction_buffer_initialize(sc_transaction_buffer * transaction_buffer, sc_uint64 txn_id)
{
  if (transaction_buffer == null_ptr)
    return;

  transaction_buffer->transaction_id = txn_id;

  if (!sc_list_init(&transaction_buffer->new_elements))
    goto error_nothing;

  if (!sc_list_init(&transaction_buffer->modified_elements))
    goto error_new_elements;

  if (!sc_list_init(&transaction_buffer->deleted_elements))
    goto error_modified_elements;

  if (!sc_list_init(&transaction_buffer->content_changes))
    goto error_deleted_elements;

  return;

error_deleted_elements:
  sc_list_destroy(transaction_buffer->deleted_elements);
error_modified_elements:
  sc_list_destroy(transaction_buffer->modified_elements);
error_new_elements:
  sc_list_destroy(transaction_buffer->new_elements);
error_nothing:
  return;
}

void sc_transaction_buffer_destroy(sc_transaction_buffer * transaction_buffer)
{
  if (transaction_buffer == null_ptr)
    return;

  if (transaction_buffer->new_elements != null_ptr)
  {
    sc_list_destroy(transaction_buffer->new_elements);
    transaction_buffer->new_elements = null_ptr;
  }

  if (transaction_buffer->modified_elements != null_ptr)
  {
    sc_list_destroy(transaction_buffer->modified_elements);
    transaction_buffer->modified_elements = null_ptr;
  }

  if (transaction_buffer->deleted_elements != null_ptr)
  {
    sc_list_destroy(transaction_buffer->deleted_elements);
    transaction_buffer->deleted_elements = null_ptr;
  }

  if (transaction_buffer->content_changes != null_ptr)
  {
    sc_iterator * it = sc_list_iterator(transaction_buffer->content_changes);
    while (sc_iterator_next(it))
    {
      sc_pair * pair = sc_iterator_get(it);
      if (pair == null_ptr)
        continue;

      if (pair->second != null_ptr)
      {
        sc_stream_free(pair->second);
        pair->second = null_ptr;
      }

      sc_mem_free(pair);
    }
    sc_iterator_destroy(it);

    sc_list_destroy(transaction_buffer->content_changes);
    transaction_buffer->content_changes = null_ptr;
  }
}

sc_bool sc_transaction_buffer_created_add(sc_transaction_buffer const * buffer, sc_addr const * addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
  if (sc_transaction_buffer_contains_created(buffer, addr))
    return SC_TRUE;

  if (sc_list_push_back(buffer->new_elements, (void *)(uintptr_t)addr_hash) == null_ptr)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_modified_add(
    sc_transaction_buffer const * buffer,
    sc_addr const * addr,
    sc_element_data const * new_element_data)
{
  if (buffer == null_ptr || buffer->modified_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_element * element = null_ptr;
  if (sc_storage_get_element_by_addr(*addr, &element) != SC_RESULT_OK || element == null_ptr)
    return SC_FALSE;

  sc_element_data * snapshot = sc_mem_new(sc_element_data, 1);
  if (snapshot == null_ptr)
    return SC_FALSE;
  *snapshot = *new_element_data;

  sc_pair * pair = sc_make_pair((void *)(uintptr_t)SC_ADDR_LOCAL_TO_INT(*addr), snapshot);
  if (pair == null_ptr)
  {
    sc_mem_free(snapshot);
    return SC_FALSE;
  }

  if (sc_list_push_back(buffer->modified_elements, pair) == null_ptr)
  {
    sc_mem_free(snapshot);
    sc_mem_free(pair);
    return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_removed_add(sc_transaction_buffer const * buffer, sc_addr const * addr)
{
  if (buffer == null_ptr || buffer->deleted_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);

  sc_iterator * it = sc_list_iterator(buffer->deleted_elements);
  while (sc_iterator_next(it))
  {
    sc_uint32 const stored_hash = (uintptr_t)sc_iterator_get(it);
    if (stored_hash == addr_hash)
    {
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  if (sc_list_push_back(buffer->deleted_elements, (void *)(uintptr_t)addr_hash) == null_ptr)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_content_set(
    sc_transaction_buffer const * buffer,
    sc_addr const * addr,
    sc_stream const * content)
{
  if (buffer == null_ptr || buffer->content_changes == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);

  sc_iterator * it = sc_list_iterator(buffer->content_changes);
  while (sc_iterator_next(it))
  {
    sc_pair * pair = sc_iterator_get(it);
    sc_uint32 const stored_hash = (uintptr_t)pair->first;
    if (stored_hash == addr_hash)
    {
      pair->second = (void *)content;
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  sc_pair const * content_pair = sc_make_pair((void *)(uintptr_t)addr_hash, (void *)content);
  if (sc_list_push_back(buffer->content_changes, (void *)content_pair) == null_ptr)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_contains_created(sc_transaction_buffer const * buffer, sc_addr const * addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
  sc_iterator * it = sc_list_iterator(buffer->new_elements);
  while (sc_iterator_next(it))
  {
    sc_uint32 const stored_hash = (uintptr_t)sc_iterator_get(it);
    if (stored_hash == addr_hash)
    {
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  return SC_FALSE;
}
