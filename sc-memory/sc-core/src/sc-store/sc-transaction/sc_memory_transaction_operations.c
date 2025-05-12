#include "sc_memory_transaction_operations.h"

#include "sc_memory_transaction_manager.h"
#include "sc-core/sc_memory.h"
#include "sc-store/sc_storage_private.h"

sc_result sc_memory_transaction_node_new(sc_transaction const * txn, sc_type const type)
{
  if (txn == null_ptr)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  sc_result result;
  sc_addr const allocated_addr = sc_memory_node_new_ext(txn->ctx, type, &result);
  sc_transaction_element_new(txn, &allocated_addr);

  return result;
}

sc_result sc_memory_transaction_link_new(sc_transaction const * txn)
{
  if (txn == null_ptr)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  sc_result result;
  sc_addr const allocated_addr = sc_memory_link_new_ext(txn->ctx, sc_type_const_node_link, &result);
  sc_transaction_element_new(txn, &allocated_addr);

  return result;
}

sc_result sc_memory_transaction_arc_new(
    sc_transaction const * txn,
    sc_type const type,
    sc_addr * const beg_addr,
    sc_addr * const end_addr)
{
  if (txn == null_ptr)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  sc_result result;
  sc_addr connector_addr = SC_ADDR_EMPTY;

  if (sc_type_is_not_connector(type))
  {
    return SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
  }

  if (beg_addr == null_ptr || end_addr == null_ptr)
  {
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  if (SC_ADDR_IS_EMPTY(*beg_addr) || SC_ADDR_IS_EMPTY(*end_addr))
  {
    return SC_RESULT_ERROR_ADDR_IS_NOT_VALID;
  }

  sc_element_data new_beg_ver_val;
  sc_element_data new_end_ver_val;

  sc_element_data *new_beg_ver = &new_beg_ver_val;
  sc_element_data *new_end_ver = &new_end_ver_val;

  sc_element * arc_el = sc_storage_allocate_new_element(txn->ctx, &connector_addr);
  if (arc_el == null_ptr)
  {
    return SC_RESULT_ERROR_FULL_MEMORY;
  }

  arc_el->flags.type = type;
  arc_el->arc.begin = *beg_addr;
  arc_el->arc.end = *end_addr;

  sc_bool const is_edge = sc_type_has_subtype(type, sc_type_common_edge);
  sc_bool const is_not_loop = SC_ADDR_IS_NOT_EQUAL(*beg_addr, *end_addr);

  // try to lock begin and end elements
  sc_monitor * beg_monitor =
      sc_monitor_table_get_monitor_for_addr(sc_memory_transaction_manager_get()->monitor_table, *beg_addr);
  sc_monitor * end_monitor =
      sc_monitor_table_get_monitor_for_addr(sc_memory_transaction_manager_get()->monitor_table, *end_addr);
  sc_monitor_acquire_write_n(2, beg_monitor, end_monitor);

  if (beg_monitor == null_ptr)
    return SC_RESULT_ERROR;
  if (end_monitor == null_ptr)
    return SC_RESULT_ERROR;

  result = sc_storage_get_element_data_by_addr(*beg_addr, new_beg_ver);
  if (result != SC_RESULT_OK)
    goto error;

  result = sc_storage_get_element_data_by_addr(*end_addr, new_end_ver);
  if (result != SC_RESULT_OK)
    goto error;

  // lock arcs to change output/input list
  sc_storage_make_elements_incident_to_arc(
      connector_addr, arc_el, *beg_addr, (sc_element*)new_beg_ver, *end_addr, (sc_element*)new_end_ver, SC_FALSE, !is_not_loop);
  if (is_edge && is_not_loop)
    sc_storage_make_elements_incident_to_arc(
        connector_addr, arc_el, *end_addr, (sc_element*)new_end_ver, *beg_addr, (sc_element*)new_beg_ver, SC_TRUE, SC_FALSE);

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
  if (sc_type_is_structure_and_arc(new_beg_ver->flags.type, type))
    sc_storage_update_structure_arcs(connector_addr, arc_el, *beg_addr, *end_addr, (sc_element*)new_end_ver);
#endif

  sc_transaction_element_new(txn, &connector_addr);
  sc_transaction_element_change(txn, beg_addr, new_beg_ver);
  sc_transaction_element_change(txn, end_addr, new_end_ver);

  return result;

error:
  sc_storage_free_element(connector_addr);
  sc_monitor_release_write_n(2, beg_monitor, end_monitor);
  return result;
}

sc_addr sc_memory_transaction_element_free(sc_transaction const * txn, sc_addr const addr)
{
  if (txn == null_ptr)
    return addr;

  sc_transaction_element_remove(txn, &addr);
  return addr;
}