/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"

#include "sc-memory/cpp/sc_stream.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

TEST_CASE("Iterators", "[test iterators]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "iterators");

  ScAddr addr1 = ctx.CreateNode(ScType::Const);
  ScAddr addr2 = ctx.CreateNode(ScType::Var);
  ScAddr arc1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);

  REQUIRE(addr1.IsValid());
  REQUIRE(addr2.IsValid());
  REQUIRE(arc1.IsValid());

  REQUIRE(ctx.IsElement(addr1));
  REQUIRE(ctx.IsElement(addr2));
  REQUIRE(ctx.IsElement(arc1));

  SECTION("iterator3_f_a_f")
  {
    SUBTEST_START("iterator3_f_a_f")
    {
      ScIterator3Ptr iter3 = ctx.Iterator3(addr1, ScType::EdgeAccessConstPosPerm, addr2);
      REQUIRE(iter3->Next());
      REQUIRE(iter3->Get(0) == addr1);
      REQUIRE(iter3->Get(1) == arc1);
      REQUIRE(iter3->Get(2) == addr2);
    }
    SUBTEST_END()
  }

  SECTION("iterator3_f_a_a")
  {
    SUBTEST_START("iterator3_f_a_a")
    {
      ScIterator3Ptr iter3 = ctx.Iterator3(addr1, ScType::EdgeAccessConstPosPerm, ScType::Node);
      REQUIRE(iter3->Next());
      REQUIRE(iter3->Get(0) == addr1);
      REQUIRE(iter3->Get(1) == arc1);
      REQUIRE(iter3->Get(2) == addr2);
    }
    SUBTEST_END()
  }

  SECTION("iterator3_a_a_f")
  {
    SUBTEST_START("iterator3_a_a_f")
    {
      ScIterator3Ptr iter3 = ctx.Iterator3(sc_type_node, sc_type_arc_pos_const_perm, addr2);
      REQUIRE(iter3->Next());
      REQUIRE(iter3->Get(0) == addr1);
      REQUIRE(iter3->Get(1) == arc1);
      REQUIRE(iter3->Get(2) == addr2);
    }
    SUBTEST_END()
  }

  SECTION("iterator3_a_f_a")
  {
    SUBTEST_START("iterator3_a_f_a")
    {
      ScIterator3Ptr iter3 = ctx.Iterator3(sc_type_node, arc1, sc_type_node);
      REQUIRE(iter3->Next());
      REQUIRE(iter3->Get(0) == addr1);
      REQUIRE(iter3->Get(1) == arc1);
      REQUIRE(iter3->Get(2) == addr2);
    }
    SUBTEST_END()
  }

  ScAddr addr3 = ctx.CreateNode(ScType::Const);
  ScAddr arc2 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr3, arc1);

  REQUIRE(addr3.IsValid());
  REQUIRE(arc2.IsValid());

  REQUIRE(ctx.IsElement(addr3));
  REQUIRE(ctx.IsElement(arc2));

  SECTION("iterator5_a_a_f_a_a")
  {
    SUBTEST_START("iterator5_a_a_f_a_a")
    {
      ScIterator5Ptr iter5 = ctx.Iterator5(
            ScType::Node,
            ScType::EdgeAccessConstPosPerm,
            addr2,
            ScType::EdgeAccessConstPosPerm,
            ScType::Node);

      REQUIRE(iter5->Next());

      REQUIRE(iter5->Get(0) == addr1);
      REQUIRE(iter5->Get(1) == arc1);
      REQUIRE(iter5->Get(2) == addr2);
      REQUIRE(iter5->Get(3) == arc2);
      REQUIRE(iter5->Get(4) == addr3);
    }
    SUBTEST_END()
  }

  SECTION("iterator5_a_a_f_a_f")
  {
    SUBTEST_START("iterator5_a_a_f_a_f")
    {
      ScIterator5Ptr iter5 = ctx.Iterator5(
            ScType::Node,
            ScType::EdgeAccessConstPosPerm,
            addr2,
            ScType::EdgeAccessConstPosPerm,
            addr3);

      REQUIRE(iter5->Next());

      REQUIRE(iter5->Get(0) == addr1);
      REQUIRE(iter5->Get(1) == arc1);
      REQUIRE(iter5->Get(2) == addr2);
      REQUIRE(iter5->Get(3) == arc2);
      REQUIRE(iter5->Get(4) == addr3);
    }
    SUBTEST_END()
  }

  SECTION("iterator5_f_a_a_a_a")
  {
    SUBTEST_START("iterator5_f_a_a_a_a")
    {
      ScIterator5Ptr iter5 = ctx.Iterator5(
            addr1,
            ScType::EdgeAccessConstPosPerm,
            ScType::Node,
            ScType::EdgeAccessConstPosPerm,
            ScType::Node);

      REQUIRE(iter5->Next());

      REQUIRE(iter5->Get(0) == addr1);
      REQUIRE(iter5->Get(1) == arc1);
      REQUIRE(iter5->Get(2) == addr2);
      REQUIRE(iter5->Get(3) == arc2);
      REQUIRE(iter5->Get(4) == addr3);
    }
    SUBTEST_END()
  }

  SECTION("iterator5_f_a_a_a_f")
  {
    SUBTEST_START("iterator5_f_a_a_a_f")
    {
      ScIterator5Ptr iter5 = ctx.Iterator5(
            addr1,
            ScType::EdgeAccessConstPosPerm,
            ScType::Node,
            ScType::EdgeAccessConstPosPerm,
            addr3);

      REQUIRE(iter5->Next());

      REQUIRE(iter5->Get(0) == addr1);
      REQUIRE(iter5->Get(1) == arc1);
      REQUIRE(iter5->Get(2) == addr2);
      REQUIRE(iter5->Get(3) == arc2);
      REQUIRE(iter5->Get(4) == addr3);
    }
    SUBTEST_END()
  }

  SECTION("iterator5_f_a_f_a_a")
  {
    SUBTEST_START("iterator5_f_a_f_a_a")
    {
      ScIterator5Ptr iter5 = ctx.Iterator5(
            addr1,
            ScType::EdgeAccessConstPosPerm,
            addr2,
            ScType::EdgeAccessConstPosPerm,
            ScType::Node);

      REQUIRE(iter5->Next());

      REQUIRE(iter5->Get(0) == addr1);
      REQUIRE(iter5->Get(1) == arc1);
      REQUIRE(iter5->Get(2) == addr2);
      REQUIRE(iter5->Get(3) == arc2);
      REQUIRE(iter5->Get(4) == addr3);
    }
    SUBTEST_END()
  }

  SECTION("iterator5_f_a_f_a_f")
  {
    SUBTEST_START("iterator5_f_a_f_a_f")
    {
      ScIterator5Ptr iter5 = ctx.Iterator5(
            addr1,
            ScType::EdgeAccessConstPosPerm,
            addr2,
            ScType::EdgeAccessConstPosPerm,
            addr3);

      REQUIRE(iter5->Next());

      REQUIRE(iter5->Get(0) == addr1);
      REQUIRE(iter5->Get(1) == arc1);
      REQUIRE(iter5->Get(2) == addr2);
      REQUIRE(iter5->Get(3) == arc2);
      REQUIRE(iter5->Get(4) == addr3);
    }
    SUBTEST_END()
  }

  SECTION("content_string")
  {
    SUBTEST_START("content_string")
    {
      std::string str("test content string");
      ScStream stream(str.c_str(), (sc_uint32) str.size(), SC_STREAM_FLAG_READ);
      ScAddr link = ctx.CreateLink();

      REQUIRE(link.IsValid());
      REQUIRE(ctx.IsElement(link));
      REQUIRE(ctx.SetLinkContent(link, stream));

      ScStream stream2;
      REQUIRE(ctx.GetLinkContent(link, stream2));

      REQUIRE(stream.Size() == stream2.Size());
      REQUIRE(stream2.Seek(SC_STREAM_SEEK_SET, 0));

      std::string str2;
      str2.resize(stream2.Size());
      for (sc_uint i = 0; i < stream2.Size(); ++i)
      {
        sc_char c;
        size_t readBytes;
        REQUIRE(stream2.Read(&c, sizeof(c), readBytes));
        REQUIRE(readBytes == sizeof(c));

        str2[i] = c;
      }

      REQUIRE(str == str2);

      ScAddrList result;
      REQUIRE(ctx.FindLinksByContent(stream, result));
      REQUIRE(result.size() == 1);
      REQUIRE(result.front() == link);
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
