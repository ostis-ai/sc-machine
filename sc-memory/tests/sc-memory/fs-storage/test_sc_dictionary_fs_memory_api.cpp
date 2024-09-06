#include <gtest/gtest.h>

#include "test_defines.hpp"

extern "C"
{
#include "sc-core/sc-store/sc-fs-memory/sc_dictionary_fs_memory.h"
#include "sc-core/sc-store/sc-fs-memory/sc_dictionary_fs_memory_private.h"
#include "sc-core/sc-store/sc-fs-memory/sc_file_system.h"
#include "sc-core/sc-store/sc-base/sc_allocator.h"
#include "sc-core/sc-store/sc-container/sc-pair/sc_pair.h"
#include "sc-core/sc-store/sc-container/sc-list/sc_list.h"
#include "sc-core/sc-store/sc-container/sc-string/sc_string.h"
#include "sc-core/sc-store/sc-fs-memory/sc_io.h"
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

#define SC_DICTIONARY_FS_MEMORY_PATH "fs-memory"
#define SC_DICTIONARY_FS_MEMORY_STRINGS_PATH SC_DICTIONARY_FS_MEMORY_PATH "/strings1.scdb"

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_shutdown)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_shutdown_no_path)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, nullptr), SC_FS_MEMORY_WRONG_PATH);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_NO);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_shutdown_no_exist_path)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, ""), SC_FS_MEMORY_WRONG_PATH);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_NO);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_nullptr)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, ""), SC_FS_MEMORY_WRONG_PATH);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, 0, nullptr, 0), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_unlink_string(memory, 0), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, 0, nullptr, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_get_link_hashes_by_string(memory, nullptr, 0, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, nullptr, 0, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_get_link_hashes_by_substring_ext(memory, nullptr, 0, 0, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_get_strings_by_substring(memory, nullptr, 0, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_get_strings_by_substring_ext(memory, nullptr, 0, 0, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_intersect_link_hashes_by_terms(memory, nullptr, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_unite_link_hashes_by_terms(memory, nullptr, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_intersect_strings_by_terms(memory, nullptr, nullptr), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_dictionary_fs_memory_unite_strings_by_terms(memory, nullptr, nullptr), SC_FS_MEMORY_NO);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_save_shutdown_load)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_rm_shutdown_load)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  sc_fs_remove_directory(SC_DICTIONARY_FS_MEMORY_PATH);
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_clear_init_shutdown_load)
{
  sc_memory_params * params = _sc_dictionary_fs_memory_get_default_params(SC_DICTIONARY_FS_MEMORY_PATH, SC_TRUE);
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
  sc_mem_free(params);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_save_shutdown_load_deprecated_dictionary)
{
  EXPECT_TRUE(sc_fs_copy_file(
      SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH "/test/strings.scdb",
      SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH "/strings.scdb"));

  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_string_by_link_hash)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_string_by_link_hash_invalid_data)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 518;

  {
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  sc_io_channel * channel = sc_io_new_write_channel(SC_DICTIONARY_FS_MEMORY_STRINGS_PATH, nullptr);
  sc_io_channel_seek(channel, 8, SC_FS_IO_SEEK_SET, nullptr);

  sc_char const invalid_string[] = "invalid string string";
  sc_uint64 const invalid_string_size = sc_str_len(invalid_string);
  sc_uint64 written_bytes;
  EXPECT_EQ(
      sc_io_channel_write_chars(channel, invalid_string, invalid_string_size, &written_bytes, nullptr),
      SC_FS_IO_STATUS_NORMAL);
  sc_io_channel_shutdown(channel, SC_TRUE, nullptr);

  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_FALSE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_READ_ERROR);
    EXPECT_EQ(found_string, null_ptr);
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_string_by_link_hash_reset)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash = 112;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    sc_char string2[] = TEXT_EXAMPLE_2;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_string_by_link_hash_not_found)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_addr_hash hash = 112;
  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_NO_STRING);
    EXPECT_EQ(found_string, nullptr);
    sc_mem_free(found_string);
  }
  {
    sc_char string1[] = TEXT_EXAMPLE_2;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_string_by_link_hash_no_file)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_addr_hash hash = 112;
  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_NO_STRING);
    EXPECT_EQ(found_string, nullptr);
    sc_mem_free(found_string);
  }
  sc_char string1[] = TEXT_EXAMPLE_2;
  {
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);
  }
  {
    sc_fs_remove_directory(SC_DICTIONARY_FS_MEMORY_PATH);
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_string_by_link_hash_reset_save_load)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 518;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_string_by_link_hash_reset_save_load_empty)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 518;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  sc_fs_remove_directory(SC_DICTIONARY_FS_MEMORY_PATH);
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_NO_STRING);
    EXPECT_EQ(found_string, nullptr);
    sc_mem_free(found_string);

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_NO_STRING);
    EXPECT_EQ(found_string, nullptr);
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

