/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "test_scs_utils.hpp"

TEST_CASE("scs_level_6_set", "[test scs level 6]")
{
  SECTION("base")
  {
    SUBTEST_START("base")
    {
      std::string const data = "@set = { a; b: c; d: e: f };;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      TripleTester tester(parser);
      tester({
                   {
                         { ScType::NodeConstTuple, scs::Visibility::Local },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                         { ScType::NodeConst,              "a" }
                   },
                   {
                         { ScType::NodeConstTuple, scs::Visibility::Local },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                         { ScType::NodeConst,              "c" }
                   },
                   {
                         { ScType::NodeConst,      "b" },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                   },
                   {
                         { ScType::NodeConstTuple, scs::Visibility::Local },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                         { ScType::NodeConst,              "f" }
                   },
                   {
                         { ScType::NodeConst,      "d" },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                   },
                   {
                         { ScType::NodeConst,      "e" },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                         { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                   }
             });
    }
    SUBTEST_END()
  }
}

TEST_CASE("scs_level_6_smoke", "[test scs level 6]")
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
    REQUIRE(parser.Parse(d));
  }
}

TEST_CASE("scs_level_6_content", "[test scs level 6]")
{
  SECTION("constant")
  {
    SUBTEST_START("constant")
    {
      std::string const data = "x -> [content_const];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 1);

      SPLIT_TRIPLE(triples[0]);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("x"));
      REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
      REQUIRE(trg.GetType() == ScType::LinkConst);
      REQUIRE_THAT(trg.GetValue(), Catch::Equals("content_const"));
    }
    SUBTEST_END()
  }

  SECTION("empty")
  {
    SUBTEST_START("empty")
    {
      std::string const data = "x -> [];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 1);

      SPLIT_TRIPLE(triples[0]);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("x"));
      REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
      REQUIRE(trg.GetType() == ScType::LinkConst);
      REQUIRE_THAT(trg.GetValue(), Catch::Equals(""));
    }
    SUBTEST_END()
  }

  SECTION("var")
  {
    SUBTEST_START("var")
    {
      std::string const data = "x -> _[var_content];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 1);

      auto const & trg = parser.GetParsedElement(triples[0].m_target);

      REQUIRE(trg.GetType() == ScType::LinkVar);
      REQUIRE_THAT(trg.GetValue(), Catch::Equals("var_content"));
    }
    SUBTEST_END()
  }

  auto const testContent = [](std::string const & src, std::string const & check)
  {
    scs::Parser parser;

    REQUIRE(parser.Parse(src));

    auto const & triples = parser.GetParsedTriples();
    REQUIRE(triples.size() == 1);

    auto const & trg = parser.GetParsedElement(triples[0].m_target);

    REQUIRE(trg.GetValue() == check);
  };

  SECTION("escape")
  {
    SUBTEST_START("escape")
    {
      testContent("x -> _[\\[test\\]];;", "[test]");
    }
    SUBTEST_END()
  }

  SECTION("escape_sequence")
  {
    SUBTEST_START("escape_sequence")
    {
      testContent("x -> _[\\\\\\[test\\\\\\]];;", "\\[test\\]");
    }
    SUBTEST_END()
  }

  SECTION("escape_error")
  {
    SUBTEST_START("escape_error")
    {
      std::string const data = "x -> _[\\test]];;";
      scs::Parser parser;
      REQUIRE_FALSE(parser.Parse(data));
      SC_LOG_WARNING(parser.GetParseError());
    }
    SUBTEST_END()
  }

  SECTION("multiline")
  {
    SUBTEST_START("multiline")
    {
      std::string const data = "x -> _[line1\nline2];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 1);

      auto const & trg = parser.GetParsedElement(triples[0].m_target);

      REQUIRE_THAT(trg.GetValue(), Catch::Equals("line1\nline2"));
    }
    SUBTEST_END()
  }
}

