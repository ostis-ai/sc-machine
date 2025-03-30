#include <gtest/gtest.h>
#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include <sc-store/sc-transaction/sc_transaction_manager.h>
#include <sc-store/sc-transaction/sc_transaction.h>
#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-store/sc_element.h>
#include <sc-store/sc-container/sc_struct_node.h>
#include <sc-store/sc_storage.h>
#include <sc_memory_context_private.h>
}

class ScTransactionManagerTest : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::Initialize();
    m_ctx = std::make_unique<ScAgentContext>();

    sc_transaction_manager_initialize();
  }

  void TearDown() override
  {
    sc_transaction_shutdown();

    if (m_ctx)
      m_ctx->Destroy();

    ScMemoryTest::Shutdown();
  }
};

TEST_F(ScTransactionManagerTest, InitializationTest)
{
  EXPECT_TRUE(sc_transaction_manager_is_initialized());
}

TEST_F(ScTransactionManagerTest, TransactionCreationTest)
{
  sc_transaction * txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr) << "Transaction creation failed";
  EXPECT_EQ(txn->transaction_id, 0u);
  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, TransactionCounterIncrementsTest)
{
  sc_transaction * txn1 = sc_transaction_manager_transaction_new();
  sc_transaction * txn2 = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn1, null_ptr);
  ASSERT_NE(txn2, null_ptr);
  EXPECT_EQ(txn1->transaction_id, 0u);
  EXPECT_EQ(txn2->transaction_id, 1u);
  sc_transaction_destroy(txn1);
  sc_transaction_destroy(txn2);
}

TEST_F(ScTransactionManagerTest, ShutdownTest)
{
  sc_transaction_shutdown();
  EXPECT_FALSE(sc_transaction_manager_is_initialized());
}

TEST_F(ScTransactionManagerTest, TransactionBufferInitializationTest)
{
  sc_transaction * txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);

  ASSERT_NE(txn->transaction_buffer, null_ptr);
  ASSERT_NE(txn->transaction_buffer->new_elements, null_ptr);
  ASSERT_NE(txn->transaction_buffer->modified_elements, null_ptr);
  ASSERT_NE(txn->transaction_buffer->deleted_elements, null_ptr);

  EXPECT_EQ(txn->transaction_buffer->new_elements->size, 0u);
  EXPECT_EQ(txn->transaction_buffer->modified_elements->size, 0u);
  EXPECT_EQ(txn->transaction_buffer->deleted_elements->size, 0u);

  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, TransactionMemoryCleanupTest)
{
  sc_transaction * txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);

  sc_element elem = {0};
  sc_list_push_back(txn->transaction_buffer->new_elements, &elem);
  sc_list_push_back(txn->transaction_buffer->modified_elements, &elem);
  sc_list_push_back(txn->transaction_buffer->deleted_elements, &elem);

  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, InvalidTransactionCreationTest)
{
  sc_transaction_shutdown();
  sc_transaction * txn = sc_transaction_manager_transaction_new();
  EXPECT_EQ(txn, null_ptr);
  sc_transaction_manager_initialize();
}

TEST_F(ScTransactionManagerTest, TransactionIdSequenceTest)
{
  constexpr sc_uint32 iterations = 100;
  sc_transaction * txns[iterations];
  for (sc_uint32 i = 0; i < iterations; ++i)
  {
    txns[i] = sc_transaction_manager_transaction_new();
    ASSERT_NE(txns[i], null_ptr);
    EXPECT_EQ(txns[i]->transaction_id, i);
  }
  for (sc_uint32 i = 0; i < iterations; ++i)
    sc_transaction_destroy(txns[i]);
}

TEST_F(ScTransactionManagerTest, AddCreatedElementSuccess)
{
  sc_transaction * txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);

  sc_addr const & addr = sc_storage_node_new(m_ctx.get()->GetRealContext(), sc_type_const_node);

  EXPECT_TRUE(sc_transaction_buffer_created_add(txn->transaction_buffer, addr));
  EXPECT_EQ(txn->transaction_buffer->new_elements->size, 1u);

  sc_struct_node * node = sc_list_front(txn->transaction_buffer->new_elements);
  ASSERT_NE(node, null_ptr);

  sc_addr const * stored_addr = static_cast<sc_addr *>(node->data);
  ASSERT_NE(stored_addr, null_ptr);

  EXPECT_EQ(stored_addr->seg, addr.seg);
  EXPECT_EQ(stored_addr->offset, addr.offset);

  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, AddCreatedDuplicateElement)
{
  sc_transaction * txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);

  sc_addr const & addr = sc_storage_node_new(m_ctx.get()->GetRealContext(), sc_type_const_node);

  EXPECT_TRUE(sc_transaction_buffer_created_add(txn->transaction_buffer, addr));
  EXPECT_TRUE(sc_transaction_buffer_created_add(txn->transaction_buffer, addr));
  EXPECT_EQ(txn->transaction_buffer->new_elements->size, 1u);

  sc_transaction_destroy(txn);
}

TEST_F(ScTransactionManagerTest, AddCreatedInvalidAddress)
{
  sc_transaction * txn = sc_transaction_manager_transaction_new();
  ASSERT_NE(txn, null_ptr);

  EXPECT_FALSE(sc_transaction_buffer_created_add(txn->transaction_buffer, SC_ADDR_EMPTY));
  EXPECT_FALSE(sc_transaction_buffer_created_add(static_cast<sc_transaction_buffer *> null_ptr, SC_ADDR_EMPTY));

  if (txn->transaction_buffer->new_elements)
    EXPECT_EQ(txn->transaction_buffer->new_elements->size, 0u);

  sc_transaction_destroy(txn);
}
