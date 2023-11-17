#include <gtest/gtest.h>

#include "sc-memory/sc_keynodes.hpp"
#include "sc-memory/utils/sc_keynode_cache.hpp"

#include "sc_test.hpp"

using ScKeynodesTest = ScMemoryTest;

TEST_F(ScKeynodesTest, CoreKeynodes)
{
  EXPECT_TRUE(ScKeynodes::kQuestionState.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionState) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kQuestionInitiated.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionInitiated) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kQuestionProgressed.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionProgressed) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kQuestionStopped.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionStopped) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kQuestionFinished.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionFinished) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kQuestionFinishedSuccessfully.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionFinishedSuccessfully) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kQuestionFinishedUnsuccessfully.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionFinishedUnsuccessfully) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kQuestionFinishedWithError.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kQuestionFinishedWithError) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kNrelAnswer.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kNrelAnswer) == ScType::NodeConstNoRole);

  EXPECT_TRUE(ScKeynodes::kScResult.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResult) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultOk.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultOk) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultNo.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultNo) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultUnknown.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultUnknown) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultError.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultError) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultErrorInvalidParams.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultErrorInvalidParams) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultErrorInvalidType.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultErrorInvalidType) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultErrorIO.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultErrorIO) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultInvalidState.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultInvalidState) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kScResultErrorNotFound.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kScResultErrorNotFound) == ScType::NodeConstClass);

  EXPECT_TRUE(ScKeynodes::kBinaryType.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryType) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryFloat.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryFloat) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryDouble.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryDouble) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryInt8.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryInt8) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryInt16.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryInt16) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryInt32.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryInt32) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryInt64.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryInt64) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryUInt8.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryUInt8) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryUInt16.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryUInt16) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryUInt16.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryUInt16) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryUInt64.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryUInt64) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryString.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryString) == ScType::NodeConstClass);
  EXPECT_TRUE(ScKeynodes::kBinaryCustom.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::kBinaryCustom) == ScType::NodeConstClass);
}

TEST_F(ScKeynodesTest, binary_types)
{
  ScAddr keynodes[] = {
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
      ScKeynodes::kBinaryString};

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
