/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_stream.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

UNIT_TEST(iterators)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "iterators");

  ScAddr addr1 = ctx.CreateNode(sc_type_const);
  ScAddr addr2 = ctx.CreateNode(sc_type_var);
  ScAddr arc1 = ctx.CreateEdge(sc_type_arc_pos_const_perm, addr1, addr2);

  SC_CHECK(addr1.IsValid(), ());
  SC_CHECK(addr2.IsValid(), ());
  SC_CHECK(arc1.IsValid(), ());

  SC_CHECK(ctx.IsElement(addr1), ());
  SC_CHECK(ctx.IsElement(addr2), ());
  SC_CHECK(ctx.IsElement(arc1), ());

  SUBTEST_START(iterator3_f_a_f)
  {
    ScIterator3Ptr iter3 = ctx.Iterator3(addr1, sc_type_arc_pos_const_perm, addr2);
    SC_CHECK(iter3->Next(), ());
    SC_CHECK_EQUAL(iter3->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter3->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter3->Get(2), addr2, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator3_f_a_a)
  {
    ScIterator3Ptr iter3 = ctx.Iterator3(addr1, sc_type_arc_pos_const_perm, sc_type_node);
    SC_CHECK(iter3->Next(), ());
    SC_CHECK_EQUAL(iter3->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter3->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter3->Get(2), addr2, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator3_a_a_f)
  {
    ScIterator3Ptr iter3 = ctx.Iterator3(sc_type_node, sc_type_arc_pos_const_perm, addr2);
    SC_CHECK(iter3->Next(), ());
    SC_CHECK_EQUAL(iter3->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter3->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter3->Get(2), addr2, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator3_a_f_a)
  {
    ScIterator3Ptr iter3 = ctx.Iterator3(sc_type_node, arc1, sc_type_node);
    SC_CHECK(iter3->Next(), ());
    SC_CHECK_EQUAL(iter3->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter3->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter3->Get(2), addr2, ());
  }
  SUBTEST_END()

  ScAddr addr3 = ctx.CreateNode(sc_type_const);
  ScAddr arc2 = ctx.CreateEdge(sc_type_arc_pos_const_perm, addr3, arc1);

  SC_CHECK(addr3.IsValid(), ());
  SC_CHECK(arc2.IsValid(), ());

  SC_CHECK(ctx.IsElement(addr3), ());
  SC_CHECK(ctx.IsElement(arc2), ());

  SUBTEST_START(iterator5_a_a_f_a_a)
  {
    ScIterator5Ptr iter5 = ctx.Iterator5(sc_type_node, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, sc_type_node);

    SC_CHECK(iter5->Next(), ());

    SC_CHECK_EQUAL(iter5->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter5->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter5->Get(2), addr2, ());
    SC_CHECK_EQUAL(iter5->Get(3), arc2, ());
    SC_CHECK_EQUAL(iter5->Get(4), addr3, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator5_a_a_f_a_f)
  {
    ScIterator5Ptr iter5 = ctx.Iterator5(sc_type_node, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, addr3);

    SC_CHECK(iter5->Next(), ());

    SC_CHECK_EQUAL(iter5->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter5->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter5->Get(2), addr2, ());
    SC_CHECK_EQUAL(iter5->Get(3), arc2, ());
    SC_CHECK_EQUAL(iter5->Get(4), addr3, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator5_f_a_a_a_a)
  {
    ScIterator5Ptr iter5 = ctx.Iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, sc_type_node);

    SC_CHECK(iter5->Next(), ());

    SC_CHECK_EQUAL(iter5->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter5->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter5->Get(2), addr2, ());
    SC_CHECK_EQUAL(iter5->Get(3), arc2, ());
    SC_CHECK_EQUAL(iter5->Get(4), addr3, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator5_f_a_a_a_f)
  {
    ScIterator5Ptr iter5 = ctx.Iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, addr3);

    SC_CHECK(iter5->Next(), ());

    SC_CHECK_EQUAL(iter5->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter5->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter5->Get(2), addr2, ());
    SC_CHECK_EQUAL(iter5->Get(3), arc2, ());
    SC_CHECK_EQUAL(iter5->Get(4), addr3, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator5_f_a_f_a_a)
  {
    ScIterator5Ptr iter5 = ctx.Iterator5(addr1, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, sc_type_node);

    SC_CHECK(iter5->Next(), ());

    SC_CHECK_EQUAL(iter5->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter5->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter5->Get(2), addr2, ());
    SC_CHECK_EQUAL(iter5->Get(3), arc2, ());
    SC_CHECK_EQUAL(iter5->Get(4), addr3, ());
  }
  SUBTEST_END()

  SUBTEST_START(iterator5_f_a_f_a_f)
  {
    ScIterator5Ptr iter5 = ctx.Iterator5(addr1, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, addr3);

    SC_CHECK(iter5->Next(), ());

    SC_CHECK_EQUAL(iter5->Get(0), addr1, ());
    SC_CHECK_EQUAL(iter5->Get(1), arc1, ());
    SC_CHECK_EQUAL(iter5->Get(2), addr2, ());
    SC_CHECK_EQUAL(iter5->Get(3), arc2, ());
    SC_CHECK_EQUAL(iter5->Get(4), addr3, ());
  }
  SUBTEST_END()

  SUBTEST_START(content_string)
  {
    std::string str("test content string");
    ScStream stream(str.c_str(), (sc_uint32)str.size(), SC_STREAM_FLAG_READ);
    ScAddr link = ctx.CreateLink();

    SC_CHECK(link.IsValid(), ());
    SC_CHECK(ctx.IsElement(link), ());
    SC_CHECK(ctx.SetLinkContent(link, stream), ());

    ScStream stream2;
    SC_CHECK(ctx.GetLinkContent(link, stream2), ());

    SC_CHECK_EQUAL(stream.Size(), stream2.Size(), ());
    SC_CHECK(stream2.Seek(SC_STREAM_SEEK_SET, 0), ());

    std::string str2;
    str2.resize(stream2.Size());
    for (sc_uint i = 0; i < stream2.Size(); ++i)
    {
      sc_char c;
      size_t readBytes;
      SC_CHECK(stream2.Read(&c, sizeof(c), readBytes), ());
      SC_CHECK_EQUAL(readBytes, sizeof(c), ());

      str2[i] = c;
    }

    SC_CHECK_EQUAL(str, str2, ());

    ScAddrList result;
    SC_CHECK(ctx.FindLinksByContent(stream, result), ());
    SC_CHECK_EQUAL(result.size(), 1, ());
    SC_CHECK_EQUAL(result.front(), link, ());
  }
  SUBTEST_END()

}
