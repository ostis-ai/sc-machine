#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-container/sc-dictionary/sc_dictionary.h"
}

#include "sc_test.hpp"

using ScDictionaryTest = ScMemoryTest;

TEST_F(ScDictionaryTest, smoke)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc-strings-complex");

  sc_dictionary * dictionary = nullptr;
  EXPECT_TRUE(sc_dictionary_initialize(&dictionary));

  sc_addr addr1 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str1 = "concept_set1";
  sc_dictionary_append(dictionary, str1, strlen(str1), &addr1);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr2 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str2 = "concept_letter";
  sc_dictionary_append(dictionary, str2, strlen(str2), &addr2);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr3 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str3 = "sc_node_norole_relation";
  sc_dictionary_append(dictionary, str3, strlen(str3), &addr3);
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str3));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str2));
  EXPECT_TRUE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr4 = sc_memory_link_new(ctx.GetRealContext());
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

  sc_dictionary_show(dictionary);

  ctx.Destroy();
}
