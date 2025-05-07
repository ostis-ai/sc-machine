#include "sc_memory_transaction_operations.h"

#include "sc-core/sc_memory.h"
#include "sc-store/sc_storage_private.h"

sc_result sc_memory_transaction_node_new(sc_transaction const * txn, sc_type const type)
{
  sc_result result;

  if (sc_type_is_not_node(type) && (!sc_type_is(type, sc_type_const) && !sc_type_is(type, sc_type_var)))
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE;
    return result;
  }

  sc_element_data * data = sc_element_data_new();
  if (data == null_ptr)
  {
    result = SC_RESULT_ERROR_FULL_MEMORY;
    return result;
  }

  data->flags.type = sc_type_node | type;

  sc_transaction_element_new(txn, data);

  result = SC_RESULT_OK;
  return result;
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
  sc_element_data * beg_new_version = null_ptr;
  sc_storage_get_element_data_by_addr(beg, beg_new_version);

  sc_element_data * end_new_version = null_ptr;
  sc_storage_get_element_data_by_addr(beg, end_new_version);

  sc_element * beg_el = null_ptr;
  sc_storage_get_element_by_addr(beg, &beg_el);

  sc_element * end_el = null_ptr;
  sc_storage_get_element_by_addr(beg, &end_el);


  sc_transaction_element_change(&beg, txn, beg_new_version);
  sc_transaction_element_change(&end, txn, end_new_version);

  return;
}

sc_addr sc_memory_transaction_element_free(sc_transaction const * txn, sc_addr const addr)
{
  sc_transaction_element_remove(txn, &addr);
  return addr;
}