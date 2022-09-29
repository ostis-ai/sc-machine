#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-container/sc-dictionary/sc_dictionary.h"
}

#include "sc_test.hpp"

using ScDictionaryTest = ScMemoryTest;

sc_uint8 _test_sc_dictionary_addr_hashes_children_size()
{
  const sc_uint8 max_sc_char = 255;
  const sc_uint8 min_sc_char = 1;

  return max_sc_char - min_sc_char + 1;
}

void _test_sc_dictionary_addr_hashes_char_to_int(sc_char ch, sc_uint8 * ch_num, sc_uint8 * mask)
{
  *ch_num = 128 + (sc_uint8)ch;
}

void _test_sc_dictionary_addr_hashes_int_to_char(sc_uint8 num, sc_uint8 mask, sc_char * ch)
{
  *ch = (sc_char)(num - 128);
}

TEST_F(ScDictionaryTest, smoke)
{
  sc_dictionary * dictionary = nullptr;
  EXPECT_TRUE(sc_dictionary_initialize(
      &dictionary,
      _test_sc_dictionary_addr_hashes_children_size(),
      _test_sc_dictionary_addr_hashes_char_to_int,
      _test_sc_dictionary_addr_hashes_int_to_char));

  sc_addr addr1 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str1 = "concept_set1";
  sc_dictionary_append(dictionary, str1, strlen(str1), &addr1);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr2 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str2 = "concept_letter";
  sc_dictionary_append(dictionary, str2, strlen(str2), &addr2);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr3 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str3 = "sc_node_norole_relation";
  sc_dictionary_append(dictionary, str3, strlen(str3), &addr3);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr4 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str4 = "sc_node_role_relation";
  sc_dictionary_append(dictionary, str4, strlen(str4), &addr4);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str4));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  EXPECT_TRUE(sc_dictionary_remove(dictionary, str4));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str4));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  EXPECT_TRUE(sc_dictionary_remove(dictionary, str3));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str4));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  EXPECT_TRUE(sc_dictionary_remove(dictionary, str2));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str4));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  EXPECT_TRUE(sc_dictionary_remove(dictionary, str1));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str4));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_FALSE(sc_dictionary_is_in(dictionary, str1));

  sc_dictionary_destroy(dictionary, sc_dictionary_node_destroy);
}
