#include "sc_transaction.h"

#include "sc_memory_transaction_manager.h"
#include "sc-store/sc_element.h"
#include "sc-store/sc_storage_private.h"
#include "sc-store/sc-container/sc_pair.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_transaction * sc_transaction_new(sc_uint64 const txn_id, sc_memory_context * ctx)
{
  sc_transaction * txn = _sc_mem_new(sizeof(sc_transaction));
  if (txn == null_ptr)
    return null_ptr;

  if (ctx == null_ptr)
  {
    sc_mem_free(txn);
    return null_ptr;
  }

  txn->transaction_id = txn_id;
  txn->is_committed = SC_FALSE;
  txn->ctx = ctx;

  txn->monitor = _sc_mem_new(sizeof(sc_monitor));
  if (txn->monitor == null_ptr)
  {
    sc_mem_free(txn);
    return null_ptr;
  }
  sc_monitor_init(txn->monitor);

  txn->transaction_buffer = _sc_mem_new(sizeof(sc_transaction_buffer));
  if (txn->transaction_buffer == null_ptr)
  {
    sc_monitor_destroy(txn->monitor);
    sc_mem_free(txn->monitor);
    sc_mem_free(txn);
    return null_ptr;
  }
  sc_transaction_buffer_initialize(txn->transaction_buffer, txn_id);

  txn->elements = sc_hash_table_init(
      sc_hash_table_default_hash_func,
      sc_hash_table_default_equal_func,
      null_ptr,
      null_ptr
  );
  if (txn->elements == null_ptr)
  {
    sc_transaction_buffer_destroy(txn->transaction_buffer);
    sc_mem_free(txn->transaction_buffer);
    sc_monitor_destroy(txn->monitor);
    sc_mem_free(txn->monitor);
    sc_mem_free(txn);
    return null_ptr;
  }

  return txn;
}

void sc_transaction_destroy(sc_transaction * txn)
{
  if (txn != null_ptr)
  {
    if (txn->transaction_buffer != null_ptr)
    {
      sc_transaction_buffer_destroy(txn->transaction_buffer);
      sc_mem_free(txn->transaction_buffer);
    }
    if (txn->elements != null_ptr)
    {
      sc_hash_table_destroy(txn->elements);
    }
    if (txn->monitor != null_ptr)
    {
      sc_monitor_destroy(txn->monitor);
      sc_mem_free(txn->monitor);
    }
    sc_mem_free(txn);
  }
}

void sc_transaction_add_element(sc_transaction * txn, sc_element * element) {}

void sc_transaction_commit(sc_transaction * txn)
{
  sc_transaction_manager_transaction_add(txn);
}

void sc_transaction_rollback(sc_transaction * txn) {}

sc_bool _sc_transaction_validate_modify_elements(sc_list const * elements_list)
{
  sc_iterator * it = sc_list_iterator(elements_list);
  if (sc_iterator_next(it))
  {
    sc_pair const * pair = sc_iterator_get(it);

    sc_addr const * element_addr = pair->first;
    sc_element_data const * snapshot = pair->second;

    sc_element * element = null_ptr;
    sc_storage_get_element_by_addr(*element_addr, &element);

    sc_element_data * current_data = null_ptr;
    sc_storage_get_element_data_by_addr(*element_addr, current_data);

    if (current_data != snapshot)
    {
      return SC_FALSE;
    }
  }

  return SC_TRUE;
}

sc_bool sc_transaction_validate(sc_transaction * txn)
{
  if (!_sc_transaction_validate_modify_elements(txn->transaction_buffer->modified_elements)) return SC_FALSE;

  return SC_TRUE;
}

void sc_transaction_merge(sc_transaction * txn) {}

void sc_transaction_apply(sc_transaction * txn) {}

void sc_transaction_clear(sc_transaction * txn) {}

sc_bool sc_transaction_element_new(sc_transaction const * txn, sc_addr const * addr)
{
  if (txn == null_ptr || addr == null_ptr || txn->transaction_buffer == null_ptr)
    return SC_FALSE;

  sc_hash_table_insert(txn->elements, (void*)addr, null_ptr);

  return sc_transaction_buffer_created_add(txn->transaction_buffer, addr);
}

sc_bool sc_transaction_element_change(sc_transaction const * txn, sc_addr const * addr, sc_element_data const * new_data)
{
  if (txn == null_ptr || addr == null_ptr || txn->transaction_buffer == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_hash_table_insert(txn->elements, (void*)addr, null_ptr);

  return sc_transaction_buffer_modified_add(txn->transaction_buffer, addr, new_data);
}

sc_bool sc_transaction_element_remove(sc_transaction const * txn, sc_addr const * addr)
{
  if (txn == null_ptr || addr == null_ptr || txn->transaction_buffer == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_hash_table_insert(txn->elements, (void*)addr, null_ptr);

  return sc_transaction_buffer_removed_add(txn->transaction_buffer, addr);
}

sc_bool sc_transaction_element_content_set(sc_transaction const * txn, sc_addr const * addr, sc_stream const * content)
{
  if (txn == null_ptr || addr == null_ptr || content == null_ptr || txn->transaction_buffer == null_ptr)
    return SC_FALSE;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_FALSE;

  sc_hash_table_insert(txn->elements, (void*)addr, null_ptr);

  return sc_transaction_buffer_content_set(txn->transaction_buffer, addr, content);
}