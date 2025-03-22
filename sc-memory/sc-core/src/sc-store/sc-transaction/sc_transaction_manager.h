#ifndef SC_TRANSACTION_MANAGER_H
#define SC_TRANSACTION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "epoch_manager.h"

typedef struct transaction_manager {
  uint64_t current_transaction_id;
  epoch_manager* epoch_mgr;
} transaction_manager;

void initialize_transaction_manager(transaction_manager* manager);
uint64_t start_transaction(transaction_manager* manager);
void commit_transaction(transaction_manager* manager, uint64_t transaction_id);
void rollback_transaction(transaction_manager* manager, uint64_t transaction_id);

#endif
