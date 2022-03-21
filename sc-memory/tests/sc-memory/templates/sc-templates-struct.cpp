#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "sc_test.hpp"
#include "template_test_utils.hpp"

using ScTemplateSearchInStructTest = ScTemplateTest;

TEST_F(ScTemplateSearchInStructTest, search_in_struct)
{
  /**
   *   _y
   *    ^
   *    | <--- _z
   *    x
   * scs: x _-> _z:: _y;;
   */

  ScAddr const templateAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(templateAddr.IsValid());

  ScAddr xAddr, _yAddr, _zAddr, _xyEdgeAddr, _zxyEdgeAddr;
  ScStruct templStruct(*m_ctx, templateAddr);
  {
    xAddr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(xAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("x", xAddr));

    _yAddr = m_ctx->CreateNode(ScType::NodeVar);
    EXPECT_TRUE(_yAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_y", _yAddr));

    _zAddr = m_ctx->CreateNode(ScType::NodeVar);
    EXPECT_TRUE(_zAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_z", _zAddr));

    _xyEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, xAddr, _yAddr);
    EXPECT_TRUE(_xyEdgeAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_xyEdge", _xyEdgeAddr));

    _zxyEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, _zAddr, _xyEdgeAddr);
    EXPECT_TRUE(_zxyEdgeAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_zxyEdge", _zxyEdgeAddr));

    templStruct << xAddr << _yAddr << _zAddr << _xyEdgeAddr << _zxyEdgeAddr;
  }

  ScTemplate templ;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, templateAddr));

  // create test struct
  ScAddr const testStructAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(testStructAddr.IsValid());

  /*   y ---> u
  *   ^
  *   | <--- z
  *   x
  *   | <--- s
  *   v
  *   g
  * scs: x -> z: y; s: g;; y -> u;;
  */
  ScAddr tyAddr, txAddr, tgAddr, tuAddr, tzAddr, tsAddr,
         tyuEdgeAddr, txyEdgeAddr, txgEdgeAddr, tzxyEdgeAddr, tsxgEdgeAddr;

  ScStruct testStruct(*m_ctx, testStructAddr);
  {
    txAddr = xAddr;

    tyAddr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(tyAddr.IsValid());

    tgAddr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(tgAddr.IsValid());

    tuAddr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(tuAddr.IsValid());

    tzAddr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(tzAddr.IsValid());

    tsAddr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(tsAddr.IsValid());

    tyuEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, tyAddr, tuAddr);
    EXPECT_TRUE(tyuEdgeAddr.IsValid());

    txyEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, txAddr, tyAddr);
    EXPECT_TRUE(txyEdgeAddr.IsValid());

    txgEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, txAddr, tgAddr);
    EXPECT_TRUE(txgEdgeAddr.IsValid());

    tzxyEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, tzAddr, txyEdgeAddr);
    EXPECT_TRUE(tzxyEdgeAddr.IsValid());

    tsxgEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, tsAddr, txgEdgeAddr);
    EXPECT_TRUE(tsxgEdgeAddr.IsValid());

    testStruct << tyAddr << txAddr << tgAddr
               << tuAddr << tzAddr << tsAddr
               << tyuEdgeAddr << txyEdgeAddr
               << txgEdgeAddr << tzxyEdgeAddr
               << tsxgEdgeAddr;
  }

  // add extra edges that not included into struct
  // scs: x -> t: y;;
  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, txAddr, tyAddr);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr edge2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, tzAddr, edge1);
  EXPECT_TRUE(edge2.IsValid());

  {
    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->HelperSearchTemplateInStruct(templ, *testStruct, result));

    EXPECT_EQ(result.Size(), 2u);
    for (uint32_t i = 0; i < result.Size(); ++i)
    {
      ScTemplateSearchResultItem res1 = result[i];
      EXPECT_EQ(res1["x"], xAddr);
      EXPECT_TRUE(res1["_y"] == tyAddr || res1["_y"] == tgAddr);
      EXPECT_TRUE(res1["_z"] == tzAddr || res1["_z"] == tsAddr);
      EXPECT_TRUE(res1["_xyEdge"] == txyEdgeAddr || res1["_xyEdge"] == txgEdgeAddr);
      EXPECT_TRUE(res1["_zxyEdge"] == tsxgEdgeAddr || res1["_zxyEdge"] == tzxyEdgeAddr);
    }
  }
}

