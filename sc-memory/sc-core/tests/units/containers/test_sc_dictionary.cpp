/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include <sc-core/sc-container/sc_dictionary.h>
#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc-container/sc_string.h>
}

sc_uint8 _test_sc_dictionary_addr_hashes_children_size()
{
  sc_uint8 const max_sc_char = 255;
  sc_uint8 const min_sc_char = 1;

  return max_sc_char - min_sc_char + 1;
}

void _test_sc_dictionary_addr_hashes_char_to_int(sc_char ch, sc_uint8 * ch_num, sc_uint8 const *)
{
  *ch_num = 128 + (sc_uint8)ch;
}

sc_bool _test_sc_uchar_dictionary_initialize(sc_dictionary ** dictionary)
{
  return sc_dictionary_initialize(
      dictionary, _test_sc_dictionary_addr_hashes_children_size(), _test_sc_dictionary_addr_hashes_char_to_int);
}

sc_bool _test_sc_uchar_dictionary_destroy(sc_dictionary * dictionary)
{
  return sc_dictionary_destroy(dictionary, nullptr);
}

sc_bool _test_sc_hashes_compare(void * hash, void * other)
{
  return hash == other;
}

TEST(ScDictionaryTest, sc_dictionary_init_destroy)
{
  sc_dictionary * dictionary;
  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));
  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));

  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));
  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));
}

TEST(ScDictionaryTest, sc_dictionary_append_get_by_key)
{
  sc_dictionary * dictionary;
  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));

  sc_addr_hash hash1 = 1024;
  sc_char string1[] = "string1";
  sc_uint32 string1_size = sc_str_len(string1);
  sc_dictionary_append(dictionary, string1, string1_size, (sc_addr_hash_to_sc_pointer)hash1);

  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string1, string1_size), hash1);

  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));
  EXPECT_FALSE(_test_sc_uchar_dictionary_destroy(nullptr));
}

TEST(ScDictionaryTest, sc_dictionary_append_get_by_key_reset)
{
  sc_dictionary * dictionary;
  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));

  sc_addr_hash hash1 = 1024;
  sc_char string1[] = "string1";
  sc_uint32 string1_size = sc_str_len(string1);
  sc_dictionary_append(dictionary, string1, string1_size, (sc_addr_hash_to_sc_pointer)hash1);

  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string1, string1_size), hash1);

  hash1 = 216;
  sc_dictionary_append(dictionary, string1, string1_size, (sc_addr_hash_to_sc_pointer)hash1);
  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string1, string1_size), hash1);

  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));
}

TEST(ScDictionaryTest, sc_dictionary_get_by_key)
{
  sc_dictionary * dictionary;
  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));

  sc_char string1[] = "string1";
  sc_uint32 string1_size = sc_str_len(string1);
  EXPECT_EQ(sc_dictionary_get_by_key(dictionary, string1, string1_size), nullptr);

  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));
}

TEST(ScDictionaryTest, sc_dictionary_get_by_nullptr)
{
  sc_dictionary * dictionary;
  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));

  EXPECT_EQ(sc_dictionary_get_by_key(dictionary, nullptr, 0), nullptr);

  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));
}

TEST(ScDictionaryTest, sc_dictionary_append_get_by_keys_smoke)
{
  sc_dictionary * dictionary;
  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));

  sc_addr_hash hash1 = 1;
  sc_char string1[] = "string1";
  sc_uint32 string1_size = sc_str_len(string1);
  sc_dictionary_append(dictionary, string1, string1_size, (sc_addr_hash_to_sc_pointer)hash1);

  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string1, string1_size), hash1);

  sc_addr_hash hash2 = 2;
  sc_char string2[] = "string2";
  sc_uint32 string2_size = sc_str_len(string2);
  sc_dictionary_append(dictionary, string2, string2_size, (sc_addr_hash_to_sc_pointer)hash2);

  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string1, string1_size), hash1);
  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string2, string2_size), hash2);

  sc_addr_hash hash3 = 3;
  sc_char string3[] = "str_to_int";
  sc_uint32 string3_size = sc_str_len(string3);
  sc_dictionary_append(dictionary, string3, string3_size, (sc_addr_hash_to_sc_pointer)hash3);

  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string1, string1_size), hash1);
  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string2, string2_size), hash2);
  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string3, string3_size), hash3);

  sc_addr_hash hash4 = 4;
  sc_char string4[] = "";
  sc_uint32 string4_size = sc_str_len(string4);
  sc_dictionary_append(dictionary, string4, string4_size, (sc_addr_hash_to_sc_pointer)hash4);

  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string1, string1_size), hash1);
  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string2, string2_size), hash2);
  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string3, string3_size), hash3);
  EXPECT_EQ((sc_pointer_to_sc_addr_hash)sc_dictionary_get_by_key(dictionary, string4, string4_size), hash4);

  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));
}

