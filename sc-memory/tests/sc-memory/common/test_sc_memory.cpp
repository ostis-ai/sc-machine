#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"
#include <algorithm>

#include "sc_test.hpp"

void checkConnectionInStruct(
    ScMemoryContext * m_ctx,
    ScAddr const & keynodeAddr,
    ScAddr const & otherKeynodeAddr,
    ScAddr const & structure)
{
  ScIterator3Ptr const it3 = m_ctx->Iterator3(keynodeAddr, ScType::EdgeAccessConstPosPerm, otherKeynodeAddr);
  while (it3->Next())
  {
    bool checkEdge = m_ctx->CheckConnector(structure, it3->Get(1), ScType::EdgeAccessConstPosPerm);
    EXPECT_TRUE(checkEdge);
    if (checkEdge == SC_FALSE)
      SC_LOG_ERROR(
          "Edge between %s" + m_ctx->GetElementSystemIdentifier(keynodeAddr) + " and %s"
          + m_ctx->GetElementSystemIdentifier(otherKeynodeAddr) + " doesn't belong to struct");
  }
}

bool checkKeynodeInStruct(
    ScMemoryContext * m_ctx,
    ScAddr const & keynodeAddr,
    ScAddr const & kNrelSystemIdtf,
    ScAddr const & structure)
{
  ScIterator5Ptr const it5 = m_ctx->Iterator5(
      keynodeAddr, ScType::EdgeDCommonConst, ScType::LinkConst, ScType::EdgeAccessConstPosPerm, kNrelSystemIdtf);
  bool result = it5->Next();
  if (result)
  {
    for (size_t i = 0; i < 4; i++)
    {
      result &= m_ctx->CheckConnector(structure, it5->Get(i), ScType::EdgeAccessConstPosPerm);
    }
  }

  return result;
}

TEST_F(ScMemoryTest, LinkContent)
{
  std::string str("test content string");
  ScStreamPtr const stream = ScStreamMakeRead(str);

  ScAddr const link = m_ctx->GenerateLink();

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
  ScAddr const linkAddr1 = m_ctx->GenerateLink();
  EXPECT_TRUE(linkAddr1.IsValid());

  std::string const linkContent1 = "ScMemoryContext_FindLinksByContent_content_1";
  ScLink link1(*m_ctx, linkAddr1);
  EXPECT_TRUE(link1.Set(linkContent1));

  ScAddrVector const result = m_ctx->FindLinksByContent(linkContent1);
  EXPECT_EQ(result.size(), 1u);
}

TEST_F(ScMemoryTest, LinkContentStringApi)
{
  ScAddr const linkAddr1 = m_ctx->GenerateLink();
  EXPECT_TRUE(linkAddr1.IsValid());

  std::string str;
  EXPECT_FALSE(m_ctx->GetLinkContent(linkAddr1, str));

  EXPECT_TRUE(m_ctx->SetLinkContent(linkAddr1, "content"));
  EXPECT_TRUE(m_ctx->GetLinkContent(linkAddr1, str));
  EXPECT_EQ(str, "content");
}

TEST_F(ScMemoryTest, ResolveNodeWithRussianIdtf)
{
  std::string russianIdtf = "узел";
  ScAddr russianNode = m_ctx->GenerateNode(ScType::NodeConstClass);
  EXPECT_THROW(m_ctx->SetElementSystemIdentifier(russianIdtf, russianNode), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->SearchElementBySystemIdentifier(russianIdtf, russianNode), utils::ExceptionInvalidParams);

  std::string englishIdtf = "russianNode";
  ScAddr englishNode = m_ctx->GenerateNode(ScType::NodeConstClass);
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier(englishIdtf, englishNode));
  EXPECT_EQ(m_ctx->SearchElementBySystemIdentifier(englishIdtf), englishNode);
}

TEST_F(ScMemoryTest, LinkContentStringWithSpaces)
{
  ScAddr const linkAddr = m_ctx->GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());

  std::string str;
  EXPECT_FALSE(m_ctx->GetLinkContent(linkAddr, str));

  EXPECT_TRUE(m_ctx->SetLinkContent(linkAddr, "content with spaces"));
  EXPECT_TRUE(m_ctx->GetLinkContent(linkAddr, str));
  EXPECT_EQ(str, "content with spaces");
}

static inline ScTemplateKeynode const & testTemplate =
    ScTemplateKeynode("test_template").Triple(ScKeynodes::action_state, ScType::EdgeAccessVarPosPerm, ScType::NodeVar);

TEST_F(ScMemoryTestWithInitMemoryGeneratedStructure, TestInitMemoryGeneratedStructure)
{
  ScAddr const & initMemoryGeneratedStructure = m_ctx->SearchElementBySystemIdentifier("result_structure");
  EXPECT_TRUE(initMemoryGeneratedStructure.IsValid());
  ScAddr const & kNrelSystemIdtf = m_ctx->SearchElementBySystemIdentifier("nrel_system_identifier");
  EXPECT_TRUE(kNrelSystemIdtf.IsValid());
  ScMemoryContext * context = m_ctx.get();

  ScAddrVector const & keynodesAddrs = {ScKeynodes::sc_event, ScKeynodes::binary_type, testTemplate};

  ScAddrVector const & events = {
      ScKeynodes::sc_event_after_generate_incoming_arc,
      ScKeynodes::sc_event_after_generate_outgoing_arc,
      ScKeynodes::sc_event_after_generate_edge,
      ScKeynodes::sc_event_before_erase_incoming_arc,
      ScKeynodes::sc_event_before_erase_outgoing_arc,
      ScKeynodes::sc_event_before_erase_edge,
      ScKeynodes::sc_event_before_erase_element,
      ScKeynodes::sc_event_before_change_link_content,
      ScKeynodes::sc_event_unknown,
  };

  ScAddrVector const & binaryTypes = {
      ScKeynodes::binary_double,
      ScKeynodes::binary_float,
      ScKeynodes::binary_string,
      ScKeynodes::binary_int8,
      ScKeynodes::binary_int16,
      ScKeynodes::binary_int32,
      ScKeynodes::binary_int64,
      ScKeynodes::binary_uint8,
      ScKeynodes::binary_uint16,
      ScKeynodes::binary_uint32,
      ScKeynodes::binary_uint64,
      ScKeynodes::binary_custom};

  std::vector<ScAddrVector> const & keynodesVectors = {keynodesAddrs, events, binaryTypes};

  ScAddrVector allKeynodes;
  for (ScAddrVector const & keynodes : keynodesVectors)
  {
    allKeynodes.insert(allKeynodes.begin(), keynodes.begin(), keynodes.end());
  }

  for (ScAddr const & keynodeAddr : allKeynodes)
  {
    EXPECT_TRUE(checkKeynodeInStruct(context, keynodeAddr, kNrelSystemIdtf, initMemoryGeneratedStructure));
    std::for_each(
        allKeynodes.begin(),
        allKeynodes.end(),
        [context, keynodeAddr, initMemoryGeneratedStructure](ScAddr otherKeynodeAddr)
        {
          checkConnectionInStruct(context, keynodeAddr, otherKeynodeAddr, initMemoryGeneratedStructure);
        });
  }
}
