#ifndef SC_TRANSACTION_MANAGER_H
#define SC_TRANSACTION_MANAGER_H

#include <sc-store/sc-transaction/sc_transaction.h>
#include <sc-store/sc-base/sc_monitor_private.h>

// TODO: determinate amount
#define SC_TRANSACTION_THREAD_COUNT 2
#define SC_TRANSACTION_MANAGER_QUEUE_SIZE 10

typedef struct sc_transaction_manager
{
  sc_queue * transaction_queue;
  sc_monitor * monitor;
  sc_mutex * mutex;
  sc_list * threads;

  sc_condition * queue_condition;
  sc_bool should_stop;
} sc_transaction_manager;

sc_transaction_manager * sc_transaction_manager_initialize();
// create and initialize the transaction manager
sc_bool sc_transaction_manager_is_initialized();
// check if the transaction manager is initialized
void sc_transaction_shutdown();
// shutdown the transaction manager

sc_transaction * sc_transaction_manager_transaction_new();
// create a new empty sc-transaction
void sc_transaction_manager_transaction_add(sc_transaction * txn);
// add transaction to the queue and start operations when the thread is ready
void sc_transaction_manager_transaction_execute(sc_transaction * txn);
// execute the transaction and wait till it's finished

void sc_transaction_handler();
// method for transaction queue processing by threads

void sc_transaction_manager_destroy();
// destroy the transaction manager

#endif
