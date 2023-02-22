#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-fs-storage/sc_dictionary_fs_storage.h"
#include "sc-core/sc-store/sc-fs-storage/sc_file_system.h"
#include "sc-core/sc-store/sc-base/sc_allocator.h"
#include "sc-core/sc-store/sc-container/sc-pair/sc_pair.h"
#include "sc-core/sc-store/sc-container/sc-list/sc_list.h"
#include "sc-core/sc-store/sc-container/sc-string/sc_string.h"
}

#define TEXT_ABOUT_CAT_EXAMPLE_1 \
  "The cat (Felis catus) is a domestic species of small carnivorous mammal.[1][2] It is the only " \
  "domesticated species in the family Felidae and is commonly referred to as the domestic cat or house " \
  "cat to distinguish it from the wild members of the family.[4] A cat can either be a house cat, a " \
  "farm cat, or a feral cat; the latter ranges freely and avoids human contact.[5] Domestic cats are " \
  "valued by humans for companionship and their ability to kill rodents. About 60 cat breeds are " \
  "recognized by various cat registries."

#define TEXT_ABOUT_CAT_EXAMPLE_2 \
  "Female domestic cats can have kittens from spring to late autumn, with litter sizes often ranging " \
  "from two to five kittens.[9] Domestic cats are bred and shown at events as registered pedigreed " \
  "cats, a hobby known as cat fancy. Population control of cats may be effected by spaying and " \
  "neutering, but their proliferation and the abandonment of pets has resulted in large numbers of " \
  "feral cats worldwide, contributing to the extinction of entire bird, mammal, and reptile species."

#define TEXT_ABOUT_CAT_EXAMPLE_3 \
  "It was long thought that cat domestication began in ancient Egypt, where cats were venerated from " \
  "around 3100 BC,[11][12] but recent advances in archaeology and genetics have shown that their " \
  "domestication occurred in Western Asia around 7500 BC."

#define TEXT_EXAMPLE_1 "it is the first string"
#define TEXT_EXAMPLE_2 "it is the second string"

#define SC_DICTIONARY_FS_STORAGE_PATH "fs-storage"

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_shutdown)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_intersect_strings_by_terms)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 112;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_EXAMPLE_1, (void *)hash)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_EXAMPLE_2, (void *)hash)));

  EXPECT_TRUE(sc_dictionary_fs_storage_link_strings(storage, strings) == SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 1u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_intersect_strings_by_terms_2)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 2586;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_1, (void *)hash)));
  hash = 148;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_2, (void *)hash)));

  EXPECT_TRUE(sc_dictionary_fs_storage_link_strings(storage, strings) == SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"cat"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"Domestic"));

  sc_list * found_strings;
  sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 2u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_unite_strings_by_terms)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 112;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_EXAMPLE_1, (void *)hash)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_EXAMPLE_2, (void *)hash)));

  EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_storage_unite_strings_by_terms(storage, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 2u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_unite_strings_by_term_2)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 2586;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_1, (void *)hash)));
  hash = 148;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_2, (void *)hash)));
  hash = 148;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_3, (void *)hash)));

  EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"cat"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"Domestic"));

  sc_dictionary_fs_storage_unite_strings_by_terms(storage, terms, &strings);
  sc_list_destroy(terms);

  EXPECT_EQ(strings->size, 3u);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_save_shutdown_load)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_rm_shutdown_load)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  sc_fs_rmdir(SC_DICTIONARY_FS_STORAGE_PATH);
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_READ_ERROR);
  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

void test_sc_dictionary_fs_storage_link_strings(
    sc_dictionary_fs_storage * storage,
    sc_addr_hash link_hash1,
    sc_addr_hash link_hash2)
{
  sc_list * strings;
  sc_list_init(&strings);
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_EXAMPLE_1, (void *)link_hash1)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_EXAMPLE_2, (void *)link_hash2)));

  EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);
};

