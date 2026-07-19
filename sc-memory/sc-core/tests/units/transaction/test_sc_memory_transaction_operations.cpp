#include <gtest/gtest.h>
#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include <sc-store/sc-transaction/sc_transaction.h>
#include "sc-store/sc-container/sc_pair.h"
#include "sc-store/sc_storage_private.h"
#include "sc-store/sc_version_segment.h"
#include "sc-store/sc-transaction/sc_memory_transaction_manager.h"
#include <sc-store/sc-transaction/sc_memory_transaction_operations.h>
}

class ScMemoryTransactionArcNewTest : public ScMemoryTest
{
protected:
  sc_transaction * transaction = nullptr;
  sc_addr beg_addr = {}, end_addr = {};

  void SetUp() override
  {
    ScMemoryTest::SetUp();
    beg_addr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node);
    end_addr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node);

    transaction = sc_transaction_new(123, m_ctx->GetRealContext());
    ASSERT_NE(transaction, nullptr);
  }

  void TearDown() override
  {
    if (transaction)
      sc_transaction_destroy(transaction);
    ScMemoryTest::TearDown();
  }
};

sc_result sc_transaction_buffer_find_new_element(
    sc_transaction_buffer const * buffer,
    sc_addr const * addr,
    sc_uint32 * out_hash)
{
  if (buffer == null_ptr || buffer->new_elements == null_ptr || addr == null_ptr || out_hash == null_ptr)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
  sc_iterator * it = sc_list_iterator(buffer->new_elements);
  while (sc_iterator_next(it))
  {
    sc_uint32 const stored_hash = (uintptr_t)sc_iterator_get(it);
    if (stored_hash == addr_hash)
    {
      *out_hash = stored_hash;
      sc_iterator_destroy(it);
      return SC_RESULT_OK;
    }
  }
  sc_iterator_destroy(it);

  return SC_RESULT_ERROR_NOT_FOUND;
}

sc_result sc_transaction_buffer_find_modified_element(
    sc_transaction_buffer const * buffer,
    sc_addr const * addr,
    sc_element_data ** out_data)
{
  if (buffer == null_ptr || buffer->modified_elements == null_ptr || addr == null_ptr || out_data == null_ptr)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (SC_ADDR_IS_EMPTY(*addr))
    return SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
  sc_iterator * it = sc_list_iterator(buffer->modified_elements);
  while (sc_iterator_next(it))
  {
    auto pair = static_cast<sc_pair *>(sc_iterator_get(it));

    sc_uint32 stored_hash = reinterpret_cast<uintptr_t>(pair->first);
    if (stored_hash == addr_hash)
    {
      *out_data = static_cast<sc_element_data *>(pair->second);
      sc_iterator_destroy(it);
      return SC_RESULT_OK;
    }
  }
  sc_iterator_destroy(it);

  return SC_RESULT_ERROR_NOT_FOUND;
}

TEST_F(ScMemoryTransactionArcNewTest, ValidParams)
{
  EXPECT_EQ(sc_memory_transaction_arc_new(transaction, sc_type_pos_arc, &beg_addr, &end_addr), SC_RESULT_OK);
}

TEST_F(ScMemoryTransactionArcNewTest, InvalidAddress)
{
  sc_addr empty_addr = SC_ADDR_EMPTY;

  EXPECT_EQ(
      sc_memory_transaction_arc_new(transaction, sc_type_pos_arc, &empty_addr, &end_addr),
      SC_RESULT_ERROR_ADDR_IS_NOT_VALID);
  EXPECT_EQ(
      sc_memory_transaction_arc_new(transaction, sc_type_pos_arc, &beg_addr, &empty_addr),
      SC_RESULT_ERROR_ADDR_IS_NOT_VALID);
}

TEST_F(ScMemoryTransactionArcNewTest, InvalidType)
{
  EXPECT_EQ(
      sc_memory_transaction_arc_new(transaction, sc_type_node, &beg_addr, &end_addr),
      SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR);
}

TEST_F(ScMemoryTransactionArcNewTest, NullParams)
{
  EXPECT_EQ(
      sc_memory_transaction_arc_new(nullptr, sc_type_pos_arc, &beg_addr, &end_addr), SC_RESULT_ERROR_INVALID_PARAMS);
  EXPECT_EQ(
      sc_memory_transaction_arc_new(transaction, sc_type_pos_arc, nullptr, &end_addr), SC_RESULT_ERROR_INVALID_PARAMS);
  EXPECT_EQ(
      sc_memory_transaction_arc_new(transaction, sc_type_pos_arc, &beg_addr, nullptr), SC_RESULT_ERROR_INVALID_PARAMS);
}

TEST_F(ScMemoryTransactionArcNewTest, BufferCheck)
{
  sc_element * beg_el_before = nullptr;
  sc_storage_get_element_by_addr(beg_addr, &beg_el_before);

  sc_element * end_el_before = nullptr;
  sc_storage_get_element_by_addr(end_addr, &end_el_before);

  EXPECT_EQ(sc_memory_transaction_arc_new(transaction, sc_type_pos_arc, &beg_addr, &end_addr), SC_RESULT_OK);

  sc_element * beg_el_after = nullptr;
  sc_storage_get_element_by_addr(beg_addr, &beg_el_after);

  sc_element * end_el_after = nullptr;
  sc_storage_get_element_by_addr(end_addr, &end_el_after);

  EXPECT_EQ(beg_el_after->outgoing_arcs_count, beg_el_after->outgoing_arcs_count);
  EXPECT_EQ(beg_el_after->incoming_arcs_count, beg_el_after->incoming_arcs_count);

  sc_element_data * beg_el_ver = nullptr;
  sc_transaction_buffer_find_modified_element(transaction->transaction_buffer, &beg_addr, &beg_el_ver);

  sc_element_data * end_el_ver = nullptr;
  sc_transaction_buffer_find_modified_element(transaction->transaction_buffer, &end_addr, &end_el_ver);

  size_t const beg_diff = beg_el_ver->outgoing_arcs_count - beg_el_after->outgoing_arcs_count;
  size_t const end_diff = end_el_ver->incoming_arcs_count - end_el_after->incoming_arcs_count;

  EXPECT_EQ(beg_diff, 1);
  EXPECT_EQ(end_diff, 1);
}

TEST_F(ScMemoryTransactionArcNewTest, FullTxnTest)
{
  EXPECT_EQ(sc_memory_transaction_arc_new(transaction, sc_type_pos_arc, &beg_addr, &end_addr), SC_RESULT_OK);

  sc_element * beg_el_before = nullptr;
  sc_storage_get_element_by_addr(beg_addr, &beg_el_before);
  sc_element * end_el_before = nullptr;
  sc_storage_get_element_by_addr(end_addr, &end_el_before);

  EXPECT_EQ(sc_hash_table_size(transaction->elements), 3);
  EXPECT_EQ(beg_el_before->version_history->count, 0);
  EXPECT_EQ(end_el_before->version_history->count, 0);

  EXPECT_EQ(sc_memory_transaction_commit(transaction), SC_RESULT_OK);

  while (transaction->state != SC_TRANSACTION_EXECUTED)
  {
  }

  sc_element * beg_el_after = nullptr;
  sc_storage_get_element_by_addr(beg_addr, &beg_el_after);
  sc_element * end_el_after = nullptr;
  sc_storage_get_element_by_addr(end_addr, &end_el_after);

  EXPECT_EQ(beg_el_before->version_history->count, 1);
  EXPECT_EQ(end_el_before->version_history->count, 1);
}