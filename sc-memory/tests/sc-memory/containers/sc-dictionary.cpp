#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-container/sc-dictionary/sc_dictionary.h"
}

#include "sc_test.hpp"

using ScDictionaryTest = ScMemoryTest;

TEST_F(ScDictionaryTest, smoke)
{
  sc_dictionary * dictionary = nullptr;
  EXPECT_TRUE(sc_dictionary_initialize(&dictionary));

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

TEST_F(ScDictionaryTest, find_by_substr)
{
  sc_dictionary * dictionary = nullptr;
  EXPECT_TRUE(sc_dictionary_initialize(&dictionary));

  sc_addr addr1 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str1 = "letter_big";
  sc_dictionary_append(dictionary, str1, strlen(str1), &addr1);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr2 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str2 = "letter_small";
  sc_dictionary_append(dictionary, str2, strlen(str2), &addr2);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));

  sc_addr addr3 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str3 = "let_out";
  sc_dictionary_append(dictionary, str3, strlen(str3), &addr3);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str3));

  sc_addr addr4 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char * str4 = "letters";
  sc_dictionary_append(dictionary, str4, strlen(str4), &addr4);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str4));

  sc_list * list = sc_dictionary_get_by_substr(dictionary, "let");
  sc_iterator * it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    auto const addr = SC_ADDR_LOCAL_TO_INT(*(sc_addr*)sc_iterator_get(it));
    EXPECT_TRUE(SC_ADDR_LOCAL_TO_INT(addr1) == addr
                || SC_ADDR_LOCAL_TO_INT(addr2) == addr
                || SC_ADDR_LOCAL_TO_INT(addr3) == addr
                || SC_ADDR_LOCAL_TO_INT(addr4) == addr);
  }
  sc_iterator_destroy(it);
  sc_list_destroy(list);

  list = sc_dictionary_get_by_substr(dictionary, "letter");
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    auto const addr = SC_ADDR_LOCAL_TO_INT(*(sc_addr*)sc_iterator_get(it));
    EXPECT_TRUE(SC_ADDR_LOCAL_TO_INT(addr1) == addr
                || SC_ADDR_LOCAL_TO_INT(addr2) == addr
                || SC_ADDR_LOCAL_TO_INT(addr4) == addr);
  }
  sc_iterator_destroy(it);
  sc_list_destroy(list);

  list = sc_dictionary_get_by_substr(dictionary, "lett");
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    auto const addr = SC_ADDR_LOCAL_TO_INT(*(sc_addr*)sc_iterator_get(it));
    EXPECT_TRUE(SC_ADDR_LOCAL_TO_INT(addr1) == addr
                || SC_ADDR_LOCAL_TO_INT(addr2) == addr
                || SC_ADDR_LOCAL_TO_INT(addr4) == addr);
  }
  sc_iterator_destroy(it);
  sc_list_destroy(list);

  list = sc_dictionary_get_by_substr(dictionary, "letters");
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    auto const addr = SC_ADDR_LOCAL_TO_INT(*(sc_addr*)sc_iterator_get(it));
    EXPECT_TRUE(SC_ADDR_LOCAL_TO_INT(addr4) == addr);
  }
  sc_iterator_destroy(it);
  sc_list_destroy(list);
}
