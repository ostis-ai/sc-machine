#include "sc_memory_transaction_operations.h"

#include "sc-core/sc_memory.h"
#include "sc-store/sc_element.h"
#include "sc-store/sc_storage_private.h"

sc_addr sc_memory_transaction_node_new(sc_transaction const * txn, sc_type const type)
{
  sc_addr const allocated_addr = sc_memory_node_new(txn->ctx, type);
  sc_transaction_element_new(txn, &allocated_addr);
  return allocated_addr;
}

sc_addr sc_memory_transaction_link_new(sc_transaction const * txn)
{
  sc_addr const allocated_addr = sc_memory_link_new(txn->ctx);
  sc_transaction_element_new(txn, &allocated_addr);
  return allocated_addr;
}

sc_addr sc_memory_transaction_arc_new(
    sc_transaction const * txn,
    sc_type const type,
    sc_addr const beg,
    sc_addr const end)
{
  sc_element * beg_new_version;
  sc_element * end_new_version;

  sc_addr beg_new_version_addr = SC_ADDR_EMPTY;
  beg_new_version = sc_storage_allocate_new_element(txn->ctx, &beg_new_version_addr);
  sc_storage_get_element_by_addr(beg, &beg_new_version);

  sc_addr end_new_version_addr = SC_ADDR_EMPTY;
  end_new_version = sc_storage_allocate_new_element(txn->ctx, &end_new_version_addr);
  sc_storage_get_element_by_addr(beg, &end_new_version);

  sc_addr const allocated_addr = sc_memory_arc_new(txn->ctx, type, beg_new_version_addr, end_new_version_addr);
  sc_transaction_element_new(txn, &allocated_addr);

  sc_storage_get_element_by_addr(beg_new_version_addr, &beg_new_version);
  sc_storage_get_element_by_addr(beg_new_version_addr, &end_new_version);

  sc_transaction_element_change(&beg, txn, beg_new_version);
  sc_transaction_element_change(&end, txn, end_new_version);

  return allocated_addr;
}

sc_addr sc_memory_transaction_element_free(sc_transaction const * txn, sc_addr const addr)
{
  sc_transaction_element_remove(txn, &addr);
  return addr;
}