void _test_push_link_hash(void * data, sc_addr const link_addr)
{
  sc_list_push_back((sc_list *)data, (sc_pointer)(sc_uint64)SC_ADDR_LOCAL_TO_INT(link_addr));
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_string)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string2, sc_str_len(string2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_not_searchable_string)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_EQ(
        sc_dictionary_fs_memory_link_string_ext(memory, hash1, string1, sc_str_len(string1), SC_FALSE),
        SC_FS_MEMORY_OK);

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_EQ(
        sc_dictionary_fs_memory_link_string_ext(memory, hash2, string2, sc_str_len(string2), SC_FALSE),
        SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_NO_STRING);
    EXPECT_EQ(found_link_hashes->size, 0u);
    sc_list_destroy(found_link_hashes);

    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string2, sc_str_len(string2), &filter),
        SC_FS_MEMORY_NO_STRING);
    EXPECT_EQ(found_link_hashes->size, 0u);
    sc_list_destroy(found_link_hashes);

    sc_char * found_string;
    sc_uint64 size;
    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string1));
    sc_mem_free(found_string);

    EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash2, &found_string, &size), SC_FS_MEMORY_OK);
    EXPECT_TRUE(sc_str_cmp(found_string, string2));
    sc_mem_free(found_string);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_string_invalid_data)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 518;

  {
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string2, sc_str_len(string2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  sc_io_channel * channel = sc_io_new_write_channel(SC_DICTIONARY_FS_MEMORY_STRINGS_PATH, nullptr);
  sc_io_channel_seek(channel, 8, SC_FS_IO_SEEK_SET, nullptr);

  sc_char const invalid_string[] = "invalid string string";
  sc_uint64 const invalid_string_size = sc_str_len(invalid_string);
  sc_uint64 written_bytes;
  EXPECT_EQ(
      sc_io_channel_write_chars(channel, invalid_string, invalid_string_size, &written_bytes, nullptr),
      SC_FS_IO_STATUS_NORMAL);
  sc_io_channel_shutdown(channel, SC_TRUE, nullptr);

  {
    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_READ_ERROR);
    EXPECT_EQ(found_link_hashes->size, 0u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_FALSE(sc_iterator_next(it));
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string2, sc_str_len(string2), &filter),
        SC_FS_MEMORY_READ_ERROR);
    EXPECT_EQ(found_link_hashes->size, 0u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_FALSE(sc_iterator_next(it));
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_string_reset)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_addr_hash hash = 112;
  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }
  {
    sc_char string2[] = TEXT_EXAMPLE_2;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string2, sc_str_len(string2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }
  {
    sc_char string1[] = TEXT_EXAMPLE_1;

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 0u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_FALSE(sc_iterator_next(it));
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_string_no_file)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  {
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }
  {
    sc_fs_remove_directory(SC_DICTIONARY_FS_MEMORY_PATH);
    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_string_save_load)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 518;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  {
    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string2, sc_str_len(string2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string2, sc_str_len(string2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_substring)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    sc_char substring1[] = "it";
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring1, sc_str_len(substring1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 2u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_char substring2[] = "it is the first";
    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring2, sc_str_len(substring2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_char substring3[] = "the first";
    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring3, sc_str_len(substring3), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_char substring4[] = "it is the second";
    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring4, sc_str_len(substring4), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_char substring5[] = "the second";
    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring5, sc_str_len(substring5), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_link_hashes_by_substring_when_false_config)
{
  sc_dictionary_fs_memory * memory;
  sc_memory_params * params = _sc_dictionary_fs_memory_get_default_params(SC_DICTIONARY_FS_MEMORY_PATH, SC_FALSE);
  params->search_by_substring = SC_FALSE;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize_ext(&memory, params), SC_FS_MEMORY_OK);
  sc_mem_free(params);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    sc_char substring1[] = "it";
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring1, sc_str_len(substring1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 2u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash2);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_char substring2[] = "the first";
    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring2, sc_str_len(substring2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 0u);
    sc_list_destroy(found_link_hashes);

    sc_char substring3[] = "the second";
    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_substring(memory, substring3, sc_str_len(substring3), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 0u);
    sc_list_destroy(found_link_hashes);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

void _test_push_link_content(void * data, sc_addr const link_addr, sc_char const * link_content)
{
  sc_uint32 const size = sc_str_len(link_content);
  sc_char * copied_string;
  sc_str_cpy(copied_string, link_content, size);

  sc_list_push_back((sc_list *)data, (sc_pointer)copied_string);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_get_strings_by_substring)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_list * found_strings;
    sc_list_init(&found_strings);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = nullptr;
    filter.push_link_callback_data = nullptr;
    filter.push_link_content_callback = _test_push_link_content;
    filter.push_link_content_callback_data = found_strings;

    sc_char substring1[] = "it";
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_strings_by_substring(memory, substring1, sc_str_len(substring1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_strings->size, 2u);

    sc_iterator * it = sc_list_iterator(found_strings);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_TRUE(sc_str_cmp((sc_char *)sc_iterator_get(it), string1));
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_TRUE(sc_str_cmp((sc_char *)sc_iterator_get(it), string2));
    sc_iterator_destroy(it);
    sc_list_clear(found_strings);
    sc_list_destroy(found_strings);

    sc_char substring2[] = "it is the first";
    sc_list_init(&found_strings);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_strings_by_substring(memory, substring2, sc_str_len(substring2), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_strings->size, 1u);

    it = sc_list_iterator(found_strings);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_TRUE(sc_str_cmp((sc_char *)sc_iterator_get(it), string1));
    sc_iterator_destroy(it);
    sc_list_clear(found_strings);
    sc_list_destroy(found_strings);

    sc_char substring3[] = "it is the second";
    sc_list_init(&found_strings);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_strings_by_substring(memory, substring3, sc_str_len(substring3), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_strings->size, 1u);

    it = sc_list_iterator(found_strings);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_TRUE(sc_str_cmp((sc_char *)sc_iterator_get(it), string2));
    sc_iterator_destroy(it);
    sc_list_clear(found_strings);
    sc_list_destroy(found_strings);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_link_unlink_strings)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  {
    sc_char string1[] = TEXT_EXAMPLE_1;
    sc_addr_hash hash1 = 112;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

    sc_char string2[] = TEXT_EXAMPLE_2;
    sc_addr_hash hash2 = 518;
    EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

    sc_list * found_link_hashes;
    sc_list_init(&found_link_hashes);

    sc_link_filter filter;
    filter.check_link_callback = nullptr;
    filter.check_link_callback_data = nullptr;
    filter.request_link_callback = nullptr;
    filter.request_link_callback_data = nullptr;
    filter.push_link_callback = _test_push_link_hash;
    filter.push_link_callback_data = found_link_hashes;
    filter.push_link_content_callback = nullptr;
    filter.push_link_content_callback_data = nullptr;

    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 1u);

    sc_iterator * it = sc_list_iterator(found_link_hashes);
    EXPECT_TRUE(sc_iterator_next(it));
    EXPECT_EQ((sc_addr_hash)sc_iterator_get(it), hash1);
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    EXPECT_EQ(sc_dictionary_fs_memory_unlink_string(memory, hash1), SC_FS_MEMORY_OK);

    sc_list_init(&found_link_hashes);
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_link_hashes_by_string(memory, string1, sc_str_len(string1), &filter),
        SC_FS_MEMORY_OK);
    EXPECT_EQ(found_link_hashes->size, 0u);

    it = sc_list_iterator(found_link_hashes);
    EXPECT_FALSE(sc_iterator_next(it));
    sc_iterator_destroy(it);
    sc_list_destroy(found_link_hashes);

    sc_char * found_string;
    sc_uint64 found_string_size;
    EXPECT_EQ(
        sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash1, &found_string, &found_string_size),
        SC_FS_MEMORY_NO_STRING);

    EXPECT_EQ(found_string, nullptr);
    EXPECT_EQ(found_string_size, 0u);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_intersect_strings_by_terms)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 513;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_memory_intersect_strings_by_terms(memory, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 1u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_intersect_strings_by_terms_2)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_ABOUT_CAT_EXAMPLE_1;
  sc_addr_hash hash = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_ABOUT_CAT_EXAMPLE_2;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"cat"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"Domestic"));

  sc_list * found_strings;
  sc_dictionary_fs_memory_intersect_strings_by_terms(memory, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 2u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_unite_strings_by_terms)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_memory_unite_strings_by_terms(memory, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 2u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_unite_strings_by_term_2)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_ABOUT_CAT_EXAMPLE_1;
  sc_addr_hash hash = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_ABOUT_CAT_EXAMPLE_2;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  sc_char string3[] = TEXT_ABOUT_CAT_EXAMPLE_3;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string3, sc_str_len(string3)), SC_FS_MEMORY_OK);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"cat"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"Domestic"));

  sc_list * found_strings;
  sc_dictionary_fs_memory_unite_strings_by_terms(memory, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 3u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

void test_sc_dictionary_fs_memory_link_strings(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash link_hash1,
    sc_addr_hash link_hash2)
{
  sc_char string1[] = TEXT_EXAMPLE_1;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, link_hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, link_hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);
}

