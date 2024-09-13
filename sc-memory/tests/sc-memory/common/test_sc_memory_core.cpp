#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc_memory.h"
#include "sc-core/sc-store/sc-container/sc-string/sc_string.h"
}

#include "sc_test.hpp"

TEST_F(ScMemoryTest, sc_memory_find_links_with_content_string)
{
  sc_memory_context * context = **m_ctx;
  sc_addr const link_addr = sc_memory_link_new2(context, sc_type_link | sc_type_const);

  sc_char content[] = "content";
  sc_stream * stream = sc_stream_memory_new(content, sc_str_len(content), SC_STREAM_FLAG_READ, SC_FALSE);
  EXPECT_EQ(sc_memory_set_link_content(context, link_addr, stream), SC_RESULT_OK);

  sc_list * link_hashes;
  sc_memory_find_links_with_content_string(context, stream, &link_hashes);
  sc_iterator * it = sc_list_iterator(link_hashes);
  while (sc_iterator_next(it))
  {
    auto found_link_hash = (sc_pointer_to_sc_addr_hash)sc_iterator_get(it);
    sc_addr found_addr;
    SC_ADDR_LOCAL_FROM_INT(found_link_hash, found_addr);
    EXPECT_TRUE(SC_ADDR_IS_EQUAL(found_addr, link_addr));
  }
  sc_iterator_destroy(it);

  sc_stream_free(stream);
  sc_list_destroy(link_hashes);
}

TEST_F(ScMemoryTest, sc_memory_find_links_by_content_substring)
{
  sc_memory_context * context = **m_ctx;
  sc_addr const link_addr = sc_memory_link_new2(context, sc_type_link | sc_type_const);

  sc_char content[] = "context";
  sc_stream * stream = sc_stream_memory_new(content, sc_str_len(content), SC_STREAM_FLAG_READ, SC_FALSE);
  EXPECT_EQ(sc_memory_set_link_content(context, link_addr, stream), SC_RESULT_OK);

  sc_list * link_hashes;
  sc_memory_find_links_by_content_substring(context, stream, 1, &link_hashes);
  sc_iterator * it = sc_list_iterator(link_hashes);
  while (sc_iterator_next(it))
  {
    auto found_link_hash = (sc_pointer_to_sc_addr_hash)sc_iterator_get(it);
    sc_addr found_addr;
    SC_ADDR_LOCAL_FROM_INT(found_link_hash, found_addr);
    EXPECT_TRUE(SC_ADDR_IS_EQUAL(found_addr, link_addr));
  }
  sc_iterator_destroy(it);

  sc_stream_free(stream);
  sc_list_destroy(link_hashes);
}

TEST_F(ScMemoryTest, sc_memory_find_links_contents_by_content_substring)
{
  sc_memory_context * context = **m_ctx;
  sc_addr const link_addr = sc_memory_link_new2(context, sc_type_link | sc_type_const);

  sc_char content[] = "context";
  sc_stream * stream = sc_stream_memory_new(content, sc_str_len(content), SC_STREAM_FLAG_READ, SC_FALSE);
  EXPECT_EQ(sc_memory_set_link_content(context, link_addr, stream), SC_RESULT_OK);

  sc_list * string_hashes;
  sc_memory_find_links_contents_by_content_substring(context, stream, 1, &string_hashes);
  sc_iterator * it = sc_list_iterator(string_hashes);
  while (sc_iterator_next(it))
  {
    auto * found_string = (sc_char *)sc_iterator_get(it);
    EXPECT_TRUE(sc_str_cmp(found_string, content));
  }
  sc_iterator_destroy(it);

  sc_stream_free(stream);
  sc_list_clear(string_hashes);
  sc_list_destroy(string_hashes);
}

TEST_F(ScMemoryTest, sc_event_subscription_invalid)
{
  sc_memory_context * context = **m_ctx;
  sc_addr const subscription_addr = sc_memory_node_new(context, sc_type_const_node);

  EXPECT_EQ(sc_event_subscription_new(context, subscription_addr, SC_ADDR_EMPTY, nullptr, nullptr, nullptr), nullptr);
  EXPECT_EQ(sc_event_subscription_new(context, SC_ADDR_EMPTY, subscription_addr, nullptr, nullptr, nullptr), nullptr);

  EXPECT_EQ(
      sc_event_subscription_with_user_new(context, subscription_addr, SC_ADDR_EMPTY, 0, nullptr, nullptr, nullptr),
      nullptr);
  EXPECT_EQ(
      sc_event_subscription_with_user_new(context, SC_ADDR_EMPTY, subscription_addr, 0, nullptr, nullptr, nullptr),
      nullptr);
}
