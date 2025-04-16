#include "sc_memory_transaction_operations.h"

#include "sc-core/sc_memory.h"
#include "sc-store/sc_element.h"
#include "sc-store/sc_storage_private.h"

sc_addr sc_memory_transaction_node_new(const sc_transaction * txn, const sc_type type)
{
  const sc_addr allocated_addr = sc_memory_node_new(txn->ctx, type);
  sc_transaction_element_new(txn, &allocated_addr);
  return allocated_addr;
}

sc_addr sc_memory_transaction_link_new(const sc_transaction * txn)
{
  const sc_addr allocated_addr = sc_memory_link_new(txn->ctx);
  sc_transaction_element_new(txn, &allocated_addr);
  return allocated_addr;
}

sc_addr sc_memory_transaction_arc_new(const sc_transaction * txn, const sc_type type, const sc_addr beg, const sc_addr end)
{
  sc_element * beg_new_version;
  sc_element * end_new_version;

  sc_addr beg_new_version_addr = SC_ADDR_EMPTY;
  beg_new_version = sc_storage_allocate_new_element(txn->ctx, &beg_new_version_addr);
  sc_storage_get_element_by_addr(beg, &beg_new_version);

  sc_addr end_new_version_addr = SC_ADDR_EMPTY;
  end_new_version = sc_storage_allocate_new_element(txn->ctx, &end_new_version_addr);
  sc_storage_get_element_by_addr(beg, &end_new_version);

  const sc_addr allocated_addr = sc_memory_arc_new(txn->ctx, type, beg_new_version_addr, end_new_version_addr);
  sc_transaction_element_new(txn, &allocated_addr);

  sc_storage_get_element_by_addr(beg_new_version_addr, &beg_new_version);
  sc_storage_get_element_by_addr(beg_new_version_addr, &end_new_version);

  sc_transaction_element_change(&beg, txn, SC_ELEMENT_ARCS_MODIFIED, beg_new_version);
  sc_transaction_element_change(&end, txn, SC_ELEMENT_ARCS_MODIFIED, end_new_version);

  return allocated_addr;
}

sc_addr sc_memory_transaction_element_free(const sc_transaction * txn, const sc_addr addr)
{
  sc_transaction_element_remove(txn, &addr);
  return addr;
}