TEST_CASE("scs_level_6_contour", "[test scs level 6]")
{
  SECTION("empty")
  {
    SUBTEST_START("empty")
    {
      std::string const data = "x -> [**];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 1);

      SPLIT_TRIPLE(triples[0]);

      REQUIRE(trg.GetType() == ScType::NodeConstStruct);
    }
    SUBTEST_END()
  }

  SECTION("err")
  {
    SUBTEST_START("err")
    {
      std::string const data = "x -> [* y -> z *];;";

      scs::Parser parser;

      REQUIRE_FALSE(parser.Parse(data));
      SC_LOG_WARNING(parser.GetParseError());
    }
    SUBTEST_END()
  }

  SECTION("base")
  {
    SUBTEST_START("base")
    {
      std::string const data = "x -|> [* y _=> z;; *];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 5);

      {
        SPLIT_TRIPLE(triples[0]);

        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("y"));
        REQUIRE(edge.GetType() == ScType::EdgeDCommonVar);
        REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("z"));
      }

      for (size_t i = 1; i < 4; ++i)
      {
        auto const & edge = parser.GetParsedElement(triples[i].m_edge);
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);

        auto const & src = parser.GetParsedElement(triples[i].m_source);
        REQUIRE(src.GetType() == ScType::NodeConstStruct);
      }

      {
        SPLIT_TRIPLE(triples.back());

        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("x"));
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstNegPerm);
        REQUIRE(trg.GetType() == ScType::NodeConstStruct);
      }
    }
    SUBTEST_END()
  }

  SECTION("recursive")
  {
    SUBTEST_START("recursive")
    {
      std::string const data = "x ~|> [* y _=> [* k ~> z;; *];; *];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 15);

      {
        SPLIT_TRIPLE(triples[0]);
        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("k"));
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosTemp);
        REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("z"));
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

        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("x"));
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstNegTemp);
        REQUIRE(trg.GetType() == ScType::NodeConstStruct);
      }
    }
    SUBTEST_END()
  }

  SECTION("aliases")
  {
    SUBTEST_START("aliases")
    {
      std::string const data = "@alias = _[];; x -> [* @alias2 = y;; @alias _~> @alias2;;*];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 5);

      {
        SPLIT_TRIPLE(triples[0]);

        REQUIRE(src.GetType() == ScType::LinkVar);
        REQUIRE(edge.GetType() == ScType::EdgeAccessVarPosTemp);
        REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("y"));
      }
    }
    SUBTEST_END()
  }

    SECTION("changing_type")
    {
        SUBTEST_START("changing_type")
        {
            std::string const data =
                    "a -> b;;"
                    "sc_node_struct -> a;;";

            scs::Parser parser;

            REQUIRE(parser.Parse(data));

            auto const & triples = parser.GetParsedTriples();
            REQUIRE(triples.size() == 1);

            {
                SPLIT_TRIPLE(triples[0]);

                REQUIRE(src.GetType() == ScType::NodeConstStruct);
            }
        }
        SUBTEST_END()
    }

  SECTION("content")
  {
    SUBTEST_START("content")
    {
      std::string const data = "x -> [* y _=> [test*];; *];;";

      scs::Parser parser;

      REQUIRE(parser.Parse(data));

      auto const & triples = parser.GetParsedTriples();
      REQUIRE(triples.size() == 5);

      {
        SPLIT_TRIPLE(triples[0]);

        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("y"));
        REQUIRE(edge.GetType() == ScType::EdgeDCommonVar);
        REQUIRE(trg.GetType() == ScType::LinkConst);
        REQUIRE_THAT(trg.GetValue(), Catch::Equals("test*"));
      }
    }
    SUBTEST_END()
  }

  SECTION("contour_error")
  {
    SUBTEST_START("contour_error")
    {
      std::string const data = "test -> [* x -> *];;";

      scs::Parser parser;

      REQUIRE_FALSE(parser.Parse(data));
      SC_LOG_WARNING(parser.GetParseError());
    }
    SUBTEST_END()
  }
}
