#ifndef SC_MEMORY_TRANSACTION_OPERATIONS_H
#define SC_MEMORY_TRANSACTION_OPERATIONS_H

#include "sc_transaction.h"

sc_result sc_memory_transaction_node_new(sc_transaction const * txn, sc_type type);
sc_result sc_memory_transaction_link_new(sc_transaction const * txn);
sc_result sc_memory_transaction_arc_new(
    sc_transaction const * txn,
    sc_type type,
    sc_addr * beg_addr,
    sc_addr * end_addr);
sc_addr sc_memory_transaction_element_free(sc_transaction const * txn, sc_addr addr);

#endif  // SC_MEMORY_TRANSACTION_OPERATIONS_H
