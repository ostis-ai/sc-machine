#include "sc_memory_transaction_operations.h"

#include "sc-core/sc_memory.h"

sc_addr sc_memory_transaction_node_new(const sc_transaction * txn, const sc_type type)
{
  sc_result result;
  const sc_addr allocated_addr = sc_memory_node_new_ext(txn->ctx, type, &result);
  sc_transaction_element_new(txn, &allocated_addr);
  return allocated_addr;
}