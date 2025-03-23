#ifndef SC_TRANSACTION_H
#define SC_TRANSACTION_H

#include <stdint.h>
#include <stdbool.h>
#include <sc-core/sc-container/sc_list.h>

typedef struct sc_transaction {
  uint64_t transaction_id;
  uint64_t timestamp;
  bool is_committed;
  sc_list elements;
  sc_uint32 element_count;
} sc_transaction;

#endif