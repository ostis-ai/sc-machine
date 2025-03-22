#ifndef SC_TRANSACTION_H
#define SC_TRANSACTION_H

#include <stdint.h>

typedef struct transaction {
  uint64_t transaction_id;
  uint64_t timestamp;
  bool is_committed;
} transaction;

#endif