void test_sc_dictionary_fs_memory_get_links_intersect_strings_by_terms(sc_dictionary_fs_memory * memory)
{
  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_memory_intersect_strings_by_terms(memory, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 1u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_add_save_shutdown_load_intersect)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  test_sc_dictionary_fs_memory_link_strings(memory, 1180, 5855);
  {
    test_sc_dictionary_fs_memory_get_links_intersect_strings_by_terms(memory);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    test_sc_dictionary_fs_memory_get_links_intersect_strings_by_terms(memory);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    test_sc_dictionary_fs_memory_get_links_intersect_strings_by_terms(memory);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

void test_sc_dictionary_fs_memory_get_links_unite_strings_by_terms(sc_dictionary_fs_memory * memory)
{
  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_strings;
  sc_dictionary_fs_memory_unite_strings_by_terms(memory, terms, &found_strings);
  sc_list_destroy(terms);

  EXPECT_EQ(found_strings->size, 2u);
  sc_list_clear(found_strings);
  sc_list_destroy(found_strings);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_init_add_save_shutdown_load_unite)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  test_sc_dictionary_fs_memory_link_strings(memory, 1122, 5880);
  {
    test_sc_dictionary_fs_memory_get_links_unite_strings_by_terms(memory);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    test_sc_dictionary_fs_memory_get_links_unite_strings_by_terms(memory);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_load(memory), SC_FS_MEMORY_OK);

  {
    test_sc_dictionary_fs_memory_get_links_unite_strings_by_terms(memory);
  }

  EXPECT_EQ(sc_dictionary_fs_memory_save(memory), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_intersect_link_hashes_by_terms)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 513;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_link_hashes;
  sc_dictionary_fs_memory_intersect_link_hashes_by_terms(memory, terms, &found_link_hashes);
  sc_list_destroy(terms);

  EXPECT_EQ(found_link_hashes->size, 1u);
  EXPECT_EQ((sc_addr_hash)found_link_hashes->begin->data, hash1);
  sc_list_destroy(found_link_hashes);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_unite_link_hashes_by_terms)
{
  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize(&memory, SC_DICTIONARY_FS_MEMORY_PATH), SC_FS_MEMORY_OK);

  sc_char string1[] = TEXT_EXAMPLE_1;
  sc_addr_hash hash1 = 112;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash1, string1, sc_str_len(string1)), SC_FS_MEMORY_OK);

  sc_char string2[] = TEXT_EXAMPLE_2;
  sc_addr_hash hash2 = 512;
  EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash2, string2, sc_str_len(string2)), SC_FS_MEMORY_OK);

  sc_list * terms;
  sc_list_init(&terms);
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"it"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"first"));
  EXPECT_TRUE(sc_list_push_back(terms, (void *)"the"));

  sc_list * found_link_hashes;
  sc_dictionary_fs_memory_unite_link_hashes_by_terms(memory, terms, &found_link_hashes);
  sc_list_destroy(terms);

  EXPECT_EQ(found_link_hashes->size, 2u);
  EXPECT_EQ((sc_addr_hash)found_link_hashes->begin->data, hash1);
  EXPECT_EQ((sc_addr_hash)found_link_hashes->begin->next->data, hash2);
  sc_list_destroy(found_link_hashes);

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_mutiple_link_strings)
{
  sc_memory_params params;
  params.repo_path = SC_DICTIONARY_FS_MEMORY_PATH;
  params.clear = SC_TRUE;
  params.max_strings_channels = DEFAULT_MAX_STRINGS_CHANNELS;
  params.max_strings_channel_size = DEFAULT_MAX_STRINGS_CHANNEL_SIZE;
  params.max_searchable_string_size = DEFAULT_MAX_SEARCHABLE_STRING_SIZE;
  params.term_separators = DEFAULT_TERM_SEPARATORS;

  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize_ext(&memory, &params), SC_FS_MEMORY_OK);

  {
    sc_char const string_template[] = "This is string number %llu";
    sc_char string[50];

    sc_uint64 const STRING_COUNT = 1000;
    for (sc_uint64 hash = 0; hash < STRING_COUNT; ++hash)
    {
      sprintf(string, string_template, hash);

      EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string, sc_str_len(string)), SC_FS_MEMORY_OK);
    }

    sc_char * found_string;
    sc_uint64 size;
    for (sc_uint64 hash = 0; hash < STRING_COUNT; ++hash)
    {
      sprintf(string, string_template, hash);

      EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_OK);
      EXPECT_TRUE(sc_str_cmp(found_string, string));
      sc_mem_free(found_string);
    }
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}