void test_sc_dictionary_fs_storage_get_links_intersect_strings_by_terms(sc_dictionary_fs_storage * storage)
{
  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 1u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);
};

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_add_save_shutdown_load_intersect)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  test_sc_dictionary_fs_storage_link_strings(storage, 1180, 5855);
  {
    test_sc_dictionary_fs_storage_get_links_intersect_strings_by_terms(storage);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);

  {
    test_sc_dictionary_fs_storage_get_links_intersect_strings_by_terms(storage);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);

  {
    test_sc_dictionary_fs_storage_get_links_intersect_strings_by_terms(storage);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

void test_sc_dictionary_fs_storage_get_links_unite_strings_by_terms(sc_dictionary_fs_storage * storage)
{
  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_storage_unite_strings_by_terms(storage, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 2u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);
};

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_add_save_shutdown_load_unite)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  test_sc_dictionary_fs_storage_link_strings(storage, 1122, 5880);
  {
    test_sc_dictionary_fs_storage_get_links_unite_strings_by_terms(storage);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);

  {
    test_sc_dictionary_fs_storage_get_links_unite_strings_by_terms(storage);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);

  {
    test_sc_dictionary_fs_storage_get_links_unite_strings_by_terms(storage);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_get_string_by_link_hash)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  {
    sc_list * strings;
    sc_list_init(&strings);
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string1, (void *)hash1)));

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string2, (void *)hash2)));

    EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
    sc_list_clear(strings);
    sc_list_destroy(strings);

    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash1, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash2, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_get_string_by_link_hash_reset)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  {
    sc_list * strings;
    sc_list_init(&strings);
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash = 112;
    EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string1, (void *)hash)));

    EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
    sc_list_clear(strings);
    sc_list_destroy(strings);

    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    sc_list_init(&strings);
    sc_char string2[] = TEXT_EXAMPLE_2;
    EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string2, (void *)hash)));

    EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
    sc_list_clear(strings);
    sc_list_destroy(strings);

    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_get_string_by_link_hash_not_found)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  sc_addr_hash hash = 112;
  {
    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash, &found_string), SC_FS_STORAGE_NO_STRING);
    EXPECT_EQ(found_string, nullptr);
    sc_mem_free(found_string);
  }
  {
    sc_list * strings;
    sc_list_init(&strings);
    sc_char string1[] = TEXT_EXAMPLE_2;
    EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string1, (void *)hash)));

    EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
    sc_list_clear(strings);
    sc_list_destroy(strings);

    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_get_string_by_link_hash_reset_save_load)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 518;

  sc_list * strings;
  sc_list_init(&strings);
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string1, (void *)hash1)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string2, (void *)hash2)));

  EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  {
    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash1, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash2, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);

  {
    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash1, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash2, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_OK);

  {
    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash1, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash2, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_get_string_by_link_hash_reset_save_load_empty)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 518;

  sc_list * strings;
  sc_list_init(&strings);
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string1, (void *)hash1)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)string2, (void *)hash2)));

  EXPECT_EQ(sc_dictionary_fs_storage_link_strings(storage, strings), SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  {
    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash1, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash2, &found_string), SC_FS_STORAGE_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);

  sc_fs_rmdir(SC_DICTIONARY_FS_STORAGE_PATH);
  EXPECT_EQ(sc_dictionary_fs_storage_initialize(&storage, SC_DICTIONARY_FS_STORAGE_PATH), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_load(storage), SC_FS_STORAGE_READ_ERROR);

  {
    sc_char * found_string;
    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash1, &found_string), SC_FS_STORAGE_NO_STRING);
    EXPECT_EQ(found_string, nullptr);
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_storage_get_string_by_link_hash(storage, hash2, &found_string), SC_FS_STORAGE_NO_STRING);
    EXPECT_EQ(found_string, nullptr);
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_storage_save(storage), SC_FS_STORAGE_OK);
  EXPECT_EQ(sc_dictionary_fs_storage_shutdown(storage), SC_FS_STORAGE_OK);
}
