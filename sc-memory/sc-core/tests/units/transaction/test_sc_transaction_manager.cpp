#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include "sc-store/sc-base/sc_thread.h"
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-store/sc-transaction/sc_memory_transaction_manager.h>
}

class ScTransactionManagerTest : public ScMemoryTest
{
protected:
  sc_memory_transaction_manager * transaction_manager = nullptr;

  void SetUp() override
  {
    ScMemoryTest::SetUp();
    transaction_manager = sc_memory_transaction_manager_get();
    if (!sc_memory_transaction_manager_is_initialized())
    {
      sc_memory_transaction_manager * manager = sc_mem_new(sc_memory_transaction_manager, 1);
      sc_memory_transaction_manager_initialize(manager);
    }
    transaction_manager = sc_memory_transaction_manager_get();
  }

  void TearDown() override
  {
    sc_memory_transaction_shutdown();
    ScMemoryTest::TearDown();
  }
};

// Test Initialization
TEST_F(ScTransactionManagerTest, Initialization)
{
  ASSERT_NE(transaction_manager, nullptr);
  EXPECT_TRUE(sc_memory_transaction_manager_is_initialized());
}

// Test Shutdown
TEST_F(ScTransactionManagerTest, Shutdown)
{
  sc_memory_transaction_shutdown();
  EXPECT_FALSE(sc_memory_transaction_manager_is_initialized());
  EXPECT_EQ(sc_memory_transaction_manager_get(), nullptr);
}

// Test Transaction Creation
TEST_F(ScTransactionManagerTest, TransactionNew)
{
  sc_transaction * txn1 = sc_memory_transaction_new(m_ctx->GetRealContext());
  EXPECT_NE(txn1, nullptr);
  EXPECT_EQ(txn1->transaction_id, 0); // First transaction should have ID 0

  sc_transaction * txn2 = sc_memory_transaction_new(m_ctx->GetRealContext());
  EXPECT_NE(txn2, nullptr);
  EXPECT_EQ(txn2->transaction_id, 1); // Second transaction should have ID 1
}

// Test Transaction Addition to Queue
TEST_F(ScTransactionManagerTest, TransactionAdd)
{
  sc_transaction * txn = sc_memory_transaction_new(m_ctx->GetRealContext());
  EXPECT_NE(txn, nullptr);
  sc_transaction_manager_transaction_add(txn);

  sc_monitor_acquire_read(transaction_manager->monitor);
  EXPECT_EQ(transaction_manager->transaction_queue->size, 1);
  auto * queued_txn = static_cast<sc_transaction *>(sc_queue_front(transaction_manager->transaction_queue));
  EXPECT_EQ(queued_txn, txn);
  sc_monitor_release_read(transaction_manager->monitor);
}

// Test Transaction Processing
TEST_F(ScTransactionManagerTest, TransactionProcessing)
{
  sc_transaction * txn = sc_memory_transaction_new(m_ctx->GetRealContext());
  EXPECT_NE(txn, nullptr);

  // Add an element to txn->elements to simulate real usage
  sc_hash_table_insert(txn->elements, (void*)1, (void*)1);

  sc_transaction_manager_transaction_add(txn);

  // Wait for transaction to be processed
  while (transaction_manager->transaction_queue->size > 0)
  {
    sc_thread_sleep(10); // Sleep for 10ms
  }

  EXPECT_EQ(txn->state, SC_TRANSACTION_EXECUTED);
}

// Test Element Intersection
TEST_F(ScTransactionManagerTest, ElementIntersection)
{
  sc_transaction * txn = sc_memory_transaction_new(m_ctx->GetRealContext());
  EXPECT_NE(txn, nullptr);

  // Add intersecting element
  sc_hash_table_insert(txn->elements, (void*)1, (void*)1);
  sc_hash_table_insert(transaction_manager->processed_elements, (void*)1, (void*)1);

  EXPECT_TRUE(sc_transaction_elements_intersect(txn, transaction_manager->processed_elements));

  // Remove intersection
  sc_hash_table_remove(txn->elements, (void*)1);
  EXPECT_FALSE(sc_transaction_elements_intersect(txn, transaction_manager->processed_elements));

  sc_hash_table_destroy(transaction_manager->processed_elements);
}

// Test Thread Safety with Multiple Transactions
TEST_F(ScTransactionManagerTest, ThreadSafety)
{
  constexpr int num_transactions = 10;
  sc_transaction * transactions[num_transactions];

  for (int i = 0; i < num_transactions; ++i)
  {
    transactions[i] = sc_memory_transaction_new(m_ctx->GetRealContext());
    EXPECT_NE(transactions[i], nullptr);
    sc_hash_table_insert(transactions[i]->elements, (void*)(i + 1), (void*)(i + 1)); // Unique element per transaction
    sc_transaction_manager_transaction_add(transactions[i]);
  }

  // Wait for all transactions to be processed
  while (transaction_manager->transaction_queue->size > 0)
  {
    sc_thread_sleep(10); // Sleep for 10ms
  }

  for (int i = 0; i < num_transactions; ++i)
  {
    EXPECT_EQ(transactions[i]->state, SC_TRANSACTION_EXECUTED);
  }
}

// Test Transaction Manager Destroy
TEST_F(ScTransactionManagerTest, TransactionManagerDestroy)
{
  sc_memory_transaction_shutdown();
  EXPECT_FALSE(sc_memory_transaction_manager_is_initialized());
  EXPECT_EQ(sc_memory_transaction_manager_get(), nullptr);
}