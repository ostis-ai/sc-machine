#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_structure.hpp"

#include "template_test_utils.hpp"

using ScTemplateApiTest = ScTemplateTest;

TEST_F(ScTemplateApiTest, TripleWithConstSourceType)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::NodeConst >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_arc", ScType::Unknown >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithSourceAndEdgeEqualNames)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::NodeVar >> "_arc", ScType::EdgeAccessVarPosPerm >> "_arc", ScType::Unknown >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithTargetAndEdgeEqualNames)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_arc", ScType::Unknown >> "_arc"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithTargetAndSourceEqualNames)
{
  ScTemplate templ;
  EXPECT_NO_THROW(
      templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_arc", ScType::Unknown >> "_addr1"));
}

TEST_F(ScTemplateApiTest, TripleWithInvalidSourceAddr)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScAddr::Empty >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_arc", ScType::Unknown >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, TripleWithInvalidTargetAddr)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_arc", ScAddr::Empty >> "_addr2"),
      utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateApiTest, FiverWithConstSourceType)
{
  ScTemplate templ;
  EXPECT_THROW(
      templ.Quintuple(
          ScType::NodeConst >> "_addr1",
          ScType::EdgeAccessVarPosPerm >> "_arc",
          ScType::Unknown >> "_addr2",
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVarNoRole >> "_relation"),
      utils::ExceptionInvalidParams);
}
