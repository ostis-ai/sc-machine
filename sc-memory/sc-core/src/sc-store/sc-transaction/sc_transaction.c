#include "sc_transaction.h"

#include <sc-core/sc-base/sc_allocator.h>

sc_transaction * sc_transaction_new(const sc_uint64 txn_id)
{
  sc_transaction * txn = sc_mem_new(sc_transaction, 1);
  if (txn == null_ptr)
    return null_ptr;

  txn->transaction_id = txn_id;
  txn->is_committed = SC_FALSE;

  txn->transaction_buffer = sc_mem_new(sc_transaction_buffer, 1);
  if (txn->transaction_buffer == null_ptr)
  {
    sc_mem_free(txn);
    return null_ptr;
  }
  sc_transaction_buffer_initialize(txn->transaction_buffer);

  if(!sc_list_init(&txn->elements))
  {
    return null_ptr;
  }

  return txn;
}

void sc_transaction_destroy(sc_transaction * txn)
{
  if (txn != null_ptr)
  {
    sc_list_destroy(txn->elements);

    sc_mem_free(txn);
  }
}

void sc_transaction_add_element(sc_transaction * txn, sc_element * element) {}

void sc_transaction_commit(sc_transaction * txn) {}

void sc_transaction_rollback(sc_transaction * txn) {}

sc_bool sc_transaction_validate(sc_transaction * txn)
{
  return SC_FALSE;
}

void sc_transaction_merge(sc_transaction * txn) {}

void sc_transaction_apply(sc_transaction * txn) {}

void sc_transaction_clear(sc_transaction * txn) {}

sc_bool sc_transaction_element_new(sc_transaction * txn, sc_element * element) {}

sc_bool sc_transaction_element_change(
    sc_element * element,
    sc_transaction * txn,
    SC_ELEMENT_MODIFIED_FLAGS modified_flags,
    sc_element const * new_data)
{
}

sc_bool sc_transaction_element_remove(sc_transaction * txn, sc_addr addr) {}

sc_bool sc_transaction_element_content_set(sc_transaction * txn, sc_addr addr, sc_stream * content) {}