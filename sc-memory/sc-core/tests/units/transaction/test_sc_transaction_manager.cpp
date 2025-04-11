#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sc-memory/test/sc_test.hpp>

extern "C"
{
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
  }

  void TearDown() override
  {
    sc_memory_transaction_shutdown();
    ScMemoryTest::TearDown();
  }
};

TEST_F(ScTransactionManagerTest, Initialization)
{
  ASSERT_NE(transaction_manager, nullptr);
  EXPECT_TRUE(sc_memory_transaction_manager_is_initialized());
}

TEST_F(ScTransactionManagerTest, Shutdown)
{
  sc_memory_transaction_shutdown();
  EXPECT_FALSE(sc_memory_transaction_manager_is_initialized());
}

TEST_F(ScTransactionManagerTest, TransactionNew)
{
  sc_transaction *txn = sc_memory_transaction_new();
  EXPECT_NE(txn, nullptr);
}

TEST_F(ScTransactionManagerTest, TransactionAdd)
{
  sc_transaction *txn = sc_memory_transaction_new();
  EXPECT_NE(txn, nullptr);
  sc_transaction_manager_transaction_add(txn);

  sc_monitor_acquire_read(transaction_manager->monitor);
  EXPECT_EQ(transaction_manager->transaction_queue->size, 1);
  sc_monitor_release_read(transaction_manager->monitor);
}

TEST_F(ScTransactionManagerTest, TransactionManagerDestroy)
{
  sc_memory_transaction_shutdown();
  EXPECT_FALSE(sc_memory_transaction_manager_is_initialized());
}
