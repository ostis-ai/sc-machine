#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-container/sc-pair/sc_pair.h"
#include "sc-core/sc-store/sc-container/sc-list/sc_list.h"
#include "sc-core/sc-store/sc-fs-storage/sc_dictionary_fs_storage.h"
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

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_shutdown)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);

  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_intersect_strings_by_terms)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 112;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the first string", (void *)hash)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the second string", (void *)hash)));

  EXPECT_TRUE(sc_dictionary_fs_storage_link_strings(storage, strings) == SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &strings);
  sc_list_destroy(terms);

  EXPECT_EQ(strings->size, 1u);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_intersect_strings_by_terms_2)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);

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

  sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &strings);
  sc_list_destroy(terms);

  EXPECT_EQ(strings->size, 2u);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_unite_strings_by_terms)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 112;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the first string", (void *)hash)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the second string", (void *)hash)));

  EXPECT_TRUE(sc_dictionary_fs_storage_link_strings(storage, strings) == SC_FS_STORAGE_OK);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_dictionary_fs_storage_unite_strings_by_terms(storage, terms, &strings);
  sc_list_destroy(terms);

  EXPECT_EQ(strings->size, 2u);
  sc_list_clear(strings);
  sc_list_destroy(strings);

  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_unite_strings_by_term_2)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 2586;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_1, (void *)hash)));
  hash = 148;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_2, (void *)hash)));
  hash = 148;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)TEXT_ABOUT_CAT_EXAMPLE_3, (void *)hash)));

  EXPECT_TRUE(sc_dictionary_fs_storage_link_strings(storage, strings) == SC_FS_STORAGE_OK);
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

  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_save_shutdown_load)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);

  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_load(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);

  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_load(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_add_save_shutdown_load_intersect)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 112;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the first string", (void *)hash)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the second string", (void *)hash)));

  {
    EXPECT_TRUE(sc_dictionary_fs_storage_link_strings(storage, strings) == SC_FS_STORAGE_OK);
    sc_list_clear(strings);
    sc_list_destroy(strings);

    sc_list * terms;
    sc_list_init(&terms);
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

    sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &strings);
    sc_list_destroy(terms);

    EXPECT_EQ(strings->size, 1u);
    sc_list_clear(strings);
    sc_list_destroy(strings);
  }

  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);

  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_load(storage) == SC_FS_STORAGE_OK);

  {
    sc_list * terms;
    sc_list_init(&terms);
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

    sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &strings);
    sc_list_destroy(terms);

    EXPECT_EQ(strings->size, 1u);
    sc_list_clear(strings);
    sc_list_destroy(strings);
  }

  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);

  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_load(storage) == SC_FS_STORAGE_OK);

  {
    sc_list * terms;
    sc_list_init(&terms);
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

    sc_dictionary_fs_storage_intersect_strings_by_terms(storage, terms, &strings);
    sc_list_destroy(terms);

    EXPECT_EQ(strings->size, 1u);
    sc_list_clear(strings);
    sc_list_destroy(strings);
  }

  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}

TEST(ScDictionaryFsStorageTest, sc_dictionary_fs_storage_init_add_save_shutdown_load_unite)
{
  sc_dictionary_fs_storage * storage;
  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);

  sc_list * strings;
  sc_list_init(&strings);
  sc_addr_hash hash = 112;
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the first string", (void *)hash)));
  EXPECT_TRUE(sc_list_push_back(strings, sc_make_pair((void *)"it is the second string", (void *)hash)));

  {
    EXPECT_TRUE(sc_dictionary_fs_storage_link_strings(storage, strings) == SC_FS_STORAGE_OK);
    sc_list_clear(strings);
    sc_list_destroy(strings);

    sc_list * terms;
    sc_list_init(&terms);
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

    sc_dictionary_fs_storage_unite_strings_by_terms(storage, terms, &strings);
    sc_list_destroy(terms);

    EXPECT_EQ(strings->size, 2u);
    sc_list_clear(strings);
    sc_list_destroy(strings);
  }

  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);

  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_load(storage) == SC_FS_STORAGE_OK);

  {
    sc_list * terms;
    sc_list_init(&terms);
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

    sc_dictionary_fs_storage_unite_strings_by_terms(storage, terms, &strings);
    sc_list_destroy(terms);

    EXPECT_EQ(strings->size, 2u);
    sc_list_clear(strings);
    sc_list_destroy(strings);
  }

  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);

  EXPECT_TRUE(sc_dictionary_fs_storage_initialize(&storage, "db/") == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_load(storage) == SC_FS_STORAGE_OK);

  {
    sc_list * terms;
    sc_list_init(&terms);
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
    EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

    sc_dictionary_fs_storage_unite_strings_by_terms(storage, terms, &strings);
    sc_list_destroy(terms);

    EXPECT_EQ(strings->size, 2u);
    sc_list_clear(strings);
    sc_list_destroy(strings);
  }

  EXPECT_TRUE(sc_dictionary_fs_storage_save(storage) == SC_FS_STORAGE_OK);
  EXPECT_TRUE(sc_dictionary_fs_storage_shutdown(storage) == SC_FS_STORAGE_OK);
}