sc_bool _test_visit_nodes_by_key_prefix(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == nullptr)
    return SC_TRUE;

  auto * list = (sc_list *)arguments[0];
  sc_list_push_back(list, node->data);

  return SC_TRUE;
}

TEST(ScDictionaryTest, sc_dictionary_append_get_by_key_prefix)
{
  sc_dictionary * dictionary;
  EXPECT_TRUE(_test_sc_uchar_dictionary_initialize(&dictionary));

  sc_addr_hash hash1 = 1;
  sc_char string1[] = "string1";
  sc_uint32 string1_size = sc_str_len(string1);
  sc_dictionary_append(dictionary, string1, string1_size, (sc_addr_hash_to_sc_pointer)hash1);

  sc_addr_hash hash2 = 2;
  sc_char string2[] = "string2";
  sc_uint32 string2_size = sc_str_len(string2);
  sc_dictionary_append(dictionary, string2, string2_size, (sc_addr_hash_to_sc_pointer)hash2);

  sc_addr_hash hash3 = 3;
  sc_char string3[] = "str_to_int";
  sc_uint32 string3_size = sc_str_len(string3);
  sc_dictionary_append(dictionary, string3, string3_size, (sc_addr_hash_to_sc_pointer)hash3);

  sc_char search_string1[] = "str";
  sc_uint32 search_string1_size = sc_str_len(search_string1);

  sc_list * hashes;
  sc_list_init(&hashes);
  sc_dictionary_get_by_key_prefix(
      dictionary, search_string1, search_string1_size, _test_visit_nodes_by_key_prefix, (void **)&hashes);
  EXPECT_EQ(hashes->size, 3u);

  EXPECT_TRUE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash1, _test_sc_hashes_compare));
  EXPECT_TRUE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash2, _test_sc_hashes_compare));
  EXPECT_TRUE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash3, _test_sc_hashes_compare));
  EXPECT_EQ(hashes->size, 0u);
  sc_list_destroy(hashes);

  sc_char search_string2[] = "stri";
  sc_uint32 search_string2_size = sc_str_len(search_string2);

  sc_list_init(&hashes);
  sc_dictionary_get_by_key_prefix(
      dictionary, search_string2, search_string2_size, _test_visit_nodes_by_key_prefix, (void **)&hashes);
  EXPECT_EQ(hashes->size, 2u);

  EXPECT_TRUE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash1, _test_sc_hashes_compare));
  EXPECT_TRUE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash2, _test_sc_hashes_compare));
  EXPECT_FALSE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash3, _test_sc_hashes_compare));
  EXPECT_EQ(hashes->size, 0u);
  sc_list_destroy(hashes);

  sc_char search_string3[] = "str_";
  sc_uint32 search_string3_size = sc_str_len(search_string2);

  sc_list_init(&hashes);
  sc_dictionary_get_by_key_prefix(
      dictionary, search_string3, search_string3_size, _test_visit_nodes_by_key_prefix, (void **)&hashes);
  EXPECT_EQ(hashes->size, 1u);

  EXPECT_FALSE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash1, _test_sc_hashes_compare));
  EXPECT_FALSE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash2, _test_sc_hashes_compare));
  EXPECT_TRUE(sc_list_remove_if(hashes, (sc_addr_hash_to_sc_pointer)hash3, _test_sc_hashes_compare));
  EXPECT_EQ(hashes->size, 0u);
  sc_list_destroy(hashes);

  EXPECT_TRUE(_test_sc_uchar_dictionary_destroy(dictionary));
}
