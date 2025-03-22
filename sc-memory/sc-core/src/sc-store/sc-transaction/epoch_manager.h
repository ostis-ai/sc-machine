#ifndef SC_EPOCH_MANAGER_H
#define SC_EPOCH_MANAGER_H

#include <stdint.h>

typedef struct epoch_manager {
  uint64_t current_epoch;
} epoch_manager;

void initialize_epoch_manager(epoch_manager* manager);
uint64_t start_epoch(epoch_manager* manager);

#endif
