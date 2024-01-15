#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

TEST(ScStreamTest, common)
{
  uint32_t const length = 1024;
  unsigned char buff[length];

  for (uint32_t i = 0; i < length; ++i)
    buff[i] = rand() % 256;

  ScStream stream((sc_char*)buff, length, SC_STREAM_FLAG_READ);

  EXPECT_TRUE(stream.IsValid());
  EXPECT_TRUE(stream.HasFlag(SC_STREAM_FLAG_READ));
  EXPECT_TRUE(stream.HasFlag(SC_STREAM_FLAG_SEEK));
  EXPECT_TRUE(stream.HasFlag(SC_STREAM_FLAG_TELL));
  EXPECT_FALSE(stream.Eof());
  EXPECT_EQ(stream.Pos(), 0u);
  EXPECT_EQ(stream.Size(), length);

  for (uint32_t i = 0; i < length; ++i)
  {
    unsigned char c;
    size_t readBytes;
    EXPECT_TRUE(stream.Read((sc_char*)&c, sizeof(c), readBytes));
    EXPECT_EQ(c, buff[i]);
  }

  EXPECT_TRUE(stream.Eof());
  EXPECT_EQ(stream.Pos(), length);

  // random seek
  static int const seekOpCount = 1000000;
  for (int i = 0; i < seekOpCount; ++i)
  {
    sc_uint32 pos = rand() % length;
    EXPECT_TRUE(stream.Seek(SC_STREAM_SEEK_SET, pos));


    unsigned char c;
    size_t readBytes;
    EXPECT_TRUE(stream.Read((sc_char*)&c, sizeof(c), readBytes));
    EXPECT_EQ(c, buff[pos]);
  }
}

TEST(ScStreamTest, MakeSmoke)
{
  ScStreamPtr stream = ScStreamMakeRead("test");

  stream = ScStreamMakeRead(uint8_t(7));
  stream = ScStreamMakeRead(uint16_t(7));
  stream = ScStreamMakeRead(uint32_t(7));
  stream = ScStreamMakeRead(uint64_t(7));
  stream = ScStreamMakeRead(int8_t(7));
  stream = ScStreamMakeRead(int16_t(7));
  stream = ScStreamMakeRead(int32_t(7));
  stream = ScStreamMakeRead(int64_t(7));
  stream = ScStreamMakeRead(float(7.f));
  stream = ScStreamMakeRead(double(7.0));
}
