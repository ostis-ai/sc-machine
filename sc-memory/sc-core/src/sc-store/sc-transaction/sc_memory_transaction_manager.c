#include "sc_memory_transaction_manager.h"

#include "sc-store/sc_storage.h"
#include "sc-store/sc-base/sc_condition_private.h"
#include "sc-store/sc-base/sc_monitor_table.h"
#include "sc-store/sc-base/sc_monitor_table_private.h"
#include "sc-store/sc-base/sc_thread.h"
#include "sc-store/sc-container/sc_struct_node.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_memory_transaction_manager * transaction_manager = null_ptr;

sc_result sc_memory_transaction_manager_initialize(sc_memory_transaction_manager * manager)
{
  if (sc_memory_transaction_manager_is_initialized())
  {
    return SC_RESULT_OK;
  }

  transaction_manager = manager;

  if (transaction_manager == null_ptr)
  {
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  transaction_manager->should_stop = SC_FALSE;
  transaction_manager->threads = null_ptr;
  transaction_manager->txn_count = 0;

  transaction_manager->transaction_queue = sc_mem_new(sc_queue, 1);
  if (transaction_manager->transaction_queue == null_ptr)
  {
    return SC_RESULT_ERROR_FULL_MEMORY;
  }
  sc_queue_init(transaction_manager->transaction_queue);

  transaction_manager->monitor = sc_mem_new(sc_monitor, 1);
  if (transaction_manager->monitor == null_ptr)
  {
    goto error_cleanup_queue;
  }
  sc_monitor_init(transaction_manager->monitor);

  transaction_manager->queue_condition = sc_mem_new(sc_condition, 1);
  if (transaction_manager->queue_condition == null_ptr)
  {
    goto error_cleanup_monitor;
  }
  sc_cond_init(transaction_manager->queue_condition);

  transaction_manager->mutex = sc_mem_new(sc_mutex, 1);
  if (transaction_manager->mutex == null_ptr)
  {
    goto error_cleanup_condition;
  }
  sc_mutex_init(transaction_manager->mutex);

  transaction_manager->monitor_table = sc_mem_new(sc_monitor_table, 1);
  if (transaction_manager->monitor_table == null_ptr)
  {
    goto error_cleanup_mutex;
  }
  _sc_monitor_table_init(transaction_manager->monitor_table);

  transaction_manager->processed_elements = sc_hash_table_init(
      sc_hash_table_default_hash_func, sc_hash_table_default_equal_func, null_ptr, null_ptr);
  if (transaction_manager->processed_elements == null_ptr)
  {
    goto error_cleanup_monitor_table;
  }

  if (sc_list_init(&transaction_manager->threads) != SC_RESULT_OK)
  {
    goto error_cleanup_hash_table;
  }

  for (int i = 0; i < SC_TRANSACTION_THREAD_COUNT; ++i)
  {
    sc_thread * thread = sc_thread_new("sc_transaction_handler", sc_transaction_handler, 0);
    if (thread == null_ptr)
    {
      goto error_cleanup_threads;
    }
    sc_list_push_back(transaction_manager->threads, thread);
  }

  return SC_RESULT_OK;

error_cleanup_threads:
  if (transaction_manager->threads != null_ptr)
  {
    for (sc_struct_node const * it = transaction_manager->threads->begin; it != null_ptr; it = it->next)
    {
      sc_thread_unref(it->data);
    }
    sc_list_destroy(transaction_manager->threads);
    transaction_manager->threads = null_ptr;
  }

error_cleanup_hash_table:
  sc_hash_table_destroy(transaction_manager->processed_elements);
  transaction_manager->processed_elements = null_ptr;

error_cleanup_monitor_table:
  sc_mem_free(transaction_manager->monitor_table);
  transaction_manager->monitor_table = null_ptr;

error_cleanup_mutex:
  sc_mutex_destroy(transaction_manager->mutex);
  sc_mem_free(transaction_manager->mutex);

error_cleanup_condition:
  sc_cond_destroy(transaction_manager->queue_condition);
  sc_mem_free(transaction_manager->queue_condition);

error_cleanup_monitor:
  sc_monitor_destroy(transaction_manager->monitor);
  sc_mem_free(transaction_manager->monitor);

error_cleanup_queue:
  sc_queue_destroy(transaction_manager->transaction_queue);
  sc_mem_free(transaction_manager->transaction_queue);

  transaction_manager = null_ptr;

  return SC_RESULT_ERROR;
}

sc_bool sc_memory_transaction_manager_is_initialized()
{
  return transaction_manager != null_ptr;
}

void sc_memory_transaction_shutdown()
{
  if (transaction_manager != null_ptr)
  {
    transaction_manager->should_stop = SC_TRUE;
    sc_transaction_manager_destroy();
  }
}

sc_memory_transaction_manager * sc_memory_transaction_manager_get()
{
  return transaction_manager;
}

void sc_transaction_manager_destroy()
{
  if (transaction_manager == null_ptr)
    return;

  transaction_manager->should_stop = SC_TRUE;

  sc_cond_broadcast(transaction_manager->queue_condition);

  if (transaction_manager->threads != null_ptr)
  {
    for (sc_struct_node const * it = transaction_manager->threads->begin; it != null_ptr; it = it->next)
    {
      if (it->data != null_ptr)
      {
        sc_thread * thread = it->data;
        sc_thread_join(thread);
        sc_thread_unref(thread);
      }
    }
    sc_list_destroy(transaction_manager->threads);
    transaction_manager->threads = null_ptr;
  }

  if (transaction_manager->transaction_queue != null_ptr)
  {
    sc_queue_destroy(transaction_manager->transaction_queue);
    sc_mem_free(transaction_manager->transaction_queue);
    transaction_manager->transaction_queue = null_ptr;
  }

  if (transaction_manager->monitor != null_ptr)
  {
    sc_monitor_destroy(transaction_manager->monitor);
    sc_mem_free(transaction_manager->monitor);
    transaction_manager->monitor = null_ptr;
  }

  sc_cond_destroy(transaction_manager->queue_condition);
  sc_mutex_destroy(transaction_manager->mutex);

  sc_mem_free(transaction_manager);
  transaction_manager = null_ptr;
}

sc_transaction * sc_memory_transaction_new(sc_memory_context * ctx)
{
  if (!sc_memory_transaction_manager_is_initialized())
  {
    return null_ptr;
  }

  sc_monitor_acquire_write(transaction_manager->monitor);
  sc_uint64 const txn_id = transaction_manager->txn_count++;
  sc_monitor_release_write(transaction_manager->monitor);

  return sc_transaction_new(txn_id, ctx);
}

sc_result sc_memory_transaction_commit(sc_transaction * txn)
{
  if (txn == null_ptr)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  sc_transaction_manager_transaction_add(txn);
  return SC_RESULT_OK;
}

void sc_transaction_manager_transaction_add(sc_transaction * txn)
{
  sc_monitor_acquire_write(transaction_manager->monitor);

  sc_queue_push(transaction_manager->transaction_queue, txn);

  sc_monitor_release_write(transaction_manager->monitor);

  sc_cond_signal(transaction_manager->queue_condition);
}

void sc_transaction_manager_block_elements(sc_hash_table * elements)
{
  // block with monitor table
}

void sc_transaction_manager_unblock_elements(sc_hash_table * elements)
{
  // unlock
}

void sc_transaction_manager_transaction_execute(sc_transaction * txn)
{
  if (!sc_transaction_elements_intersect(txn, transaction_manager->processed_elements))
  {
    sc_transaction_manager_block_elements(txn->elements);

    if (sc_transaction_validate(txn))
    {
      sc_transaction_apply(txn);
    }

    sc_transaction_manager_unblock_elements(txn->elements);
  }
  else
    txn->state = SC_TRANSACTION_FAILED;
}

void sc_transaction_handler()
{
  sc_monitor_acquire_read(transaction_manager->monitor);

  while (!transaction_manager->should_stop)
  {
    if (transaction_manager->transaction_queue->size == 0)
    {
      sc_monitor_release_read(transaction_manager->monitor);
      sc_mutex_lock(transaction_manager->mutex);
      sc_cond_wait(transaction_manager->queue_condition, transaction_manager->mutex);
      sc_mutex_unlock(transaction_manager->mutex);
      sc_monitor_acquire_read(transaction_manager->monitor);
      continue;
    }

    // TODO fix deadlock here (txn_manager monitor) in test thread safety

    sc_monitor_release_read(transaction_manager->monitor);
    sc_monitor_acquire_write(transaction_manager->monitor);

    if (!transaction_manager->should_stop && transaction_manager->transaction_queue->size > 0)
    {
      sc_transaction * txn = sc_queue_front(transaction_manager->transaction_queue);
      sc_queue_pop(transaction_manager->transaction_queue);

      sc_monitor_release_write(transaction_manager->monitor);
      sc_monitor_acquire_read(transaction_manager->monitor);

      sc_transaction_manager_transaction_execute(txn);

      txn->state = SC_TRANSACTION_EXECUTED;
    }
    else
    {
      sc_monitor_release_write(transaction_manager->monitor);
      sc_monitor_acquire_read(transaction_manager->monitor);
    }
  }

  sc_monitor_release_read(transaction_manager->monitor);
}

sc_bool sc_transaction_elements_intersect(sc_transaction const * txn, sc_hash_table * processed_elements)
{
  if (txn == null_ptr || txn->elements == null_ptr || processed_elements == null_ptr)
    return SC_FALSE;

  sc_hash_table * smaller_table =
      sc_hash_table_size(txn->elements) <= sc_hash_table_size(processed_elements) ? txn->elements : processed_elements;
  sc_hash_table * larger_table = smaller_table == txn->elements ? processed_elements : txn->elements;

  sc_monitor * txn_monitor = txn->monitor;
  sc_monitor * processed_monitor = sc_memory_transaction_manager_get()->monitor;

  sc_monitor_acquire_read(txn_monitor);

  sc_hash_table_iterator iter;
  sc_hash_table_iterator_init(&iter, smaller_table);
  void *key, *value;

  while (sc_hash_table_iterator_next(&iter, &key, &value))
  {
    if (sc_hash_table_get(larger_table, key) != null_ptr)
    {
      sc_monitor_release_read(txn_monitor);
      sc_monitor_release_read(processed_monitor);
      return SC_TRUE;
    }
  }

  sc_monitor_release_read(txn_monitor);
  sc_monitor_release_read(processed_monitor);
  return SC_FALSE;
}