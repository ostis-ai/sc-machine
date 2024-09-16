#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

TEST(ScTypeTest, Nodes)
{
  EXPECT_TRUE(ScType::Node.IsNode());

  EXPECT_TRUE(ScType::ConstNode.IsNode());
  EXPECT_TRUE(ScType::ConstNodeClass.IsNode());
  EXPECT_TRUE(ScType::ConstNodeMaterial.IsNode());
  EXPECT_TRUE(ScType::ConstNodeNoRole.IsNode());
  EXPECT_TRUE(ScType::ConstNodeRole.IsNode());
  EXPECT_TRUE(ScType::ConstNodeStructure.IsNode());
  EXPECT_TRUE(ScType::ConstNodeTuple.IsNode());

  EXPECT_TRUE(ScType::VarNode.IsNode());
  EXPECT_TRUE(ScType::VarNodeClass.IsNode());
  EXPECT_TRUE(ScType::VarNodeMaterial.IsNode());
  EXPECT_TRUE(ScType::VarNodeNoRole.IsNode());
  EXPECT_TRUE(ScType::VarNodeRole.IsNode());
  EXPECT_TRUE(ScType::VarNodeStructure.IsNode());
  EXPECT_TRUE(ScType::VarNodeTuple.IsNode());
}

TEST(ScTypeTest, Constancy)
{
  EXPECT_FALSE(ScType::Node.IsConst());
  EXPECT_FALSE(ScType::Node.IsVar());

  EXPECT_TRUE(ScType::ConstNodeLink.IsConst());

  EXPECT_TRUE(ScType::ConstNode.IsConst());
  EXPECT_TRUE(ScType::ConstNodeClass.IsConst());
  EXPECT_TRUE(ScType::ConstNodeMaterial.IsConst());
  EXPECT_TRUE(ScType::ConstNodeNoRole.IsConst());
  EXPECT_TRUE(ScType::ConstNodeRole.IsConst());
  EXPECT_TRUE(ScType::ConstNodeStructure.IsConst());
  EXPECT_TRUE(ScType::ConstNodeTuple.IsConst());
  EXPECT_TRUE(ScType::ConstCommonArc.IsConst());
  EXPECT_TRUE(ScType::ConstCommonEdge.IsConst());
  EXPECT_TRUE(ScType::ConstFuzArc.IsConst());
  EXPECT_TRUE(ScType::ConstFuzArc.IsConst());
  EXPECT_TRUE(ScType::ConstPermNegArc.IsConst());
  EXPECT_TRUE(ScType::ConstTempNegArc.IsConst());
  EXPECT_TRUE(ScType::ConstPermPosArc.IsConst());
  EXPECT_TRUE(ScType::ConstTempPosArc.IsConst());

  EXPECT_TRUE(ScType::VarNode.IsVar());
  EXPECT_TRUE(ScType::VarNodeLink.IsVar());
  EXPECT_TRUE(ScType::VarNodeClass.IsVar());
  EXPECT_TRUE(ScType::VarNodeMaterial.IsVar());
  EXPECT_TRUE(ScType::VarNodeNoRole.IsVar());
  EXPECT_TRUE(ScType::VarNodeRole.IsVar());
  EXPECT_TRUE(ScType::VarNodeStructure.IsVar());
  EXPECT_TRUE(ScType::VarNodeTuple.IsVar());
  EXPECT_TRUE(ScType::VarCommonArc.IsVar());
  EXPECT_TRUE(ScType::VarCommonEdge.IsVar());
  EXPECT_TRUE(ScType::VarFuzArc.IsVar());
  EXPECT_TRUE(ScType::VarFuzArc.IsVar());
  EXPECT_TRUE(ScType::VarPermNegArc.IsVar());
  EXPECT_TRUE(ScType::VarTempNegArc.IsVar());
  EXPECT_TRUE(ScType::VarPermPosArc.IsVar());
  EXPECT_TRUE(ScType::VarTempPosArc.IsVar());
}

TEST(ScTypeTest, Connectors)
{
  EXPECT_TRUE(ScType::MembershipArc.IsConnector());
  EXPECT_TRUE(ScType::ConstFuzArc.IsConnector());
  EXPECT_TRUE(ScType::ConstFuzArc.IsConnector());
  EXPECT_TRUE(ScType::ConstPermNegArc.IsConnector());
  EXPECT_TRUE(ScType::ConstTempNegArc.IsConnector());
  EXPECT_TRUE(ScType::ConstPermPosArc.IsConnector());
  EXPECT_TRUE(ScType::ConstTempPosArc.IsConnector());
  EXPECT_TRUE(ScType::VarFuzArc.IsConnector());
  EXPECT_TRUE(ScType::VarFuzArc.IsConnector());
  EXPECT_TRUE(ScType::VarPermNegArc.IsConnector());
  EXPECT_TRUE(ScType::VarTempNegArc.IsConnector());
  EXPECT_TRUE(ScType::VarPermPosArc.IsConnector());
  EXPECT_TRUE(ScType::VarTempPosArc.IsConnector());
  EXPECT_TRUE(ScType::CommonArc.IsConnector());
  EXPECT_TRUE(ScType::ConstCommonArc.IsConnector());
  EXPECT_TRUE(ScType::VarCommonArc.IsConnector());
  EXPECT_TRUE(ScType::CommonEdge.IsConnector());
  EXPECT_TRUE(ScType::ConstCommonEdge.IsConnector());
  EXPECT_TRUE(ScType::VarCommonEdge.IsConnector());
}

TEST(ScTypeTest, ExtendTypes)
{
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::Node));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::ConstNode));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::VarNode));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::NodeMaterial));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::ConstNodeMaterial));

  EXPECT_FALSE(ScType::Node.CanExtendTo(ScType::MembershipArc));

  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::NodeLink));
  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::ConstNodeLink));
  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::VarNodeLink));
  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::NodeLinkClass));

  EXPECT_FALSE(ScType::NodeLink.CanExtendTo(ScType::Node));

  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::Node));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::ConstNode));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::ConstFuzArc));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::NodeLink));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::MembershipArc));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::CommonEdge));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::CommonArc));

  EXPECT_TRUE(ScType::MembershipArc.CanExtendTo(ScType::ConstPermPosArc));
  EXPECT_TRUE(ScType::CommonArc.CanExtendTo(ScType::ConstCommonArc));

  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::CommonArc));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::CommonEdge));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::NodeLink));
  EXPECT_FALSE(ScType::Const.CanExtendTo(ScType::Var));
  EXPECT_TRUE(ScType::ConstFuzArc.CanExtendTo(ScType::ConstFuzArc));
  EXPECT_FALSE(ScType::ConstFuzArc.CanExtendTo(ScType::ConstPermNegArc));
  EXPECT_FALSE(ScType::ConstFuzArc.CanExtendTo(ScType::VarFuzArc));
}
