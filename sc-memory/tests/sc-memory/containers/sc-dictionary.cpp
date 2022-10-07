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
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr2 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str2 = "concept_concept";
  sc_dictionary_append(dictionary, str2, strlen(str2), &addr2);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr3 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str3 = "norole_relation";
  sc_dictionary_append(dictionary, str3, strlen(str3), &addr3);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr4 = sc_memory_link_new(m_ctx->GetRealContext());
  sc_char const * str4 = "node_role_relation";
  sc_dictionary_append(dictionary, str4, strlen(str4), &addr4);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str4));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  auto const checkAfterRemove
      = [dictionary](
            sc_char const * stringToRemove,
            std::unordered_map<sc_char const *, sc_addr> const & toCheckExistence,
            std::unordered_map<sc_char const *, sc_addr> const & toCheckNonExistence) {
    auto const checkExistence = [dictionary](sc_char const * string, sc_addr addr) {
      EXPECT_TRUE(sc_dictionary_is_in(dictionary, string));
      sc_list * list = sc_dictionary_get(dictionary, string);
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

    EXPECT_TRUE(sc_dictionary_remove(dictionary, stringToRemove));

    auto const checkNonExistence = [dictionary](sc_char const * string, sc_addr addr) {
      EXPECT_FALSE(sc_dictionary_is_in(dictionary, string));
      sc_list * list = sc_dictionary_get(dictionary, string);
      sc_iterator * it = sc_list_iterator(list);
      EXPECT_FALSE(sc_iterator_next(it));
      EXPECT_TRUE(it == nullptr || sc_iterator_destroy(it));
    };

    for (auto const & item : toCheckNonExistence)
    {
      checkNonExistence(item.first, item.second);
    }
  };

  checkAfterRemove(str1, {{str2, addr2}, {str3, addr3}, {str4, addr4}}, {{str1, addr1}});
  checkAfterRemove(str2, {{str3, addr3}, {str4, addr4}}, {{str1, addr1}, {str2, addr2}});
  checkAfterRemove(str3, {{str4, addr4}}, {{str1, addr1}, {str2, addr2}, {str3, addr3}});
  checkAfterRemove(str4, {}, {{str1, addr1}, {str2, addr2}, {str3, addr3}, {str4, addr4}});

  sc_dictionary_destroy(dictionary, sc_dictionary_node_destroy);
}
