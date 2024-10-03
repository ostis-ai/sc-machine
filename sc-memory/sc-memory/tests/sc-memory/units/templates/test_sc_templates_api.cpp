/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_structure.hpp>

#include "template_test_utils.hpp"

using ScTemplateApiTest = ScTemplateTest;

TEST_F(ScTemplateApiTest, TripleWithConstSourceType)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::ConstNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::Unknown >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithSourceAndEdgeEqualNames)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::VarNode >> "_arc", ScType::VarPermPosArc >> "_arc", ScType::Unknown >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithTargetAndEdgeEqualNames)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::Unknown >> "_arc"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithTargetAndSourceEqualNames)
{
  ScTemplate templ;
  EXPECT_NO_THROW(
      templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::Unknown >> "_addr1"));
}

TEST_F(ScTemplateApiTest, TripleWithInvalidSourceAddr)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScAddr::Empty >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::Unknown >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithInvalidTargetAddr)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScAddr::Empty >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, FiverWithConstSourceType)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Quintuple(
          ScType::ConstNode >> "_addr1",
          ScType::VarPermPosArc >> "_arc",
          ScType::Unknown >> "_addr2",
          ScType::VarPermPosArc,
          ScType::VarNodeNoRole >> "_relation"),
      utils::ExceptionInvalidParams);
}
