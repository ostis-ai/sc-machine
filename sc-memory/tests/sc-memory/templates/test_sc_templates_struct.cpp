#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "template_test_utils.hpp"

using ScTemplateSearchInStructTest = ScTemplateTest;

TEST_F(ScTemplateSearchInStructTest, SearchInStruct)
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
  ScAddr tyAddr, txAddr, tgAddr, tuAddr, tzAddr, tsAddr, tyuEdgeAddr, txyEdgeAddr, txgEdgeAddr, tzxyEdgeAddr,
      tsxgEdgeAddr;

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

    testStruct << tyAddr << txAddr << tgAddr << tuAddr << tzAddr << tsAddr << tyuEdgeAddr << txyEdgeAddr << txgEdgeAddr
               << tzxyEdgeAddr << tsxgEdgeAddr;
  }

  // add extra edges that not included into struct
  // scs: x -> t: y;;
  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, txAddr, tyAddr);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr edge2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, tzAddr, edge1);
  EXPECT_TRUE(edge2.IsValid());

  {
    ScTemplateSearchResult result;
    SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN
    EXPECT_TRUE(m_ctx->HelperSearchTemplateInStruct(templ, *testStruct, result));
    SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END

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

TEST_F(ScTemplateSearchInStructTest, BuildGenSearchTemplateWithConstantTriple)
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

  ScAddr xAddr, yAddr, zAddr, xyEdgeAddr, zxyEdgeAddr;
  ScStruct templStruct(*m_ctx, templateAddr);
  {
    xAddr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(xAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("x", xAddr));

    yAddr = m_ctx->CreateNode(ScType::NodeVar);
    EXPECT_TRUE(yAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_y", yAddr));

    zAddr = m_ctx->CreateNode(ScType::NodeVar);
    EXPECT_TRUE(zAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_z", zAddr));

    xyEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, xAddr, yAddr);
    EXPECT_TRUE(xyEdgeAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_xyEdge", xyEdgeAddr));

    zxyEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, zAddr, xyEdgeAddr);
    EXPECT_TRUE(zxyEdgeAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_zxyEdge", zxyEdgeAddr));

    templStruct << xAddr << yAddr << zAddr << xyEdgeAddr << zxyEdgeAddr;
  }

  ScAddr yReplAddr, xyEdgeReplAddr;
  ScTemplateParams params;
  {
    yReplAddr = m_ctx->CreateNode(ScType::NodeConst);
    xyEdgeReplAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, xAddr, yReplAddr);
    params.Add(xyEdgeAddr, xyEdgeReplAddr);
    params.Add(yAddr, yReplAddr);
  }

  ScTemplate templ;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, templateAddr, params));

  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));

  EXPECT_EQ(genResult[xyEdgeReplAddr], xyEdgeReplAddr);
  EXPECT_EQ(genResult[yReplAddr], yReplAddr);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);

  EXPECT_EQ(searchResult[0][xyEdgeReplAddr], xyEdgeReplAddr);
  EXPECT_EQ(searchResult[0][yReplAddr], yReplAddr);

  ScAddr const & structureAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScStruct structure(*m_ctx, structureAddr);
  structure << searchResult[0];

  EXPECT_TRUE(structure.HasElement(xyEdgeReplAddr));
  EXPECT_TRUE(structure.HasElement(yReplAddr));
}
