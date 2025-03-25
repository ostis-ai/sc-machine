#ifndef SC_TRANSACTION_MANAGER_H
#define SC_TRANSACTION_MANAGER_H

#include "sc_transaction.h"
#include "sc-store/sc-base/sc_monitor_private.h"

typedef struct sc_transaction_manager {
  sc_transaction*current_sc_transaction;
  sc_queue* transaction_queue;
  sc_monitor* monitor;
  sc_uint64 transaction_counter;
} sc_transaction_manager;

sc_transaction_manager* sc_transaction_manager_initialize();
// create and initialize the transaction manager
sc_bool sc_transaction_manager_is_initialized();
// check if the transaction manager is initialized
void sc_transaction_shutdown();
// shutdown the transaction manager

sc_transaction* sc_transaction_manager_transaction_new();
// create a new empty sc-transaction
void sc_transaction_manager_transaction_add(sc_transaction* txn);
// add transaction to the queue and start operations when the thread is ready
void sc_transaction_manager_transaction_execute();
// try to execute the transaction and wait till it's finished

void sc_transaction_manager_destroy();
// destroy the transaction manager

#endif
