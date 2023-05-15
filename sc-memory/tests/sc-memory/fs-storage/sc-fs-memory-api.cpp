#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-fs-memory/sc_file_system.h"
#include "sc-core/sc-store/sc-fs-memory/sc_fs_memory.h"
#include "sc-core/sc-store/sc-fs-memory/sc_io.h"
#include "sc-core/sc-store/sc-container/sc-string/sc_string.h"
#include "sc-core/sc-store/sc_segment.h"
}

#define SC_FS_MEMORY_PATH "fs-memory"
#define SC_FS_MEMORY_SEGMENTS_PATH SC_FS_MEMORY_PATH "/segments.scdb"

TEST(ScFSMemoryTest, sc_fs_memory_initialize_shutdown)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_FALSE));
  EXPECT_TRUE(sc_fs_memory_shutdown());

  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_FALSE));
  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_initialize_shutdown_invalid_path)
{
  EXPECT_FALSE(sc_fs_memory_initialize("", SC_FALSE));
  EXPECT_FALSE(sc_fs_memory_shutdown());

  EXPECT_FALSE(sc_fs_memory_initialize("", SC_FALSE));
  EXPECT_FALSE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_initialize_shutdown_clear)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));
  EXPECT_TRUE(sc_fs_memory_shutdown());

  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));
  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));

  sc_uint32 written_size = 2;
  sc_segment * segments[written_size];

  sc_uint32 read_size;
  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_EQ(read_size, 0u);

  segments[0] = sc_segment_new(0);
  segments[1] = sc_segment_new(1);
  EXPECT_TRUE(sc_fs_memory_save(segments, written_size));
  sc_segment_free(segments[0]);
  sc_segment_free(segments[1]);

  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_EQ(read_size, written_size);
  sc_segment_free(segments[0]);
  sc_segment_free(segments[1]);

  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_file_read)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));

  sc_uint32 size = 2;
  sc_segment * segments[size];

  sc_uint32 read_size;
  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_EQ(read_size, 0u);

  EXPECT_TRUE(sc_fs_create_file(SC_FS_MEMORY_SEGMENTS_PATH));
  EXPECT_FALSE(sc_fs_memory_load(segments, &read_size));
  EXPECT_TRUE(sc_fs_remove_file(SC_FS_MEMORY_SEGMENTS_PATH));

  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segments_num_read)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));

  sc_uint32 size = 2;
  sc_segment * segments[size];

  sc_uint32 read_size;
  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_EQ(read_size, 0u);

  EXPECT_TRUE(sc_fs_create_file(SC_FS_MEMORY_SEGMENTS_PATH));

  EXPECT_FALSE(sc_fs_memory_load(segments, &read_size));
  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segment_read)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));

  sc_uint32 size = 2;
  sc_segment * segments[size];

  sc_uint32 read_size;
  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_EQ(read_size, 0u);

  sc_io_channel * channel = sc_io_new_write_channel(SC_FS_MEMORY_SEGMENTS_PATH, nullptr);
  EXPECT_NE(channel, nullptr);
  sc_uint32 const segments_num = 2;
  sc_uint64 written_bytes;
  EXPECT_EQ(
      sc_io_channel_write_chars(channel, &segments_num, sizeof(sc_uint32), &written_bytes, nullptr),
      SC_FS_IO_STATUS_NORMAL);
  sc_char const invalid_segment[] = "segment";
  sc_uint32 const invalid_segment_size = sc_str_len(invalid_segment);
  EXPECT_EQ(
      sc_io_channel_write_chars(channel, invalid_segment, invalid_segment_size, &written_bytes, nullptr),
      SC_FS_IO_STATUS_NORMAL);
  sc_io_channel_shutdown(channel, SC_TRUE, nullptr);

  EXPECT_FALSE(sc_fs_memory_load(segments, &read_size));
  sc_segment_free(segments[0]);
  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_file_write)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));
  EXPECT_TRUE(sc_fs_remove_directory(SC_FS_MEMORY_PATH));

  sc_uint32 size = 2;
  sc_segment * segments[size];
  segments[0] = nullptr;
  segments[1] = nullptr;

  EXPECT_FALSE(sc_fs_memory_save(segments, size));

  sc_uint32 read_size;
  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segments_num_write)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));

  sc_uint32 size = 2;
  sc_segment * segments[size];
  segments[0] = nullptr;
  segments[1] = nullptr;

  EXPECT_FALSE(sc_fs_memory_save(segments, *(sc_uint64 *)"invalid_size"));

  sc_uint32 read_size;
  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_TRUE(sc_fs_memory_shutdown());
}

TEST(ScFSMemoryTest, sc_fs_memory_save_load_save_invalid_segment_write)
{
  EXPECT_TRUE(sc_fs_memory_initialize(SC_FS_MEMORY_PATH, SC_TRUE));

  sc_uint32 size = 2;
  sc_segment * segments[size];
  segments[0] = nullptr;
  segments[1] = nullptr;

  EXPECT_FALSE(sc_fs_memory_save(segments, size));

  sc_uint32 read_size;
  EXPECT_TRUE(sc_fs_memory_load(segments, &read_size));
  EXPECT_TRUE(sc_fs_memory_shutdown());
}