TEST(ScDictionaryFSMemoryTest, sc_dictionary_fs_memory_mutiple_link_strings_with_optimized_config)
{
  sc_memory_params params;
  params.repo_path = SC_DICTIONARY_FS_MEMORY_PATH;
  params.clear = SC_TRUE;
  params.max_strings_channels = DEFAULT_MAX_STRINGS_CHANNELS;
  params.max_strings_channel_size = DEFAULT_MAX_STRINGS_CHANNEL_SIZE;
  params.max_searchable_string_size = DEFAULT_MAX_SEARCHABLE_STRING_SIZE;
  params.term_separators = "";

  sc_dictionary_fs_memory * memory;
  EXPECT_EQ(sc_dictionary_fs_memory_initialize_ext(&memory, &params), SC_FS_MEMORY_OK);

  {
    sc_char const string_template[] = "This is string number %llu";
    sc_char string[50];

    sc_uint64 const STRING_COUNT = 1000;
    for (sc_uint64 hash = 0; hash < STRING_COUNT; ++hash)
    {
      sprintf(string, string_template, hash);

      EXPECT_EQ(sc_dictionary_fs_memory_link_string(memory, hash, string, sc_str_len(string)), SC_FS_MEMORY_OK);
    }

    sc_char * found_string;
    sc_uint64 size;
    for (sc_uint64 hash = 0; hash < STRING_COUNT; ++hash)
    {
      sprintf(string, string_template, hash);

      EXPECT_EQ(sc_dictionary_fs_memory_get_string_by_link_hash(memory, hash, &found_string, &size), SC_FS_MEMORY_OK);
      EXPECT_TRUE(sc_str_cmp(found_string, string));
      sc_mem_free(found_string);
    }
  }

  EXPECT_EQ(sc_dictionary_fs_memory_shutdown(memory), SC_FS_MEMORY_OK);
}
