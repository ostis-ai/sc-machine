#ifndef SC_TRANSACTION_MANAGER_H
#define SC_TRANSACTION_MANAGER_H

#include "sc_transaction.h"
#include "sc-store/sc-base/sc_monitor_private.h"

#include <sc-core/sc-base/sc_monitor.h>

typedef struct sc_transaction_manager
{
  sc_transaction * current_sc_transaction;
  sc_monitor sc_monitor;
  sc_queue sc_transaction_queue;
} sc_transaction_manager;

sc_transaction_manager* sc_transaction_manager_initialize(void);

#endif
