#include "sc_transaction.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_transaction* sc_transaction_create(const sc_uint64* txn_id) {
  sc_transaction *txn = sc_mem_new(sc_transaction, 1);
  if (txn == NULL) {
    return NULL;
  }

  txn->transaction_id = *txn_id;
  txn->is_committed = SC_FALSE;
  txn->element_count = 0;
  sc_list_init(&txn->elements);
  return txn;
}

void sc_transaction_destroy(sc_transaction* txn) {
  if (txn != NULL) {
    sc_list_clear(txn->elements);

    sc_mem_free(txn);
  }
}

void sc_transaction_add_element(sc_transaction* txn, sc_element* element) {
}

void sc_transaction_commit(sc_transaction* txn) {
}

void sc_transaction_rollback(sc_transaction* txn) {
}

sc_bool sc_transaction_validate(sc_transaction* txn) {
  return SC_FALSE;
}

void sc_transaction_merge(sc_transaction* txn) {
}

void sc_transaction_apply(sc_transaction* txn) {
}

void sc_transaction_clear(sc_transaction* txn) {
}
