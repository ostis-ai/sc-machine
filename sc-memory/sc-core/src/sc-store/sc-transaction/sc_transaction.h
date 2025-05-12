#ifndef SC_TRANSACTION_H
#define SC_TRANSACTION_H

#include "sc-store/sc-container/sc_hash_table.h"
#include <sc-store/sc-transaction/sc_element_version.h>
#include <sc-store/sc-transaction/sc_transaction_buffer.h>
#include <sc-core/sc_stream.h>

typedef struct sc_transaction
{
  sc_uint64 transaction_id;
  sc_bool is_committed;
  sc_hash_table * elements;
  sc_monitor * monitor;
  sc_transaction_buffer * transaction_buffer;
  sc_memory_context * ctx;
} sc_transaction;

sc_transaction * sc_transaction_new(sc_uint64 txn_id, sc_memory_context * ctx);
// create a new transaction
void sc_transaction_destroy(sc_transaction * txn);
// destroy the given transaction

sc_bool sc_transaction_element_new(sc_transaction const * txn, sc_addr const * addr);
// Writes information about adding an item to the buffer
sc_bool sc_transaction_element_change(sc_transaction const * txn, sc_addr const * addr, sc_element_data const * new_data);
// Writes information about the element change to the buffer
sc_bool sc_transaction_element_remove(sc_transaction const * txn, sc_addr const * addr);
// Writes the deletion information to the buffer
sc_bool sc_transaction_element_content_set(sc_transaction const * txn, sc_addr const * addr, sc_stream const * content);
// Writes information about content changes to the buffer

void sc_transaction_commit(sc_transaction * txn);
// try to apply all operations of the transaction on sc-memory
void sc_transaction_rollback(sc_transaction * txn);
// rollback the transaction operations

sc_bool sc_transaction_validate(sc_transaction * txn);
// check if the transaction can be applied based on element versions and free segments
void sc_transaction_merge(sc_transaction * txn);
// check versions of all elements in the transaction and try to merge them
void sc_transaction_apply(sc_transaction * txn);
// apply all operations (merged versions, allocated spaces) to sc-memory
void sc_transaction_clear(sc_transaction * txn);
// deletes all transaction items and clears them without performing a commit

#endif
