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

void _test_sc_dictionary_addr_hashes_char_to_int(sc_char ch, sc_uint8 * ch_num, const sc_uint8 * mask)
{
  *ch_num = 128 + (sc_uint8)ch;
}

sc_bool _test_sc_hashes_compare(void * hash, void * other)
{
  return SC_ADDR_IS_EQUAL(*(sc_addr *)hash, *(sc_addr *)other);
}

TEST_F(ScDictionaryTest, smoke)
{
  sc_dictionary * dictionary = nullptr;
  EXPECT_TRUE(sc_dictionary_initialize(
      &dictionary,
      _test_sc_dictionary_addr_hashes_children_size(),
      _test_sc_dictionary_addr_hashes_char_to_int));

  sc_addr addr1 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str1 = "concept_set1";
  sc_dictionary_append(dictionary, str1, strlen(str1), &addr1);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));

  sc_addr addr2 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str2 = "concept_concept";
  sc_dictionary_append(dictionary, str2, strlen(str2), &addr2);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));

  sc_addr addr3 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str3 = "norole_relation";
  sc_dictionary_append(dictionary, str3, strlen(str3), &addr3);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str3, strlen(str3)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));

  sc_addr addr4 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str4 = "node_role_relation";
  sc_dictionary_append(dictionary, str4, strlen(str4), &addr4);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str4, strlen(str4)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str3, strlen(str3)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));

  auto const checkAfterRemove
      = [dictionary](
            sc_char const * stringToRemove,
            sc_addr const addrToRemove,
            std::unordered_map<sc_char const *, sc_addr> const & toCheckExistence,
            std::unordered_map<sc_char const *, sc_addr> const & toCheckNonExistence) {
    auto const checkExistence = [dictionary](sc_char const * string, sc_addr addr) {
      EXPECT_TRUE(sc_dictionary_has(dictionary, string, strlen(string)));
      auto * list = (sc_list *)sc_dictionary_get_by_key(dictionary, string, strlen(string));
      sc_iterator * it = sc_list_iterator(list);
      EXPECT_TRUE(sc_iterator_next(it));
      EXPECT_TRUE(SC_ADDR_IS_EQUAL(*(sc_addr *)sc_iterator_get(it), addr));
      EXPECT_FALSE(sc_iterator_next(it));
      EXPECT_TRUE(sc_iterator_destroy(it));
    };

    for (auto const & item : toCheckExistence)
    {
      checkExistence(item.first, item.second);
    }

    EXPECT_TRUE(sc_dictionary_remove(dictionary, stringToRemove, strlen(stringToRemove), (void *)&addrToRemove, _test_sc_hashes_compare));

    auto const checkNonExistence = [dictionary](sc_char const * string, sc_addr addr) {
      EXPECT_FALSE(sc_dictionary_has(dictionary, string, strlen(string)));
      auto * list = (sc_list *)sc_dictionary_get_by_key(dictionary, string, strlen(string));
      sc_iterator * it = sc_list_iterator(list);
      EXPECT_FALSE(sc_iterator_next(it));
      EXPECT_TRUE(it == nullptr || sc_iterator_destroy(it));
    };

    for (auto const & item : toCheckNonExistence)
    {
      checkNonExistence(item.first, item.second);
    }
  };

  checkAfterRemove(str1, addr1, {{str2, addr2}, {str3, addr3}, {str4, addr4}}, {{str1, addr1}});
  checkAfterRemove(str2, addr2, {{str3, addr3}, {str4, addr4}}, {{str1, addr1}, {str2, addr2}});
  checkAfterRemove(str3, addr3, {{str4, addr4}}, {{str1, addr1}, {str2, addr2}, {str3, addr3}});
  checkAfterRemove(str4, addr4, {}, {{str1, addr1}, {str2, addr2}, {str3, addr3}, {str4, addr4}});

  sc_dictionary_destroy(dictionary, sc_dictionary_node_destroy);
}

