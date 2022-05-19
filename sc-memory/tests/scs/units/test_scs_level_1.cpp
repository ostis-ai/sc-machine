/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "sc-memory/scs/scs_parser.hpp"

#include "test_scs_utils.hpp"

TEST(scs_level_1, dummy)
{
  char const * data = "sc_node#a | sc_edge#e1 | sc_node#b;;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);
  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetType(), ScType::NodeConst);
    EXPECT_EQ(trg.GetType(), ScType::NodeConst);
    EXPECT_EQ(edge.GetType(), ScType::EdgeUCommonConst);
  }
}
