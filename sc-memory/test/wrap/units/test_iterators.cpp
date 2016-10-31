/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

UNIT_TEST(iterators)
{
	ScMemoryContext ctx;
	ScAddr addr1 = ctx.createNode(sc_type_const);
	ScAddr addr2 = ctx.createNode(sc_type_var);
	ScAddr arc1 = ctx.createEdge(sc_type_arc_pos_const_perm, addr1, addr2);

	SC_CHECK(addr1.isValid(), ());
	SC_CHECK(addr2.isValid(), ());
	SC_CHECK(arc1.isValid(), ());

	SC_CHECK(ctx.isElement(addr1), ());
	SC_CHECK(ctx.isElement(addr2), ());
	SC_CHECK(ctx.isElement(arc1), ());

	SUBTEST_START(iterator3_f_a_f)
	{
		ScIterator3Ptr iter3 = ctx.iterator3(addr1, sc_type_arc_pos_const_perm, addr2);
		SC_CHECK(iter3->next(), ());
		SC_CHECK_EQUAL(iter3->value(0), addr1, ());
		SC_CHECK_EQUAL(iter3->value(1), arc1, ());
		SC_CHECK_EQUAL(iter3->value(2), addr2, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator3_f_a_a)
	{
		ScIterator3Ptr iter3 = ctx.iterator3(addr1, sc_type_arc_pos_const_perm, sc_type_node);
		SC_CHECK(iter3->next(), ());
		SC_CHECK_EQUAL(iter3->value(0), addr1, ());
		SC_CHECK_EQUAL(iter3->value(1), arc1, ());
		SC_CHECK_EQUAL(iter3->value(2), addr2, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator3_a_a_f)
	{
		ScIterator3Ptr iter3 = ctx.iterator3(sc_type_node, sc_type_arc_pos_const_perm, addr2);
		SC_CHECK(iter3->next(), ());
		SC_CHECK_EQUAL(iter3->value(0), addr1, ());
		SC_CHECK_EQUAL(iter3->value(1), arc1, ());
		SC_CHECK_EQUAL(iter3->value(2), addr2, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator3_a_f_a)
	{
		ScIterator3Ptr iter3 = ctx.iterator3(sc_type_node, arc1, sc_type_node);
		SC_CHECK(iter3->next(), ());
		SC_CHECK_EQUAL(iter3->value(0), addr1, ());
		SC_CHECK_EQUAL(iter3->value(1), arc1, ());
		SC_CHECK_EQUAL(iter3->value(2), addr2, ());
	}
	SUBTEST_END

		ScAddr addr3 = ctx.createNode(sc_type_const);
	ScAddr arc2 = ctx.createEdge(sc_type_arc_pos_const_perm, addr3, arc1);

	SC_CHECK(addr3.isValid(), ());
	SC_CHECK(arc2.isValid(), ());

	SC_CHECK(ctx.isElement(addr3), ());
	SC_CHECK(ctx.isElement(arc2), ());

	SUBTEST_START(iterator5_a_a_f_a_a)
	{
		ScIterator5Ptr iter5 = ctx.iterator5(sc_type_node, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, sc_type_node);

		SC_CHECK(iter5->next(), ());

		SC_CHECK_EQUAL(iter5->value(0), addr1, ());
		SC_CHECK_EQUAL(iter5->value(1), arc1, ());
		SC_CHECK_EQUAL(iter5->value(2), addr2, ());
		SC_CHECK_EQUAL(iter5->value(3), arc2, ());
		SC_CHECK_EQUAL(iter5->value(4), addr3, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator5_a_a_f_a_f)
	{
		ScIterator5Ptr iter5 = ctx.iterator5(sc_type_node, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, addr3);

		SC_CHECK(iter5->next(), ());

		SC_CHECK_EQUAL(iter5->value(0), addr1, ());
		SC_CHECK_EQUAL(iter5->value(1), arc1, ());
		SC_CHECK_EQUAL(iter5->value(2), addr2, ());
		SC_CHECK_EQUAL(iter5->value(3), arc2, ());
		SC_CHECK_EQUAL(iter5->value(4), addr3, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator5_f_a_a_a_a)
	{
		ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, sc_type_node);

		SC_CHECK(iter5->next(), ());

		SC_CHECK_EQUAL(iter5->value(0), addr1, ());
		SC_CHECK_EQUAL(iter5->value(1), arc1, ());
		SC_CHECK_EQUAL(iter5->value(2), addr2, ());
		SC_CHECK_EQUAL(iter5->value(3), arc2, ());
		SC_CHECK_EQUAL(iter5->value(4), addr3, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator5_f_a_a_a_f)
	{
		ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, addr3);

		SC_CHECK(iter5->next(), ());

		SC_CHECK_EQUAL(iter5->value(0), addr1, ());
		SC_CHECK_EQUAL(iter5->value(1), arc1, ());
		SC_CHECK_EQUAL(iter5->value(2), addr2, ());
		SC_CHECK_EQUAL(iter5->value(3), arc2, ());
		SC_CHECK_EQUAL(iter5->value(4), addr3, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator5_f_a_f_a_a)
	{
		ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, sc_type_node);

		SC_CHECK(iter5->next(), ());

		SC_CHECK_EQUAL(iter5->value(0), addr1, ());
		SC_CHECK_EQUAL(iter5->value(1), arc1, ());
		SC_CHECK_EQUAL(iter5->value(2), addr2, ());
		SC_CHECK_EQUAL(iter5->value(3), arc2, ());
		SC_CHECK_EQUAL(iter5->value(4), addr3, ());
	}
	SUBTEST_END

		SUBTEST_START(iterator5_f_a_f_a_f)
	{
		ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, addr3);

		SC_CHECK(iter5->next(), ());

		SC_CHECK_EQUAL(iter5->value(0), addr1, ());
		SC_CHECK_EQUAL(iter5->value(1), arc1, ());
		SC_CHECK_EQUAL(iter5->value(2), addr2, ());
		SC_CHECK_EQUAL(iter5->value(3), arc2, ());
		SC_CHECK_EQUAL(iter5->value(4), addr3, ());
	}
	SUBTEST_END

	SUBTEST_START(content_string)
	{
		std::string str("test content string");
		ScStream stream(str.c_str(), (sc_uint32)str.size(), SC_STREAM_FLAG_READ);
		ScAddr link = ctx.createLink();

		SC_CHECK(link.isValid(), ());
		SC_CHECK(ctx.isElement(link), ());
		SC_CHECK(ctx.setLinkContent(link, stream), ());

		ScStream stream2;
		SC_CHECK(ctx.getLinkContent(link, stream2), ());

		SC_CHECK_EQUAL(stream.size(), stream2.size(), ());
		SC_CHECK(stream2.seek(SC_STREAM_SEEK_SET, 0), ());

		std::string str2;
		str2.resize(stream2.size());
		for (sc_uint i = 0; i < stream2.size(); ++i)
		{
			sc_char c;
			sc_uint32 readBytes;
			SC_CHECK(stream2.read(&c, sizeof(c), readBytes), ());
			SC_CHECK_EQUAL(readBytes, sizeof(c), ());

			str2[i] = c;
		}

		SC_CHECK_EQUAL(str, str2, ());

		tAddrList result;
		SC_CHECK(ctx.findLinksByContent(stream, result), ());
		SC_CHECK_EQUAL(result.size(), 1, ());
		SC_CHECK_EQUAL(result.front(), link, ());
	}
	SUBTEST_END

}
