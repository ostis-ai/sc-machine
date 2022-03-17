#include <gtest/gtest.h>

#include "sc-memory/sc_keynodes.hpp"
#include "sc-memory/utils/sc_keynode_cache.hpp"

#include "sc_test.hpp"

using ScKeynodesTest = ScMemoryTest;

TEST_F(ScKeynodesTest, binary_types)
{
  ScAddr keynodes[] =
  {
    ScKeynodes::kBinaryDouble,
    ScKeynodes::kBinaryFloat,
    ScKeynodes::kBinaryInt8,
    ScKeynodes::kBinaryInt16,
    ScKeynodes::kBinaryInt32,
    ScKeynodes::kBinaryInt64,
    ScKeynodes::kBinaryUInt8,
    ScKeynodes::kBinaryUInt16,
    ScKeynodes::kBinaryUInt32,
    ScKeynodes::kBinaryUInt64,
    ScKeynodes::kBinaryString
  };

  for (ScAddr a : keynodes)
    EXPECT_TRUE(m_ctx->HelperCheckEdge(ScKeynodes::kBinaryType, a, ScType::EdgeAccessConstPosPerm));
}

TEST_F(ScKeynodesTest, cache)
{
  utils::ScKeynodeCache cache(*m_ctx);

  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(addr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("test_idtf", addr));

  EXPECT_TRUE(cache.GetKeynode("test_idtf").IsValid());
  EXPECT_FALSE(cache.GetKeynode("other").IsValid());
  EXPECT_FALSE(cache.GetKeynode("any_idtf").IsValid());
}
