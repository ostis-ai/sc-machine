#include "sc_transaction_manager.h"

#include "sc-store/sc-base/sc_thread.h"
#include "sc-store/sc-container/sc_struct_node.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_transaction_manager * transaction_manager = null_ptr;

sc_transaction_manager * sc_transaction_manager_initialize(sc_transaction_manager* manager)
{
  if (sc_transaction_manager_is_initialized())
  {
    return transaction_manager;
  }

  transaction_manager = manager;

  if (transaction_manager == null_ptr)
  {
    return null_ptr;
  }

  transaction_manager->should_stop = SC_FALSE;
  transaction_manager->threads = null_ptr;

  transaction_manager->transaction_queue = sc_mem_new(sc_queue, 1);
  if (transaction_manager->transaction_queue == null_ptr)
  {
    goto error_cleanup_manager;
  }

  sc_queue_init(transaction_manager->transaction_queue);

  transaction_manager->monitor = sc_mem_new(sc_monitor, 1);
  if (transaction_manager->monitor == null_ptr)
  {
    goto error_cleanup_queue;
  }

  sc_monitor_init(transaction_manager->monitor);

  sc_cond_init(transaction_manager->queue_condition);
  sc_mutex_init(transaction_manager->mutex);

  if (sc_list_init(&transaction_manager->threads) != SC_RESULT_OK)
  {
    goto error_cleanup_condition;
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

  return transaction_manager;

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

error_cleanup_mutex:
  sc_mutex_destroy(transaction_manager->mutex);

error_cleanup_condition:
  sc_cond_destroy(transaction_manager->queue_condition);

error_cleanup_monitor:
  sc_monitor_destroy(transaction_manager->monitor);
  sc_mem_free(transaction_manager->monitor);

error_cleanup_queue:
  sc_queue_destroy(transaction_manager->transaction_queue);
  sc_mem_free(transaction_manager->transaction_queue);

error_cleanup_manager:
  sc_mem_free(transaction_manager);
  transaction_manager = null_ptr;

  return null_ptr;
}

sc_bool sc_transaction_manager_is_initialized()
{
  return transaction_manager != null_ptr;
}

void sc_transaction_shutdown()
{
  if (transaction_manager != null_ptr)
  {
    transaction_manager->should_stop = SC_TRUE;
    sc_transaction_manager_destroy();
  }
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
      sc_thread_join(it->data);
      sc_thread_unref(it->data);
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


sc_transaction * sc_transaction_manager_transaction_new()
{
  if (!sc_transaction_manager_is_initialized())
  {
    return null_ptr;
  }

  sc_monitor_acquire_write(transaction_manager->monitor);
  sc_uint64 const txn_id = transaction_manager->transaction_queue->size++;
  sc_monitor_release_write(transaction_manager->monitor);

  return sc_transaction_new(txn_id);
}

void sc_transaction_manager_transaction_add(sc_transaction * txn)
{
  sc_monitor_acquire_write(transaction_manager->monitor);

  sc_queue_push(transaction_manager->transaction_queue, txn);

  sc_monitor_release_write(transaction_manager->monitor);

  sc_cond_signal(transaction_manager->queue_condition);
}

void sc_transaction_manager_transaction_execute(sc_transaction * txn)
{

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

    sc_monitor_release_read(transaction_manager->monitor);
    sc_monitor_acquire_write(transaction_manager->monitor);

    if (!transaction_manager->should_stop && transaction_manager->transaction_queue->size > 0)
    {
      sc_transaction * txn = sc_queue_front(transaction_manager->transaction_queue);
      sc_queue_pop(transaction_manager->transaction_queue);

      sc_transaction_manager_transaction_execute(txn);
    }

    sc_monitor_release_write(transaction_manager->monitor);
    sc_monitor_acquire_read(transaction_manager->monitor);
  }

  sc_monitor_release_read(transaction_manager->monitor);
}

