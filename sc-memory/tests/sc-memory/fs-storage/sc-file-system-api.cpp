#include <gtest/gtest.h>
#include "sc-core/sc-store/sc-fs-memory/sc_io.h"

extern "C"
{
#include "sc-core/sc-store/sc-fs-memory/sc_file_system.h"
#include "sc-core/sc-store/sc-base/sc_allocator.h"
#include "sc-core/sc-store/sc-container/sc-string/sc_string.h"
}

TEST(ScFileSystemTest, sc_file_system_create_remove_directory)
{
  sc_char const parent_path[] = "temp";
  sc_char const child_path[] = "temp/child1/child2";
  EXPECT_TRUE(sc_fs_create_directory(child_path));
  EXPECT_TRUE(sc_fs_is_directory(parent_path));
  EXPECT_TRUE(sc_fs_is_directory(child_path));
  EXPECT_TRUE(sc_fs_remove_directory(child_path));
  EXPECT_FALSE(sc_fs_remove_file(child_path));
  EXPECT_TRUE(sc_fs_remove_directory(parent_path));
  EXPECT_FALSE(sc_fs_is_directory(parent_path));
  EXPECT_FALSE(sc_fs_is_directory(child_path));
}

TEST(ScFileSystemTest, sc_file_system_create_invalid_directory)
{
  sc_char const path[] = "";
  EXPECT_FALSE(sc_fs_create_directory(path));
  EXPECT_FALSE(sc_fs_create_directory(nullptr));
}

TEST(ScFileSystemTest, sc_file_system_create_remove_file)
{
  sc_char const parent_path[] = "temp";
  sc_char const child_path[] = "temp/text.txt";
  EXPECT_TRUE(sc_fs_create_directory(parent_path));
  EXPECT_TRUE(sc_fs_create_file(child_path));
  EXPECT_TRUE(sc_fs_is_directory(parent_path));
  EXPECT_FALSE(sc_fs_is_directory(child_path));
  EXPECT_TRUE(sc_fs_is_file(child_path));
  EXPECT_FALSE(sc_fs_remove_directory(child_path));
  EXPECT_TRUE(sc_fs_remove_file(child_path));
  EXPECT_TRUE(sc_fs_remove_directory(parent_path));
  EXPECT_FALSE(sc_fs_is_directory(parent_path));
  EXPECT_FALSE(sc_fs_is_file(child_path));
  EXPECT_FALSE(sc_fs_is_directory(child_path));
}

TEST(ScFileSystemTest, sc_file_system_create_invalid_file)
{
  sc_char const path[] = "";
  EXPECT_FALSE(sc_fs_create_file(path));
  EXPECT_FALSE(sc_fs_create_file(nullptr));
}

TEST(ScFileSystemTest, sc_file_system_execute)
{
  sc_char const path[] = "image.png";
  EXPECT_TRUE(sc_fs_create_file(path));
  sc_char const command[] = "file image.png";
  sc_char * result = sc_fs_execute(command);
  EXPECT_TRUE(sc_str_cmp(result, "image.png: empty\n"));
  sc_mem_free(result);
  EXPECT_TRUE(sc_fs_remove_file(path));
}

TEST(ScFileSystemTest, sc_file_system_read_binary_file)
{
  sc_char const path[] = "image.png";
  sc_io_channel * file = sc_io_new_write_channel(path, nullptr);
  EXPECT_NE(file, nullptr);
  sc_uint64 written_bytes;
  sc_uint16 number = 0x1024;
  EXPECT_EQ(
      sc_io_channel_write_chars(file, (sc_char *)&number, sizeof(sc_uint16), &written_bytes, nullptr),
      SC_FS_IO_STATUS_NORMAL);
  EXPECT_EQ(written_bytes, sizeof(sc_uint16));
  EXPECT_TRUE(sc_fs_is_binary_file(path));
  sc_io_channel_shutdown(file, SC_TRUE, nullptr);

  sc_char * content;
  sc_uint64 content_size;
  sc_fs_get_file_content(path, &content, &content_size);
  EXPECT_TRUE(sc_str_cmp(content, "$\u0010"));
  sc_mem_free(content);
}

TEST(ScFileSystemTest, sc_file_system_read_invalid_file)
{
  sc_char const path[] = "";
  sc_char * content;
  sc_uint64 content_size;
  sc_fs_get_file_content(path, &content, &content_size);
  EXPECT_EQ(content, nullptr);
  sc_fs_get_file_content(nullptr, &content, &content_size);
  EXPECT_EQ(content, nullptr);
}
