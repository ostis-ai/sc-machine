/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "sc-memory/scs/scs_parser.hpp"

#include "test_scs_utils.hpp"

TEST(scs_level_1, SimpleTriple)
{
  sc_char const * data = "sc_node#a | sc_edge#e1 | sc_node#b;;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);
  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetType(), ScType::NodeConst);
    EXPECT_EQ(trg.GetType(), ScType::NodeConst);
    EXPECT_EQ(arcAddr.GetType(), ScType::EdgeUCommonConst);
  }
}

TEST(scs_level_1, TripleWithLinkUrl)
{
  sc_char const * data = "sc_node#a | sc_edge#e1 | \"file://my_file.txt\";;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);
  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetType(), ScType::NodeConst);
    EXPECT_EQ(trg.GetType(), ScType::LinkConst);
    EXPECT_TRUE(trg.IsURL());
    EXPECT_EQ(trg.GetValue(), "file://my_file.txt");
    EXPECT_EQ(arcAddr.GetType(), ScType::EdgeUCommonConst);
  }
}
