/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

UNIT_TEST(streams)
{
	SUBTEST_START(content_streams)
	{
		static int const length = 1024;
		unsigned char buff[length];

		for (int i = 0; i < length; ++i)
			buff[i] = rand() % 256;

		ScStream stream((sc_char*)buff, length, SC_STREAM_FLAG_READ);

		SC_CHECK(stream.isValid(), ());
		SC_CHECK(stream.hasFlag(SC_STREAM_FLAG_READ), ());
		SC_CHECK(stream.hasFlag(SC_STREAM_FLAG_SEEK), ());
		SC_CHECK(stream.hasFlag(SC_STREAM_FLAG_TELL), ());
		SC_CHECK_NOT(stream.eof(), ());
		SC_CHECK_EQUAL(stream.pos(), 0, ());
		SC_CHECK_EQUAL(stream.size(), length, ());

		for (int i = 0; i < length; ++i)
		{
			unsigned char c;
			sc_uint32 readBytes;
			SC_CHECK(stream.read((sc_char*)&c, sizeof(c), readBytes), ());
			SC_CHECK_EQUAL(c, buff[i], ());
		}

		SC_CHECK(stream.eof(), ());
		SC_CHECK_EQUAL(stream.pos(), length, ());

		// random seek
		static int const seekOpCount = 1000000;
		for (int i = 0; i < seekOpCount; ++i)
		{
			sc_uint32 pos = rand() % length;
			SC_CHECK(stream.seek(SC_STREAM_SEEK_SET, pos), ());


			unsigned char c;
			sc_uint32 readBytes;
			SC_CHECK(stream.read((sc_char*)&c, sizeof(c), readBytes), ());
			SC_CHECK_EQUAL(c, buff[pos], ());
		}
	}
	SUBTEST_END
}
