#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"
#include <algorithm>

#include "sc_test.hpp"

void checkConnectionInStruct(
    ScMemoryContext * m_ctx,
    ScAddr const & keynodeAddr,
    ScAddr const & otherKeynodeAddr,
    ScAddr const & resultStruct)
{
  ScIterator3Ptr it3 = m_ctx->Iterator3(keynodeAddr, ScType::EdgeAccessConstPosPerm, otherKeynodeAddr);
  while (it3->Next())
  {
    bool checkEdge = m_ctx->HelperCheckEdge(resultStruct, it3->Get(1), ScType::EdgeAccessConstPosPerm);
    EXPECT_TRUE(checkEdge);
    if (checkEdge == SC_FALSE)
      SC_LOG_ERROR(
          "Edge between %s" + m_ctx->HelperGetSystemIdtf(keynodeAddr) + " and %s" +
          m_ctx->HelperGetSystemIdtf(otherKeynodeAddr) + " doesn't belong to resultStruct");
  };
}

TEST_F(ScMemoryTest, LinkContent)
{
  std::string str("test content string");
  ScStreamPtr const stream = ScStreamMakeRead(str);

  ScAddr const link = m_ctx->CreateLink();

  EXPECT_TRUE(link.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(link));
  EXPECT_TRUE(m_ctx->SetLinkContent(link, stream));

  ScStreamPtr const stream2 = m_ctx->GetLinkContent(link);
  EXPECT_TRUE(m_ctx->GetLinkContent(link));

  EXPECT_EQ(stream->Size(), stream2->Size());
  EXPECT_TRUE(stream2->Seek(SC_STREAM_SEEK_SET, 0));

  std::string str2;
  str2.resize(stream2->Size());
  for (uint32_t i = 0; i < stream2->Size(); ++i)
  {
    sc_char c;
    size_t readBytes;
    EXPECT_TRUE(stream2->Read(&c, sizeof(c), readBytes));
    EXPECT_EQ(readBytes, sizeof(c));

    str2[i] = c;
  }

  EXPECT_EQ(str, str2);

  ScAddrVector const result = m_ctx->FindLinksByContent(stream);
  EXPECT_EQ(result.size(), 1u);
  EXPECT_EQ(result.front(), link);
}

TEST_F(ScMemoryTest, FindByLinkContent)
{
  ScAddr const linkAddr1 = m_ctx->CreateLink();
  EXPECT_TRUE(linkAddr1.IsValid());

  std::string const linkContent1 = "ScMemoryContext_FindLinksByContent_content_1";
  ScLink link1(*m_ctx, linkAddr1);
  EXPECT_TRUE(link1.Set(linkContent1));

  ScAddrVector const result = m_ctx->FindLinksByContent(linkContent1);
  EXPECT_EQ(result.size(), 1u);
}

TEST_F(ScMemoryTest, LinkContentStringApi)
{
  ScAddr const linkAddr1 = m_ctx->CreateLink();
  EXPECT_TRUE(linkAddr1.IsValid());

  std::string str;
  EXPECT_FALSE(m_ctx->GetLinkContent(linkAddr1, str));

  EXPECT_TRUE(m_ctx->SetLinkContent(linkAddr1, "content"));
  EXPECT_TRUE(m_ctx->GetLinkContent(linkAddr1, str));
  EXPECT_TRUE(str == "content");
}

TEST_F(ScMemoryTest, ResolveNodeWithRussianIdtf)
{
  std::string russianIdtf = "узел";
  ScAddr russianNode = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_FALSE(m_ctx->HelperSetSystemIdtf(russianIdtf, russianNode));
  EXPECT_FALSE(m_ctx->HelperFindBySystemIdtf(russianIdtf, russianNode));

  std::string englishIdtf = "russianNode";
  ScAddr englishNode = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf(englishIdtf, englishNode));
  EXPECT_EQ(m_ctx->HelperFindBySystemIdtf(englishIdtf), englishNode);
}

TEST_F(ScMemoryTestWithResultStruct, TestResultStruct)
{
  ScAddr const & resultStruct = m_ctx->HelperFindBySystemIdtf("resultStructure");
  EXPECT_TRUE(resultStruct.IsValid());
  ScMemoryContext * context = m_ctx.get();

  ScAddrVector const & keynodesAddrs = {
      ScKeynodes::kCommandStateAddr,
      ScKeynodes::kCommandInitiatedAddr,
      ScKeynodes::kCommandProgressedAddr,
      ScKeynodes::kCommandFinishedAddr,
      ScKeynodes::kNrelResult,
      ScKeynodes::kNrelCommonTemplate,
      ScKeynodes::kNrelIdtf,
      ScKeynodes::kNrelFormat,
      ScKeynodes::kScResult,
      ScKeynodes::kBinaryType};

  ScAddrVector const & resultCodes = {
      ScKeynodes::kScResultOk,
      ScKeynodes::kScResultNo,
      ScKeynodes::kScResultUnknown,
      ScKeynodes::kScResultError,
      ScKeynodes::kScResultErrorInvalidParams,
      ScKeynodes::kScResultErrorInvalidType,
      ScKeynodes::kScResultErrorIO,
      ScKeynodes::kScResultInvalidState,
      ScKeynodes::kScResultErrorNotFound,
      ScKeynodes::kScResultErrorNoWriteRights,
      ScKeynodes::kScResultErrorNoReadRights};

  ScAddrVector const & binaryTypes = {
      ScKeynodes::kBinaryDouble,
      ScKeynodes::kBinaryFloat,
      ScKeynodes::kBinaryString,
      ScKeynodes::kBinaryInt8,
      ScKeynodes::kBinaryInt16,
      ScKeynodes::kBinaryInt32,
      ScKeynodes::kBinaryInt64,
      ScKeynodes::kBinaryUInt8,
      ScKeynodes::kBinaryUInt16,
      ScKeynodes::kBinaryUInt32,
      ScKeynodes::kBinaryUInt64,
      ScKeynodes::kBinaryCustom};

  std::vector<ScAddrVector> const & keynodesVectors = {keynodesAddrs, resultCodes, binaryTypes};

  ScAddrVector allKeynodes;
  for (ScAddrVector const & keynodes : keynodesVectors)
  {
    allKeynodes.insert(allKeynodes.begin(), keynodes.begin(), keynodes.end());
  }

  SC_LOG_ERROR(allKeynodes.size());

  for (ScAddr const & keynodeAddr : allKeynodes)
  {
    EXPECT_TRUE(m_ctx->HelperCheckEdge(resultStruct, keynodeAddr, ScType::EdgeAccessConstPosPerm));
    std::for_each(
        allKeynodes.begin(), allKeynodes.end(), [context, keynodeAddr, resultStruct](ScAddr otherKeynodeAddr) {
          checkConnectionInStruct(context, keynodeAddr, otherKeynodeAddr, resultStruct);
        });
  }
};