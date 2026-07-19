#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include <sc-store/sc-container/sc_pair.h>
#include <sc-store/sc-transaction/sc_transaction.h>
#include <sc-store/sc_storage_private.h>
}

class ScTransactionTest : public ScMemoryTest
{
protected:
  sc_transaction * transaction = nullptr;
  sc_uint64 const test_txn_id = 123;

  void SetUp() override
  {
    ScMemoryTest::SetUp();
    transaction = sc_transaction_new(test_txn_id, m_ctx->GetRealContext());
    ASSERT_NE(transaction, nullptr);
  }

  void TearDown() override
  {
    if (transaction != nullptr)
      sc_transaction_destroy(transaction);
    ScMemoryTest::TearDown();
  }
};

TEST_F(ScTransactionTest, CreationAndDestruction)
{
  EXPECT_EQ(transaction->transaction_id, test_txn_id);
  EXPECT_EQ(transaction->is_committed, SC_FALSE);
  EXPECT_NE(transaction->transaction_buffer, nullptr);
  EXPECT_NE(transaction->elements, nullptr);
}

TEST_F(ScTransactionTest, TransactionElementNew)
{
  constexpr sc_addr addr = {1, 2};
}

TEST_F(ScTransactionTest, TransactionElementRemove)
{
  constexpr sc_addr addr = {3, 4};

  EXPECT_TRUE(sc_transaction_element_remove(transaction, &addr));

  constexpr sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  sc_iterator * it = sc_list_iterator(transaction->transaction_buffer->deleted_elements);
  sc_bool found = SC_FALSE;
  while (sc_iterator_next(it))
  {
    if (reinterpret_cast<uintptr_t>(sc_iterator_get(it)) == addr_hash)
    {
      found = SC_TRUE;
      break;
    }
  }
  sc_iterator_destroy(it);
  EXPECT_TRUE(found);

  EXPECT_FALSE(sc_transaction_element_remove(nullptr, &addr));
  EXPECT_FALSE(sc_transaction_element_remove(transaction, nullptr));
}

TEST_F(ScTransactionTest, TransactionElementContentSet)
{
  constexpr sc_addr addr = {5, 6};

  sc_stream * stream = sc_stream_memory_new("content", 7, SC_STREAM_FLAG_READ, SC_FALSE);
  ASSERT_NE(stream, nullptr);

  EXPECT_TRUE(sc_transaction_element_content_set(transaction, &addr, stream));

  constexpr sc_uint32 addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  sc_iterator * it = sc_list_iterator(transaction->transaction_buffer->content_changes);
  sc_bool found = SC_FALSE;
  while (sc_iterator_next(it))
  {
    sc_pair * pair = static_cast<sc_pair *>(sc_iterator_get(it));
    if (reinterpret_cast<uintptr_t>(pair->first) == addr_hash)
    {
      EXPECT_EQ(pair->second, stream);
      found = SC_TRUE;
      break;
    }
  }
  sc_iterator_destroy(it);
  EXPECT_TRUE(found);

  EXPECT_FALSE(sc_transaction_element_content_set(nullptr, &addr, stream));
  EXPECT_FALSE(sc_transaction_element_content_set(transaction, nullptr, stream));
  EXPECT_FALSE(sc_transaction_element_content_set(transaction, &addr, nullptr));
}

TEST_F(ScTransactionTest, TransactionValidation)
{
  EXPECT_FALSE(sc_transaction_validate(transaction));
}

TEST_F(ScTransactionTest, TransactionEdgeCases)
{
  sc_transaction_destroy(nullptr);

  sc_transaction * failed_txn = sc_transaction_new(0, m_ctx->GetRealContext());
  EXPECT_NE(failed_txn, nullptr);
}