TEST_F(ScDictionaryTest, CorrespondenceCheck)
{
  sc_dictionary * dictionary = nullptr;
  EXPECT_TRUE(sc_dictionary_initialize(
    &dictionary,
    _test_sc_dictionary_addr_hashes_children_size(),
    _test_sc_dictionary_addr_hashes_char_to_int));

  sc_addr addr1 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str1 = "users_1";
  sc_dictionary_append(dictionary, str1, strlen(str1), &addr1);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_1_1", strlen("users_1_1")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_1_2", strlen("users_1_2")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2", strlen("users_2")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_1", strlen("users_2_1")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_2", strlen("users_2_2")));

  sc_addr addr2 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str2 = "users_1_1";
  sc_dictionary_append(dictionary, str2, strlen(str2), &addr2);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_1_2", strlen("users_1_2")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2", strlen("users_2")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_1", strlen("users_2_1")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_2", strlen("users_2_2")));

  sc_addr addr3 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str3 = "users_1_2";
  sc_dictionary_append(dictionary, str3, strlen(str3), &addr3);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str3, strlen(str3)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2", strlen("users_2")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_1", strlen("users_2_1")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_2", strlen("users_2_2")));

  sc_addr addr4 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str4 = "users_2";
  sc_dictionary_append(dictionary, str4, strlen(str4), &addr4);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str4, strlen(str4)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str3, strlen(str3)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_1", strlen("users_2_1")));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_2", strlen("users_2_2")));

  sc_addr addr5 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str5 = "users_2_1";
  sc_dictionary_append(dictionary, str5, strlen(str5), &addr5);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str5, strlen(str5)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str4, strlen(str4)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str3, strlen(str3)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));
  EXPECT_FALSE(sc_dictionary_has(dictionary, "users_2_2", strlen("users_2_2")));

  sc_addr addr6 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str6 = "users_2_2";
  sc_dictionary_append(dictionary, str6, strlen(str6), &addr6);
  EXPECT_TRUE(sc_dictionary_has(dictionary, str6, strlen(str6)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str5, strlen(str5)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str4, strlen(str4)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str3, strlen(str3)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str2, strlen(str2)));
  EXPECT_TRUE(sc_dictionary_has(dictionary, str1, strlen(str1)));

  auto * list = (sc_list *)sc_dictionary_get_by_key(dictionary, str1, strlen(str1));
  sc_iterator * it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_addr const linkAddr = *(sc_addr *)sc_iterator_get(it);
    EXPECT_TRUE(addr1.seg == linkAddr.seg);
    EXPECT_TRUE(addr1.offset == linkAddr.offset);
  }
  sc_iterator_destroy(it);

  list = (sc_list *)sc_dictionary_get_by_key(dictionary, str2, strlen(str2));
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_addr const linkAddr = *(sc_addr *)sc_iterator_get(it);
    EXPECT_TRUE(addr2.seg == linkAddr.seg);
    EXPECT_TRUE(addr2.offset == linkAddr.offset);
  }
  sc_iterator_destroy(it);

  list = (sc_list *)sc_dictionary_get_by_key(dictionary, str3, strlen(str3));
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_addr const linkAddr = *(sc_addr *)sc_iterator_get(it);
    EXPECT_TRUE(addr3.seg == linkAddr.seg);
    EXPECT_TRUE(addr3.offset == linkAddr.offset);
  }
  sc_iterator_destroy(it);

  list = (sc_list *)sc_dictionary_get_by_key(dictionary, str4, strlen(str4));
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_addr const linkAddr = *(sc_addr *)sc_iterator_get(it);
    EXPECT_TRUE(addr4.seg == linkAddr.seg);
    EXPECT_TRUE(addr4.offset == linkAddr.offset);
  }
  sc_iterator_destroy(it);

  list = (sc_list *)sc_dictionary_get_by_key(dictionary, str5, strlen(str5));
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_addr const linkAddr = *(sc_addr *)sc_iterator_get(it);
    EXPECT_TRUE(addr5.seg == linkAddr.seg);
    EXPECT_TRUE(addr5.offset == linkAddr.offset);
  }
  sc_iterator_destroy(it);

  list = (sc_list *)sc_dictionary_get_by_key(dictionary, str6, strlen(str6));
  it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_addr const linkAddr = *(sc_addr *)sc_iterator_get(it);
    EXPECT_TRUE(addr6.seg == linkAddr.seg);
    EXPECT_TRUE(addr6.offset == linkAddr.offset);
  }
  sc_iterator_destroy(it);

  sc_dictionary_destroy(dictionary, sc_dictionary_node_destroy);
}
