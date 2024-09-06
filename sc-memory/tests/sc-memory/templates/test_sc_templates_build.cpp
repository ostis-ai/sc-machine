#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_structure.hpp"

#include "template_test_utils.hpp"

using ScTemplateBuildTest = ScTemplateTest;

TEST_F(ScTemplateBuildTest, DoubleAttributes)
{
  /**
   * addr1 _-> addr3:: addr4:: _addr2;;
   */
  ScAddr const addr1 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->GenerateNode(ScType::NodeVar);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const addr3 = m_ctx->GenerateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr3.IsValid());
  ScAddr const addr4 = m_ctx->GenerateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr4.IsValid());

  ScAddr const edge1 = m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr const edge2 = m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, addr3, edge1);
  EXPECT_TRUE(edge2.IsValid());
  ScAddr const edge3 = m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, addr4, edge1);
  EXPECT_TRUE(edge3.IsValid());

  auto const testOrder = [this](std::vector<ScAddr> const & addrs)
  {
    ScAddr const structAddr = m_ctx->GenerateNode(ScType::NodeConstStruct);
    ScStructure st = m_ctx->ConvertToStructure(structAddr);

    for (auto const & a : addrs)
      st << a;

    ScTemplate templ;
    m_ctx->BuildTemplate(templ, structAddr);
  };

  testOrder({addr1, addr2, addr3, addr4, edge1, edge2, edge3});
  testOrder({edge3, edge2, edge1, addr4, addr3, addr2, addr1});
  testOrder({addr1, addr2, addr3, addr4, edge2, edge1, edge3});
}

TEST_F(ScTemplateBuildTest, EdgeFromEdgeToEdge)
{
  /**
   * @edge1 = addr1 _-> _addr2;;
   * @adge1 _-> addr3;;
   */
  ScAddr const addr1 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->GenerateNode(ScType::NodeVar);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const addr3 = m_ctx->GenerateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const edge1 = m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr const edge2 = m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, edge1, addr3);
  EXPECT_TRUE(edge2.IsValid());

  auto const testOrder = [this](std::vector<ScAddr> const & addrs)
  {
    ScAddr const structAddr = m_ctx->GenerateNode(ScType::NodeConstStruct);
    ScStructure st = m_ctx->ConvertToStructure(structAddr);

    for (auto const & a : addrs)
      st << a;

    ScTemplate templ;
    m_ctx->BuildTemplate(templ, structAddr);
  };

  testOrder({addr1, addr2, addr3, edge1, edge2});
  testOrder({edge2, edge1, addr3, addr2, addr1});
  testOrder({addr1, addr2, addr3, edge2, edge1});
}

TEST_F(ScTemplateBuildTest, BuildGenWithParams)
{
  m_ctx->ResolveElementSystemIdentifier("_node", ScType::NodeVar);

  ScTemplate templ;
  m_ctx->BuildTemplate(templ, "_node _-> rrel_1:: _var;;");

  ScAddr const & addr = m_ctx->GenerateNode(ScType::NodeConst);
  ScTemplateParams params;
  params.Add("_node", addr);

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult, params);
  EXPECT_EQ(genResult["_node"], addr);

  ScAddr const & varAddr = m_ctx->SearchElementBySystemIdentifier("_node");
  EXPECT_EQ(genResult[varAddr], addr);
}

TEST_F(ScTemplateBuildTest, GenWithParams)
{
  m_ctx->ResolveElementSystemIdentifier("_node", ScType::NodeVar);

  ScTemplate templ;
  templ(
      ScType::NodeVar >> "_node",
      ScType::EdgeDCommonVar,
      ScType::NodeVar,
      ScType::EdgeAccessVarPosPerm,
      m_ctx->SearchElementBySystemIdentifier("rrel_1"));

  ScAddr const & addr = m_ctx->GenerateNode(ScType::NodeConst);
  ScTemplateParams params;
  params.Add("_node", addr);
  EXPECT_THROW(params.Add("_node", addr), utils::ExceptionInvalidParams);

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult, params);
  EXPECT_EQ(genResult["_node"], addr);
  ScAddr outAddr;
  EXPECT_TRUE(genResult.Get("_node", outAddr));
  EXPECT_EQ(outAddr, addr);
  EXPECT_TRUE(genResult.Has("_node"));

  ScAddr const & varAddr = m_ctx->SearchElementBySystemIdentifier("_node");
  EXPECT_EQ(genResult[varAddr], addr);
  EXPECT_TRUE(genResult.Get(varAddr, outAddr));
  EXPECT_EQ(outAddr, addr);
  EXPECT_TRUE(genResult.Has(varAddr));

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));
  EXPECT_EQ(searchResult[0]["_node"], addr);
  EXPECT_TRUE(searchResult[0].Get("_node", outAddr));
  EXPECT_EQ(outAddr, addr);
  EXPECT_TRUE(searchResult[0].Has("_node"));

  EXPECT_FALSE(searchResult[0].Has("_node1"));

  EXPECT_EQ(searchResult[0][varAddr], addr);
  EXPECT_TRUE(searchResult[0].Get(varAddr, outAddr));
  EXPECT_EQ(outAddr, addr);
  EXPECT_TRUE(searchResult[0].Has(varAddr));

  EXPECT_FALSE(searchResult[0].Has(ScAddr::Empty));
}
