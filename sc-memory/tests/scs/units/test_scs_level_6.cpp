/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "test_scs_utils.hpp"

TEST(scs_level_6, set)
{
  std::string const data = "@set = { a; b: c; d: e: f };;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

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

TEST(scs_level_6, smoke)
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
    EXPECT_TRUE(parser.Parse(d));
  }
}

TEST(scs_level_6, content_empty)
{
  std::string const data = "x -> [];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  SPLIT_TRIPLE(triples[0]);

  EXPECT_EQ(src.GetIdtf(), "x");
  EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosPerm);
  EXPECT_EQ(trg.GetType(), ScType::LinkConst);

  EXPECT_EQ(trg.GetValue(), "");
}

TEST(scs_level_6, content_constant)
{
  std::string const data = "x -> [content_const];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  SPLIT_TRIPLE(triples[0]);

  EXPECT_EQ(src.GetIdtf(), "x");
  EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosPerm);
  EXPECT_EQ(trg.GetType(), ScType::LinkConst);

  EXPECT_EQ(trg.GetValue(), "content_const");
}

TEST(scs_level_6, content_var)
{
  std::string const data = "x -> _[var_content];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  auto const & trg = parser.GetParsedElement(triples[0].m_target);

  EXPECT_EQ(trg.GetType(), ScType::LinkVar);
  EXPECT_EQ(trg.GetValue(), "var_content");
}

TEST(scs_level_6, content_escaping)
{
  auto const testContent = [](std::string const & src, std::string const & check)
  {
    scs::Parser parser;

    EXPECT_TRUE(parser.Parse(src));

    auto const & triples = parser.GetParsedTriples();
    EXPECT_EQ(triples.size(), 1u);

    auto const & trg = parser.GetParsedElement(triples[0].m_target);

    EXPECT_EQ(trg.GetValue(), check);
  };

  testContent("x -> _[\\[test\\]];;", "[test]");
  testContent("x -> _[\\\\\\[test\\\\\\]];;", "\\[test\\]");

  {
    std::string const data = "x -> _[\\test]];;";

    scs::Parser parser;

    EXPECT_FALSE(parser.Parse(data));
  }
}

TEST(scs_level_6, content_multiline)
{
  std::string const data = "x -> _[line1\nline2];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  auto const & trg = parser.GetParsedElement(triples[0].m_target);

  EXPECT_EQ(trg.GetValue(), "line1\nline2");
}

TEST(scs_level_6, contour_empty)
{
  std::string const data = "x -> [**];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  SPLIT_TRIPLE(triples[0]);

  EXPECT_EQ(trg.GetType(), ScType::NodeConstStruct);
}


TEST(scs_level_6, contour_parse_error)
{
  {
    std::string const data = "x -> [* y -> z *];;";

    scs::Parser parser;

    EXPECT_FALSE(parser.Parse(data));
  }

  {
    std::string const data = "test -> [* x -> *];;";

    scs::Parser parser;

    EXPECT_FALSE(parser.Parse(data));
  }
}


TEST(scs_level_6, contour_simple)
{
  std::string const data = "x -|> [* y _=> z;; *];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 5u);

  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetIdtf(), "y");
    EXPECT_EQ(edge.GetType(), ScType::EdgeDCommonVar);
    EXPECT_EQ(trg.GetIdtf(), "z");
  }

  for (size_t i = 1; i < 4; ++i)
  {
    auto const & edge = parser.GetParsedElement(triples[i].m_edge);
    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosPerm);

    auto const & src = parser.GetParsedElement(triples[i].m_source);
    EXPECT_EQ(src.GetType(), ScType::NodeConstStruct);
  }

  {
    SPLIT_TRIPLE(triples.back());

    EXPECT_EQ(src.GetIdtf(), "x");
    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstNegPerm);
    EXPECT_EQ(trg.GetType(), ScType::NodeConstStruct);
  }
}

TEST(scs_level_6, contour_recursive)
{
  std::string const data = "x ~|> [* y _=> [* k ~> z;; *];; *];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 15u);

  {
    SPLIT_TRIPLE(triples[0]);
    EXPECT_EQ(src.GetIdtf(), "k");
    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosTemp);
    EXPECT_EQ(trg.GetIdtf(), "z");
  }

  auto const checkStructEdges = [&triples, &parser](size_t idxStart, size_t idxEnd)
  {
    for (size_t i = idxStart; i < idxEnd; ++i)
    {
      auto const & edge = parser.GetParsedElement(triples[i].m_edge);
      EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosPerm);

      auto const & src = parser.GetParsedElement(triples[i].m_source);
      EXPECT_EQ(src.GetType(), ScType::NodeConstStruct);
    }
  };

  checkStructEdges(1, 4);

  {
    SPLIT_TRIPLE(triples[4]);

    EXPECT_EQ(src.GetIdtf(), "y");
    EXPECT_EQ(edge.GetType(), ScType::EdgeDCommonVar);
    EXPECT_EQ(trg.GetType(), ScType::NodeConstStruct);
  }

  checkStructEdges(5, 14);

  {
    SPLIT_TRIPLE(triples[14]);

    EXPECT_EQ(src.GetIdtf(), "x");
    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstNegTemp);
    EXPECT_EQ(trg.GetType(), ScType::NodeConstStruct);
  }
}


TEST(scs_level_6, contout_with_content)
{
  std::string const data = "x -> [* y _=> [test*];; *];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 5u);

  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetIdtf(), "y");
    EXPECT_EQ(edge.GetType(), ScType::EdgeDCommonVar);
    EXPECT_EQ(trg.GetType(), ScType::LinkConst);
    EXPECT_EQ(trg.GetValue(), "test*");
  }
}
