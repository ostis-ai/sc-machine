#include <gtest/gtest.h>

#include "test_defines.hpp"

extern "C"
{
#include "sc-core/sc-store/sc-fs-memory/sc_file_system.h"
#include "sc-core/sc-store/sc-fs-memory/sc_fs_memory.h"
#include "sc-core/sc-store/sc-fs-memory/sc_io.h"
#include "sc-core/sc-store/sc-container/sc-string/sc_string.h"
#include "sc-core/sc-store/sc_segment.h"
#include "sc-core/sc-store/sc_storage_private.h"
}

#define SC_FS_MEMORY_PATH "fs-memory"
#define SC_FS_MEMORY_SEGMENTS_PATH SC_FS_MEMORY_PATH "/segments.scdb"

TEST(ScFSMemoryTest, sc_fs_memory_initialize_shutdown)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_FALSE), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_FALSE), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_initialize_shutdown_invalid_path)
{
  EXPECT_EQ(sc_fs_memory_initialize("", SC_FALSE), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_NO);

  EXPECT_EQ(sc_fs_memory_initialize("", SC_FALSE), SC_FS_MEMORY_NO);
  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_NO);
}

TEST(ScFSMemoryTest, sc_fs_memory_initialize_shutdown_clear)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);

  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);
  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 2);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);
  EXPECT_EQ(storage->segments_count, 0u);

  storage->segments_count = 2;
  storage->segments[0] = sc_segment_new(0);
  storage->segments[1] = sc_segment_new(1);
  EXPECT_EQ(sc_fs_memory_save(storage), SC_FS_MEMORY_OK);
  sc_segment_free(storage->segments[0]);
  sc_segment_free(storage->segments[1]);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);
  sc_segment_free(storage->segments[0]);
  sc_segment_free(storage->segments[1]);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_deprecated_segments)
{
  EXPECT_TRUE(sc_fs_copy_file(
      SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH "/test/segments.scdb",
      SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH "/segments.scdb"));
  EXPECT_TRUE(sc_fs_copy_file(
      SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH "/test/strings.scdb",
      SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH "/strings.scdb"));

  EXPECT_EQ(sc_fs_memory_initialize(SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH, SC_FALSE), SC_FS_MEMORY_OK);

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 1);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);
  EXPECT_EQ(storage->segments_count, 1u);

  EXPECT_EQ(sc_fs_memory_save(storage), SC_FS_MEMORY_OK);
  for (sc_addr_seg i = 0; i < storage->segments_count; ++i)
    sc_segment_free(storage->segments[i]);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);
  for (sc_addr_seg i = 0; i < storage->segments_count; ++i)
    sc_segment_free(storage->segments[i]);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_file_read)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 2);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);
  EXPECT_EQ(storage->segments_count, 0u);

  EXPECT_TRUE(sc_fs_create_file(SC_FS_MEMORY_SEGMENTS_PATH));
  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_READ_ERROR);
  EXPECT_TRUE(sc_fs_remove_file(SC_FS_MEMORY_SEGMENTS_PATH));

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segments_num_read)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 2);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);
  EXPECT_EQ(storage->segments_count, 0u);

  EXPECT_TRUE(sc_fs_create_file(SC_FS_MEMORY_SEGMENTS_PATH));

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_READ_ERROR);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segment_read)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 2);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);
  EXPECT_EQ(storage->segments_count, 0u);

  sc_io_channel * channel = sc_io_new_write_channel(SC_FS_MEMORY_SEGMENTS_PATH, nullptr);
  EXPECT_NE(channel, nullptr);
  sc_addr_seg const segments_num = 2;
  sc_uint64 written_bytes;
  EXPECT_EQ(
      sc_io_channel_write_chars(channel, &segments_num, sizeof(sc_addr_seg), &written_bytes, nullptr),
      SC_FS_IO_STATUS_NORMAL);
  sc_char const invalid_segment[] = "segment";
  sc_addr_seg const invalid_segment_size = sc_str_len(invalid_segment);
  EXPECT_EQ(
      sc_io_channel_write_chars(channel, invalid_segment, invalid_segment_size, &written_bytes, nullptr),
      SC_FS_IO_STATUS_NORMAL);
  sc_io_channel_shutdown(channel, SC_TRUE, nullptr);

  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_READ_ERROR);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_file_write)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);
  EXPECT_TRUE(sc_fs_remove_directory(SC_FS_MEMORY_PATH));

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 2);
  storage->segments[0] = nullptr;
  storage->segments[1] = nullptr;
  storage->segments_count = 2;

  EXPECT_EQ(sc_fs_memory_save(storage), SC_FS_MEMORY_WRITE_ERROR);
  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segments_num_write)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 2);
  storage->segments[0] = nullptr;
  storage->segments[1] = nullptr;
  storage->segments_count = *(sc_uint64 *)"invalid_size";

  EXPECT_EQ(sc_fs_memory_save(storage), SC_FS_MEMORY_WRITE_ERROR);
  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segment_write)
{
  EXPECT_EQ(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE), SC_FS_MEMORY_OK);

  sc_storage * storage = sc_mem_new(sc_storage, 1);
  storage->segments = sc_mem_new(sc_segment *, 2);
  storage->segments[0] = nullptr;
  storage->segments[1] = nullptr;
  storage->segments_count = 2;

  EXPECT_EQ(sc_fs_memory_save(storage), SC_FS_MEMORY_WRITE_ERROR);
  EXPECT_EQ(sc_fs_memory_load(storage), SC_FS_MEMORY_OK);

  sc_mem_free(storage->segments);
  sc_mem_free(storage);

  EXPECT_EQ(sc_fs_memory_shutdown(), SC_FS_MEMORY_OK);
}
