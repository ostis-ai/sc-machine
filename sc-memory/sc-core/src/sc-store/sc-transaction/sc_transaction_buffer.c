#include "sc_transaction_buffer.h"

#include "sc-store/sc_element.h"
#include "sc-store/sc_storage_private.h"
#include "sc-store/sc-base/sc_monitor_private.h"
#include "sc-store/sc-base/sc_monitor_table.h"
#include "sc-store/sc-base/sc_monitor_table_private.h"
#include "sc-store/sc-container/sc_pair.h"

#include <sc-store/sc-transaction/sc_element_version.h>
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc_stream.h>

typedef struct sc_content_version {
    sc_addr addr;
    sc_stream * content;
    SC_ELEMENT_MODIFIED_FLAGS SC_ELEMENT_CONTENT_MODIFIED;
} sc_content_version;

sc_bool sc_transaction_buffer_initialize(sc_transaction_buffer * transaction_buffer, sc_uint64 txn_id)
{
  if (transaction_buffer == null_ptr)
    return SC_FALSE;

  transaction_buffer->transaction_id = txn_id;

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

  if (!sc_list_init(&transaction_buffer->content_changes))
  {
    sc_list_destroy(transaction_buffer->new_elements);
    sc_list_destroy(transaction_buffer->modified_elements);
    sc_list_destroy(transaction_buffer->deleted_elements);
    return SC_FALSE;
  }

  transaction_buffer->monitor_table = sc_mem_new(sc_monitor_table, 1);
  if (transaction_buffer->monitor_table == null_ptr)
  {
    sc_list_destroy(transaction_buffer->new_elements);
    sc_list_destroy(transaction_buffer->modified_elements);
    sc_list_destroy(transaction_buffer->deleted_elements);
    sc_list_destroy(transaction_buffer->content_changes);
    return SC_FALSE;
  }
  _sc_monitor_table_init(transaction_buffer->monitor_table);

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_created_add(const sc_transaction_buffer * buffer, const sc_addr* addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
  if (sc_transaction_buffer_contains_created(buffer, addr))
    return SC_TRUE;

  if (sc_list_push_back(buffer->new_elements, (void*)(uintptr_t)addr_hash) == null_ptr)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_modified_add(
    const sc_transaction_buffer *buffer,
    const sc_addr *addr,
    const sc_element *new_element_data,
    const SC_ELEMENT_MODIFIED_FLAGS flags)
{
  if (buffer == NULL || buffer->modified_elements == NULL)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_element *element = NULL;
  if (sc_storage_get_element_by_addr(*addr, &element) != SC_RESULT_OK || element == NULL)
    return SC_FALSE;

  sc_monitor *monitor = sc_monitor_table_get_monitor_for_addr(buffer->monitor_table, *addr);
  if (monitor == NULL)
    return SC_FALSE;

  sc_monitor_acquire_write(monitor);

  const sc_uint64 new_version_id = (element->version_history.latest_version != NULL)
                                      ? (element->version_history.latest_version->version_id + 1)
                                      : 0;

  sc_element_version *new_version = sc_element_create_new_version(
      element->version_history.latest_version, new_element_data, new_version_id, buffer->transaction_id, flags);

  if (new_version == NULL)
  {
    sc_monitor_release_write(monitor);
    return SC_FALSE;
  }

  element->version_history.latest_version = new_version;

  monitor->id++;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
  if (sc_list_push_back(buffer->modified_elements, (void *)(uintptr_t)addr_hash) == NULL)
  {
    sc_monitor_release_write(monitor);
    return SC_FALSE;
  }

  sc_monitor_release_write(monitor);

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_removed_add(const sc_transaction_buffer * buffer, const sc_addr* addr)
{
  if (buffer == null_ptr || buffer->deleted_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);

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

sc_bool sc_transaction_buffer_content_set(const sc_transaction_buffer * buffer, const sc_addr* addr, const sc_stream * content)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);

  sc_iterator * it = sc_list_iterator(buffer->content_changes);
  while (sc_iterator_next(it))
  {
    sc_pair* pair = sc_iterator_get(it);
    const sc_uint32 stored_hash = (uintptr_t)pair->first;
    if (stored_hash == addr_hash)
    {
      pair->second = (void*)content;
      sc_iterator_destroy(it);
      return SC_TRUE;
    }
  }
  sc_iterator_destroy(it);

  const sc_pair* content_pair = sc_make_pair((void*)addr_hash, (void*)content);
  if (sc_list_push_back(buffer->content_changes, &content_pair) == null_ptr)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_transaction_buffer_contains_created(sc_transaction_buffer const * buffer, const sc_addr* addr)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr)
    return SC_FALSE;

  const sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
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


