#include "sc_transaction_manager.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_transaction_manager * transaction_manager = null_ptr;

sc_transaction_manager * sc_transaction_manager_initialize()
{
  if (sc_transaction_manager_is_initialized())
  {
    return transaction_manager;
  }

  transaction_manager = sc_mem_new(sc_transaction_manager, 1);
  if (!sc_transaction_manager_is_initialized())
  {
    return null_ptr;
  }

  transaction_manager->current_sc_transaction = null_ptr;

  transaction_manager->transaction_queue = sc_mem_new(sc_queue, 1);
  transaction_manager->monitor = sc_mem_new(sc_monitor, 1);

  if (transaction_manager->transaction_queue == NULL || transaction_manager->monitor == NULL)
  {
    sc_mem_free(transaction_manager);
    return NULL;
  }

  sc_queue_init(transaction_manager->transaction_queue);
  sc_monitor_init(transaction_manager->monitor);
  transaction_manager->transaction_counter = 0;

  return transaction_manager;
}

sc_bool sc_transaction_manager_is_initialized()
{
  return transaction_manager != null_ptr;
}

void sc_transaction_shutdown()
{
  if (transaction_manager != null_ptr)
  {
    sc_transaction_manager_destroy();
  }
}

void sc_transaction_manager_destroy()
{
  if (transaction_manager != null_ptr)
  {
    sc_monitor_acquire_write(transaction_manager->monitor);
    sc_queue_destroy(transaction_manager->transaction_queue);
    sc_monitor_release_write(transaction_manager->monitor);

    sc_monitor_destroy(transaction_manager->monitor);

    sc_mem_free(transaction_manager);
    transaction_manager = null_ptr;
  }
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

  return sc_transaction_new(&txn_id);
}

void sc_transaction_manager_transaction_add(sc_transaction * txn) {}

void sc_transaction_manager_transaction_execute() {}
