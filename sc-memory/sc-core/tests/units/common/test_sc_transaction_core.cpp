#include "sc-memory/test/sc_test.hpp"
#include "sc-store/sc_element.h"

#include <gtest/gtest.h>

extern "C" {
#include <sc-store/sc-transaction/sc_transaction_manager.h>
#include <sc-store/sc-transaction/sc_transaction.h>
}

#ifndef null_ptr
#define null_ptr NULL
#endif

class ScTransactionManagerTest : public ScMemoryTest {
protected:
  void SetUp() override {
    sc_transaction_manager_initialize();
  }

  void TearDown() override {
    sc_transaction_shutdown();
  }
};

TEST_F(ScTransactionManagerTest, InitializationTest) {
  EXPECT_TRUE(sc_transaction_manager_is_initialized());
}

TEST_F(ScTransactionManagerTest, TransactionCreationTest) {
  sc_transaction* txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);
  EXPECT_EQ(txn->transaction_id, 0u);
  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, TransactionCounterIncrementsTest) {
  sc_transaction* txn1 = sc_transaction_manager_transaction_new();
  sc_transaction* txn2 = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn1, null_ptr);
  ASSERT_NE(txn2, null_ptr);
  EXPECT_EQ(txn1->transaction_id, 0u);
  EXPECT_EQ(txn2->transaction_id, 1u);
  sc_transaction_destroy(txn1);
  sc_transaction_destroy(txn2);
}

TEST_F(ScTransactionManagerTest, ShutdownTest) {
  sc_transaction_shutdown();
  EXPECT_FALSE(sc_transaction_manager_is_initialized());
}

TEST_F(ScTransactionManagerTest, TransactionBufferInitializationTest) {
  sc_transaction* txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);
  ASSERT_NE(txn->transaction_buffer, null_ptr);
  EXPECT_NE(txn->transaction_buffer->new_elements, null_ptr);
  EXPECT_NE(txn->transaction_buffer->modified_elements, null_ptr);
  EXPECT_NE(txn->transaction_buffer->deleted_elements, null_ptr);
  EXPECT_TRUE(txn->transaction_buffer->new_elements->size == 0u);
  EXPECT_TRUE(txn->transaction_buffer->modified_elements->size == 0u);
  EXPECT_TRUE(txn->transaction_buffer->deleted_elements->size == 0u);
  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, TransactionMemoryCleanupTest) {
  sc_transaction* txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);
  sc_element elem = {0};
  sc_list_push_back(txn->transaction_buffer->new_elements, &elem);
  sc_list_push_back(txn->transaction_buffer->modified_elements, &elem);
  sc_list_push_back(txn->transaction_buffer->deleted_elements, &elem);
  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, InvalidTransactionCreationTest) {
  sc_transaction_shutdown();
  sc_transaction* txn = sc_transaction_manager_transaction_new();
  EXPECT_EQ(txn, null_ptr);
  sc_transaction_manager_initialize();
}

TEST_F(ScTransactionManagerTest, BufferOperationsTest) {
  sc_transaction* txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);
  sc_element elem1 = {.flags = {.type = sc_type_node}};
  sc_element elem2 = {.flags = {.type = sc_type_arc}};
  sc_list_push_back(txn->transaction_buffer->new_elements, &elem1);
  EXPECT_EQ(txn->transaction_buffer->new_elements->size, 1u);
  sc_list_push_back(txn->transaction_buffer->modified_elements, &elem2);
  EXPECT_EQ(txn->transaction_buffer->modified_elements->size, 1u);
  sc_list_push_back(txn->transaction_buffer->deleted_elements, &elem1);
  EXPECT_EQ(txn->transaction_buffer->deleted_elements->size, 1u);
  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, TransactionIdSequenceTest) {
  constexpr sc_uint32 iterations = 100;
  sc_transaction* txns[iterations];
  for (sc_uint32 i = 0; i < iterations; ++i) {
    txns[i] = sc_transaction_manager_transaction_new();
    ASSERT_NE(txns[i], null_ptr);
    EXPECT_EQ(txns[i]->transaction_id, i);
  }
  for (sc_uint32 i = 0; i < iterations; ++i)
    sc_transaction_destroy(txns[i]);
}
