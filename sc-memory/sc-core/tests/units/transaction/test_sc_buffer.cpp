#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include <sc-store/sc_storage_private.h>
#include <sc-store/sc-transaction/sc_transaction_buffer.h>
#include <sc-store/sc_element.h>
#include <sc-core/sc_stream.h>
#include <sc-store/sc-container/sc_struct_node.h>
#include <sc-core/sc_stream_memory.h>
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-store/sc-base/sc_monitor_table.h>
}

class ScTransactionBufferTest : public ScMemoryTest
{
protected:
  sc_transaction_buffer * buffer = sc_mem_new(sc_transaction_buffer, 1);

  void SetUp() override
  {
    ScMemoryTest::SetUp();
    ASSERT_TRUE(sc_transaction_buffer_initialize(buffer, 1));
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }
};

TEST_F(ScTransactionBufferTest, InitializationTest)
{
  EXPECT_NE(buffer->new_elements, nullptr);
  EXPECT_NE(buffer->modified_elements, nullptr);
  EXPECT_NE(buffer->deleted_elements, nullptr);
  EXPECT_NE(buffer->content_changes, nullptr);
  EXPECT_NE(buffer->monitor_table, nullptr);
  EXPECT_EQ(buffer->transaction_id, 1u);
}

TEST_F(ScTransactionBufferTest, CreatedAddTest)
{
  sc_addr addr = {1, 1};

  EXPECT_TRUE(sc_transaction_buffer_created_add(buffer, &addr));
  EXPECT_EQ(buffer->new_elements->size, 1u);

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  EXPECT_EQ(buffer->new_elements->begin->data, reinterpret_cast<void *>(addr_hash));

  EXPECT_TRUE(sc_transaction_buffer_contains_created(buffer, &addr));
}

TEST_F(ScTransactionBufferTest, RemovedAddTest)
{
  sc_addr addr = {1, 1};

  EXPECT_TRUE(sc_transaction_buffer_removed_add(buffer, &addr));
  EXPECT_EQ(buffer->deleted_elements->size, 1u);

  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  EXPECT_EQ(buffer->deleted_elements->begin->data, reinterpret_cast<void *>(addr_hash));
}

TEST_F(ScTransactionBufferTest, ModifiedAddTest)
{
  sc_addr * addr = sc_mem_new(sc_addr, 1);
  sc_storage_allocate_new_element(m_ctx->GetRealContext(), addr);

  sc_element * element = sc_mem_new(sc_element, 1);
  sc_storage_get_element_by_addr(*addr, &element);

  element->incoming_arcs_count++;

  constexpr SC_ELEMENT_MODIFIED_FLAGS flags = SC_ELEMENT_ARCS_MODIFIED;

  EXPECT_TRUE(sc_transaction_buffer_modified_add(buffer, addr, element, flags));
  EXPECT_EQ(buffer->modified_elements->size, 1);
  sc_uint32 const addr_hash = SC_ADDR_LOCAL_TO_INT(*addr);
  EXPECT_EQ(buffer->modified_elements->begin->data, reinterpret_cast<void *>(addr_hash));
}

TEST_F(ScTransactionBufferTest, ContentSetTest)
{
  sc_addr addr = {1, 4};
  sc_stream * stream = sc_stream_memory_new("test", 4, SC_STREAM_FLAG_READ, SC_FALSE);
  ASSERT_NE(stream, nullptr);

  EXPECT_TRUE(sc_transaction_buffer_content_set(buffer, &addr, stream));
  EXPECT_EQ(buffer->content_changes->size, 1u);

  sc_stream_free(stream);
}
