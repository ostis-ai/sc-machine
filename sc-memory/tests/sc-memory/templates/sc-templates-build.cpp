#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "sc_test.hpp"
#include "template_test_utils.hpp"

using ScTemplateBuildTest = ScTemplateTest;

TEST_F(ScTemplateBuildTest, double_attrs)
{
  /**
   * addr1 _-> addr3:: addr4:: _addr2;;
   */
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeVar);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const addr3 = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr3.IsValid());
  ScAddr const addr4 = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr4.IsValid());

  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr const edge2 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr3, edge1);
  EXPECT_TRUE(edge2.IsValid());
  ScAddr const edge3 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr4, edge1);
  EXPECT_TRUE(edge3.IsValid());

  auto const testOrder = [this](std::vector<ScAddr> const & addrs)
  {
    ScAddr const structAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
    ScStruct st(*m_ctx, structAddr);

    for (auto const & a : addrs)
      st << a;

    ScTemplate templ;
    EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, structAddr));
  };

  testOrder({ addr1, addr2, addr3, addr4, edge1, edge2, edge3 });
  testOrder({ edge3, edge2, edge1, addr4, addr3, addr2, addr1 });
  testOrder({ addr1, addr2, addr3, addr4, edge2, edge1, edge3 });
}


TEST_F(ScTemplateBuildTest, edge_from_edge)
{
  /**
   * @edge1 = addr1 _-> _addr2;;
   * @adge1 _-> addr3;;
   */
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeVar);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const addr3 = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr const edge2 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, edge1, addr3);
  EXPECT_TRUE(edge2.IsValid());

  auto const testOrder = [this](std::vector<ScAddr> const & addrs)
  {
    ScAddr const structAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
    ScStruct st(*m_ctx, structAddr);

    for (auto const & a : addrs)
      st << a;

    ScTemplate templ;
    EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, structAddr));
  };

  testOrder({ addr1, addr2, addr3, edge1, edge2 });
  testOrder({ edge2, edge1, addr3, addr2, addr1 });
  testOrder({ addr1, addr2, addr3, edge2, edge1 });
}
