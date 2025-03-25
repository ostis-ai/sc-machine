#include "sc_transaction.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_transaction* sc_transaction_new(const sc_uint64* txn_id)
{
  if (txn_id == NULL)
    return NULL;

  sc_transaction *txn = sc_mem_new(sc_transaction, 1);
  if (txn == NULL)
    return NULL;

  txn->transaction_id = *txn_id;
  txn->is_committed = SC_FALSE;

  txn->elements = sc_mem_new(sc_list, 1);
  if (txn->elements == NULL)
  {
    sc_mem_free(txn);
    return NULL;
  }

  txn->transaction_buffer = sc_mem_new(sc_transaction_buffer, 1);
  if (txn->transaction_buffer == NULL)
  {
    sc_mem_free(txn->elements);
    sc_mem_free(txn);
    return NULL;
  }

  sc_transaction_buffer_initialize(txn->transaction_buffer);
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
