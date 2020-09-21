/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-memory/sc_stream.hpp"
#include "sc-test-framework/sc_test_unit.hpp"

TEST_CASE("ScStream", "[test_streams]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  SECTION("content_streams")
  {
    SUBTEST_START("content_streams")
    {
      try
      {
        static int const length = 1024;
        unsigned char buff[length];

        for (int i = 0; i < length; ++i)
          buff[i] = rand() % 256;

        ScStream stream((sc_char *) buff, length, SC_STREAM_FLAG_READ);

        REQUIRE(stream.IsValid());
        REQUIRE(stream.HasFlag(SC_STREAM_FLAG_READ));
        REQUIRE(stream.HasFlag(SC_STREAM_FLAG_SEEK));
        REQUIRE(stream.HasFlag(SC_STREAM_FLAG_TELL));
        REQUIRE_FALSE(stream.Eof());
        REQUIRE(stream.Pos() == 0);
        REQUIRE(stream.Size() == length);

        for (int i = 0; i < length; ++i)
        {
          unsigned char c;
          size_t readBytes;
          REQUIRE(stream.Read((sc_char *) &c, sizeof(c), readBytes));
          REQUIRE(c == buff[i]);
        }

        REQUIRE(stream.Eof());
        REQUIRE(stream.Pos() == length);

        // random seek
        static int const seekOpCount = 1000000;
        for (int i = 0; i < seekOpCount; ++i)
        {
          sc_uint32 pos = rand() % length;
          REQUIRE(stream.Seek(SC_STREAM_SEEK_SET, pos));

          unsigned char c;
          size_t readBytes;
          REQUIRE(stream.Read((sc_char *) &c, sizeof(c), readBytes));
          REQUIRE(c == buff[pos]);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"content_streams\" failed")
      }
    }
    SUBTEST_END()
  }

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("ScStream_Make_Smoke", "[test_streams]")
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
