/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "test_scs_utils.hpp"

TEST(scs_level_5, simple)
{
  std::string const data = "set ~> attr:: item (* -/> subitem;; <= subitem2;; *);;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  TripleTester tester(parser);
  tester({
           {
             { ScType::NodeConst, "item" },
             { ScType::EdgeAccessConstFuzPerm, scs::Visibility::Local },
             { ScType::NodeConst, "subitem" }
           },
           {
             { ScType::NodeConst, "subitem2" },
             { ScType::EdgeDCommonConst, scs::Visibility::Local },
             { ScType::NodeConst, "item" }
           },
           {
             { ScType::NodeConst, "set" },
             { ScType::EdgeAccessConstPosTemp, scs::Visibility::Local },
             { ScType::NodeConst, "item" }
           },
           {
             { ScType::NodeConst, "attr" },
             { ScType::EdgeAccessVarPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosTemp, scs::Visibility::Local }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 4u);

  EXPECT_EQ(triples[3].m_target, triples[2].m_edge);
}
