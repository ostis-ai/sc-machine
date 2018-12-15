/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"

#include "test_scs_utils.hpp"

UNIT_TEST(scs_level_6_set)
{
  SUBTEST_START(base)
  {
    std::string const data = "@set = { a; b: c; d: e: f };;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    TripleTester tester(parser);
    tester({
             {
               { ScType::NodeConstTuple, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::NodeConst, "a" }
             },
             {
               { ScType::NodeConstTuple, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::NodeConst, "c" }
             },
             {
               { ScType::NodeConst, "b" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
             },
             {
               { ScType::NodeConstTuple, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::NodeConst, "f" }
             },
             {
               { ScType::NodeConst, "d" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
             },
             {
               { ScType::NodeConst, "e" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
             }
           });
  }
  SUBTEST_END()
}

UNIT_TEST(scs_level_6_smoke)
{
  std::vector<std::string> data = {
    "z -> [**];;",
    "x -> [test*];;",
    "@a = [\\[* r-> b;; *\\]];;",
    "@alias = u;; @alias -> [* x -> [* y -> z;; *];; *];;",
    "y <= nrel_main_idtf: [y*];;",
    "a -> [* z -> [begin*];; *];;",
    "a -> [* b -> c;; *];;"
  };

  for (auto const & d : data)
  {
    scs::Parser parser;
    SC_CHECK(parser.Parse(d), (parser.GetParseError()));
  }
}

UNIT_TEST(scs_level_6_content)
{
  SUBTEST_START(constant)
  {
    std::string const data = "x -> [content_const];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    SPLIT_TRIPLE(triples[0]);

    SC_CHECK_EQUAL(src.GetIdtf(), "x", ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::LinkConst, ());

    SC_CHECK_EQUAL(trg.GetValue(), "content_const", ());
  }
  SUBTEST_END()

  SUBTEST_START(empty)
  {
    std::string const data = "x -> [];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    SPLIT_TRIPLE(triples[0]);

    SC_CHECK_EQUAL(src.GetIdtf(), "x", ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::LinkConst, ());

    SC_CHECK_EQUAL(trg.GetValue(), "", ());
  }
  SUBTEST_END()

  SUBTEST_START(var)
  {
    std::string const data = "x -> _[var_content];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    auto const & trg = parser.GetParsedElement(triples[0].m_target);

    SC_CHECK_EQUAL(trg.GetType(), ScType::LinkVar, ());
    SC_CHECK_EQUAL(trg.GetValue(), "var_content", ());
  }
  SUBTEST_END()


  auto const testContent = [](std::string const & src, std::string const & check)
  {
    scs::Parser parser;

    SC_CHECK(parser.Parse(src), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    auto const & trg = parser.GetParsedElement(triples[0].m_target);

    SC_CHECK_EQUAL(trg.GetValue(), check, ());
  };

  SUBTEST_START(escape)
  {
    testContent("x -> _[\\[test\\]];;", "[test]");
  }
  SUBTEST_END()

  SUBTEST_START(escape_sequence)
  {
    testContent("x -> _[\\\\\\[test\\\\\\]];;", "\\[test\\]");
  }
  SUBTEST_END()

  SUBTEST_START(escape_error)
  {
    std::string const data = "x -> _[\\test]];;";

    scs::Parser parser;

    SC_CHECK(!parser.Parse(data), (parser.GetParseError()));
  }
  SUBTEST_END()

  SUBTEST_START(multiline)
  {
    std::string const data = "x -> _[line1\nline2];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    auto const & trg = parser.GetParsedElement(triples[0].m_target);

    SC_CHECK_EQUAL(trg.GetValue(), "line1\nline2", ());
  }
  SUBTEST_END()
}

UNIT_TEST(scs_level_6_contour)
{
  SUBTEST_START(empty)
  {
    std::string const data = "x -> [**];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    SPLIT_TRIPLE(triples[0]);

    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConstStruct, ());
  }
  SUBTEST_END()

  SUBTEST_START(base)
  {
    std::string const data = "x -|> [* y _=> z;; *];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 5, ());

    {
      SPLIT_TRIPLE(triples[0]);

      SC_CHECK_EQUAL(src.GetIdtf(), "y", ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeDCommonVar, ());
      SC_CHECK_EQUAL(trg.GetIdtf(), "z", ());
    }

    for (size_t i = 1; i < 4; ++i)
    {
      auto const & edge = parser.GetParsedElement(triples[i].m_edge);
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());

      auto const & src = parser.GetParsedElement(triples[i].m_source);
      SC_CHECK_EQUAL(src.GetType(), ScType::NodeConstStruct, ());
    }

    {
      SPLIT_TRIPLE(triples.back());

      SC_CHECK_EQUAL(src.GetIdtf(), "x", ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstNegPerm, ());
      SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConstStruct, ());
    }
  }
  SUBTEST_END()

  SUBTEST_START(recursive)
  {
    std::string const data = "x ~|> [* y _=> [* k ~> z;; *];; *];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 15, ());

    {
      SPLIT_TRIPLE(triples[0]);
      SC_CHECK_EQUAL(src.GetIdtf(), "k", ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosTemp, ());
      SC_CHECK_EQUAL(trg.GetIdtf(), "z", ());
    }

    auto const checkStructEdges = [&triples, &parser](size_t idxStart, size_t idxEnd)
    {
      for (size_t i = idxStart; i < idxEnd; ++i)
      {
        auto const & edge = parser.GetParsedElement(triples[i].m_edge);
        SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());

        auto const & src = parser.GetParsedElement(triples[i].m_source);
        SC_CHECK_EQUAL(src.GetType(), ScType::NodeConstStruct, ());
      }
    };

    checkStructEdges(1, 4);

    {
      SPLIT_TRIPLE(triples[4]);

      SC_CHECK_EQUAL(src.GetIdtf(), "y", ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeDCommonVar, ());
      SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConstStruct, ());
    }

    checkStructEdges(5, 14);

    {
      SPLIT_TRIPLE(triples[14]);

      SC_CHECK_EQUAL(src.GetIdtf(), "x", ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstNegTemp, ());
      SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConstStruct, ());
    }
  }
  SUBTEST_END()

  SUBTEST_START(aliases)
  {
    std::string const data = "@alias = _[];; x -> [* @alias2 = y;; @alias _~> @alias2;;*];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 5, ());

    {
      SPLIT_TRIPLE(triples[0]);

      SC_CHECK_EQUAL(src.GetType(), ScType::LinkVar, ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessVarPosTemp, ());
      SC_CHECK_EQUAL(trg.GetIdtf(), "y", ());
    }
  }
  SUBTEST_END()

  SUBTEST_START(content)
  {
    std::string const data = "x -> [* y _=> [test*];; *];;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 5, ());

    {
      SPLIT_TRIPLE(triples[0]);

      SC_CHECK_EQUAL(src.GetIdtf(), "y", ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeDCommonVar, ());
      SC_CHECK_EQUAL(trg.GetType(), ScType::LinkConst, ());
      SC_CHECK_EQUAL(trg.GetValue(), "test*", ());
    }
  }
  SUBTEST_END()
}
