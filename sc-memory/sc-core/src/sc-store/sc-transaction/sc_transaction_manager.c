#include "sc_transaction_manager.h"
#include <stdlib.h>
#include <stdio.h>

sc_transaction_manager* sc_transaction_manager_initialize(void) {
  sc_transaction_manager* manager = malloc(sizeof(sc_transaction_manager));
  if (!manager)
  {
    perror("Failed to allocate memory for transaction manager");
    return NULL;
  }

  manager->current_sc_transaction = NULL;
  sc_monitor_init(&manager->sc_monitor);
  sc_queue_init(&manager->sc_transaction_queue);

  return manager;
}
