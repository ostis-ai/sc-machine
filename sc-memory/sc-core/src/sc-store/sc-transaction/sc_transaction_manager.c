#include "sc_transaction_manager.h"

#include "sc-store/sc-base/sc_thread.h"
#include "sc-store/sc-container/sc_struct_node.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_transaction_manager * transaction_manager = null_ptr;

sc_transaction_manager * sc_transaction_manager_initialize()
{
  if (sc_transaction_manager_is_initialized())
  {
    return transaction_manager;
  }

  transaction_manager = sc_mem_new(sc_transaction_manager, 1);
  if (transaction_manager == null_ptr)
  {
    return null_ptr;
  }

  transaction_manager->should_stop = SC_FALSE;
  transaction_manager->transaction_counter = 0;
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
    goto error_cleanup_threads;
  }

  for (int i = 0; i < SC_TRANSACTION_THREAD_COUNT; ++i)
  {
    sc_thread * thread = g_thread_new("sc_transaction_handler", sc_transaction_handler, transaction_manager);
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
      g_thread_unref(it->data);
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

void sc_transaction_shutdown(sc_transaction_manager * manager)
{
  if (transaction_manager != null_ptr)
  {
    manager->should_stop = SC_TRUE;
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
      g_thread_join(it->data);
      g_thread_unref(it->data);
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
  sc_uint64 const txn_id = transaction_manager->transaction_counter++;
  sc_monitor_release_write(transaction_manager->monitor);

  return sc_transaction_new(txn_id);
}

void sc_transaction_manager_transaction_add(sc_transaction * txn) {}

void sc_transaction_manager_transaction_execute() {}

void sc_transaction_handler(sc_transaction_manager * manager)
{
  sc_monitor_acquire_read(manager->monitor);

  while (!manager->should_stop)
  {
    if (manager->transaction_counter == 0)
    {
      sc_monitor_release_read(manager->monitor);
      sc_mutex_lock(manager->mutex);
      sc_cond_wait(manager->queue_condition, manager->mutex);
      sc_mutex_unlock(manager->mutex);
      sc_monitor_acquire_read(manager->monitor);
      continue;
    }

    sc_monitor_release_read(manager->monitor);
    sc_monitor_acquire_write(manager->monitor);

    if (!manager->should_stop && manager->transaction_counter > 0)
    {
      sc_transaction * txn = sc_queue_front(manager->transaction_queue);
      sc_queue_pop(manager->transaction_queue);
      manager->transaction_counter--;

      sc_transaction_manager_transaction_execute(txn);
    }

    sc_monitor_release_write(manager->monitor);
    sc_monitor_acquire_read(manager->monitor);
  }

  sc_monitor_release_read(manager->monitor);
}

