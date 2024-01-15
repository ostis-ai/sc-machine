#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"


TEST(ScTypeTest, nodes)
{
  EXPECT_TRUE(ScType::Node.IsNode());

  EXPECT_TRUE(ScType::NodeConst.IsNode());
  EXPECT_TRUE(ScType::NodeConstAbstract.IsNode());
  EXPECT_TRUE(ScType::NodeConstClass.IsNode());
  EXPECT_TRUE(ScType::NodeConstMaterial.IsNode());
  EXPECT_TRUE(ScType::NodeConstNoRole.IsNode());
  EXPECT_TRUE(ScType::NodeConstRole.IsNode());
  EXPECT_TRUE(ScType::NodeConstStruct.IsNode());
  EXPECT_TRUE(ScType::NodeConstTuple.IsNode());

  EXPECT_TRUE(ScType::NodeVar.IsNode());
  EXPECT_TRUE(ScType::NodeVarAbstract.IsNode());
  EXPECT_TRUE(ScType::NodeVarClass.IsNode());
  EXPECT_TRUE(ScType::NodeVarMaterial.IsNode());
  EXPECT_TRUE(ScType::NodeVarNoRole.IsNode());
  EXPECT_TRUE(ScType::NodeVarRole.IsNode());
  EXPECT_TRUE(ScType::NodeVarStruct.IsNode());
  EXPECT_TRUE(ScType::NodeVarTuple.IsNode());
}

TEST(ScTypeTest, constancy)
{
  EXPECT_FALSE(ScType::Node.IsConst());
  EXPECT_FALSE(ScType::Node.IsVar());

  EXPECT_TRUE(ScType::LinkConst.IsConst());

  EXPECT_TRUE(ScType::NodeConst.IsConst());
  EXPECT_TRUE(ScType::NodeConstAbstract.IsConst());
  EXPECT_TRUE(ScType::NodeConstClass.IsConst());
  EXPECT_TRUE(ScType::NodeConstMaterial.IsConst());
  EXPECT_TRUE(ScType::NodeConstNoRole.IsConst());
  EXPECT_TRUE(ScType::NodeConstRole.IsConst());
  EXPECT_TRUE(ScType::NodeConstStruct.IsConst());
  EXPECT_TRUE(ScType::NodeConstTuple.IsConst());
  EXPECT_TRUE(ScType::EdgeDCommonConst.IsConst());
  EXPECT_TRUE(ScType::EdgeUCommonConst.IsConst());
  EXPECT_TRUE(ScType::EdgeAccessConstFuzPerm.IsConst());
  EXPECT_TRUE(ScType::EdgeAccessConstFuzTemp.IsConst());
  EXPECT_TRUE(ScType::EdgeAccessConstNegPerm.IsConst());
  EXPECT_TRUE(ScType::EdgeAccessConstNegTemp.IsConst());
  EXPECT_TRUE(ScType::EdgeAccessConstPosPerm.IsConst());
  EXPECT_TRUE(ScType::EdgeAccessConstPosTemp.IsConst());

  EXPECT_TRUE(ScType::NodeVar.IsVar());
  EXPECT_TRUE(ScType::LinkVar.IsVar());
  EXPECT_TRUE(ScType::NodeVarAbstract.IsVar());
  EXPECT_TRUE(ScType::NodeVarClass.IsVar());
  EXPECT_TRUE(ScType::NodeVarMaterial.IsVar());
  EXPECT_TRUE(ScType::NodeVarNoRole.IsVar());
  EXPECT_TRUE(ScType::NodeVarRole.IsVar());
  EXPECT_TRUE(ScType::NodeVarStruct.IsVar());
  EXPECT_TRUE(ScType::NodeVarTuple.IsVar());
  EXPECT_TRUE(ScType::EdgeDCommonVar.IsVar());
  EXPECT_TRUE(ScType::EdgeUCommonVar.IsVar());
  EXPECT_TRUE(ScType::EdgeAccessVarFuzPerm.IsVar());
  EXPECT_TRUE(ScType::EdgeAccessVarFuzTemp.IsVar());
  EXPECT_TRUE(ScType::EdgeAccessVarNegPerm.IsVar());
  EXPECT_TRUE(ScType::EdgeAccessVarNegTemp.IsVar());
  EXPECT_TRUE(ScType::EdgeAccessVarPosPerm.IsVar());
  EXPECT_TRUE(ScType::EdgeAccessVarPosTemp.IsVar());
}

TEST(ScTypeTest, edges)
{
  EXPECT_TRUE(ScType::EdgeAccess.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessConstFuzPerm.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessConstFuzTemp.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessConstNegPerm.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessConstNegTemp.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessConstPosPerm.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessConstPosTemp.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessVarFuzPerm.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessVarFuzTemp.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessVarNegPerm.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessVarNegTemp.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessVarPosPerm.IsEdge());
  EXPECT_TRUE(ScType::EdgeAccessVarPosTemp.IsEdge());
  EXPECT_TRUE(ScType::EdgeDCommon.IsEdge());
  EXPECT_TRUE(ScType::EdgeDCommonConst.IsEdge());
  EXPECT_TRUE(ScType::EdgeDCommonVar.IsEdge());
  EXPECT_TRUE(ScType::EdgeUCommon.IsEdge());
  EXPECT_TRUE(ScType::EdgeUCommonConst.IsEdge());
  EXPECT_TRUE(ScType::EdgeUCommonVar.IsEdge());
}

TEST(ScTypeTest, extend)
{
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::Node));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::NodeConst));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::NodeVar));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::NodeMaterial));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::NodeConstMaterial));

  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::Node));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::NodeConst));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::EdgeAccessConstFuzPerm));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::Link));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::EdgeAccess));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::EdgeUCommon));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::EdgeDCommon));

  EXPECT_FALSE(ScType::Node.CanExtendTo(ScType::EdgeAccess));
  EXPECT_FALSE(ScType::NodeAbstract.CanExtendTo(ScType::NodeConstMaterial));
  EXPECT_FALSE(ScType::Link.CanExtendTo(ScType::Node));
  EXPECT_FALSE(ScType::EdgeAccess.CanExtendTo(ScType::EdgeDCommon));
  EXPECT_FALSE(ScType::EdgeAccess.CanExtendTo(ScType::EdgeUCommon));
  EXPECT_FALSE(ScType::EdgeAccess.CanExtendTo(ScType::Link));
  EXPECT_FALSE(ScType::Const.CanExtendTo(ScType::Var));
  EXPECT_FALSE(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessConstFuzTemp));
  EXPECT_FALSE(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessConstNegPerm));
  EXPECT_FALSE(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessVarFuzPerm));
}
