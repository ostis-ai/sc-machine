/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_stream.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

UNIT_TEST(ScStream)
{
  SUBTEST_START(content_streams)
  {
    static int const length = 1024;
    unsigned char buff[length];

    for (int i = 0; i < length; ++i)
      buff[i] = rand() % 256;

    ScStream stream((sc_char*)buff, length, SC_STREAM_FLAG_READ);

    SC_CHECK(stream.IsValid(), ());
    SC_CHECK(stream.HasFlag(SC_STREAM_FLAG_READ), ());
    SC_CHECK(stream.HasFlag(SC_STREAM_FLAG_SEEK), ());
    SC_CHECK(stream.HasFlag(SC_STREAM_FLAG_TELL), ());
    SC_CHECK_NOT(stream.Eof(), ());
    SC_CHECK_EQUAL(stream.Pos(), 0, ());
    SC_CHECK_EQUAL(stream.Size(), length, ());

    for (int i = 0; i < length; ++i)
    {
      unsigned char c;
      size_t readBytes;
      SC_CHECK(stream.Read((sc_char*)&c, sizeof(c), readBytes), ());
      SC_CHECK_EQUAL(c, buff[i], ());
    }

    SC_CHECK(stream.Eof(), ());
    SC_CHECK_EQUAL(stream.Pos(), length, ());

    // random seek
    static int const seekOpCount = 1000000;
    for (int i = 0; i < seekOpCount; ++i)
    {
      sc_uint32 pos = rand() % length;
      SC_CHECK(stream.Seek(SC_STREAM_SEEK_SET, pos), ());


      unsigned char c;
      size_t readBytes;
      SC_CHECK(stream.Read((sc_char*)&c, sizeof(c), readBytes), ());
      SC_CHECK_EQUAL(c, buff[pos], ());
    }
  }
  SUBTEST_END()
}

UNIT_TEST(ScStream_Make_Smoke)
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
