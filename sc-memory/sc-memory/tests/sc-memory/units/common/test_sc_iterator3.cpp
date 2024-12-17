/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>

class ScIterator3Test : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = m_ctx->GenerateNode(ScType::ConstNode);
    m_target = m_ctx->GenerateNode(ScType::VarNode);
    m_connector = m_ctx->GenerateConnector(ScType::ConstPermPosArc, m_source, m_target);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_connector.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_connector;
};

TEST_F(ScIterator3Test, Smoke)
{
  EXPECT_TRUE(m_source.IsValid());
  EXPECT_TRUE(m_target.IsValid());
  EXPECT_TRUE(m_connector.IsValid());

  EXPECT_TRUE(m_ctx->IsElement(m_source));
  EXPECT_TRUE(m_ctx->IsElement(m_target));
  EXPECT_TRUE(m_ctx->IsElement(m_connector));
}

TEST_F(ScIterator3Test, InvalidIndex)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstPermPosArc, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);
  EXPECT_THROW(iter3->Get(3), utils::ExceptionInvalidParams);

  ScAddrTriple triple = iter3->Get();
  EXPECT_EQ(triple[0], m_source);
  EXPECT_EQ(triple[1], m_connector);
  EXPECT_EQ(triple[2], m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FAF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstPermPosArc, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FAF2)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, sc_type_const_perm_pos_arc, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FAA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstPermPosArc, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FAA2)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, sc_type_const_perm_pos_arc, sc_type_node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AAF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Node, ScType::ConstPermPosArc, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AAF2)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(sc_type_node, sc_type_const_perm_pos_arc, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AFA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Node, m_connector, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AFA2)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(sc_type_node, m_connector, sc_type_node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FFA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, m_connector, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FFA2)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, m_connector, sc_type_node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AFF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Node, m_connector, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AFF2)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(sc_type_node, m_connector, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FFF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, m_connector, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

class ScEdgeTest : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = m_ctx->GenerateNode(ScType::ConstNode);
    m_target = m_ctx->GenerateNode(ScType::VarNode);
    m_connector = m_ctx->GenerateConnector(ScType::ConstCommonEdge, m_source, m_target);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_connector.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_connector;
};

TEST_F(ScEdgeTest, FAF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstCommonEdge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FAA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstCommonEdge, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FAAReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_target, ScType::ConstCommonEdge, ScType::Const);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AAF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Const, ScType::ConstCommonEdge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AAFReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Node, ScType::ConstCommonEdge, m_source);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, m_connector, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFAReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_target, m_connector, ScType::Const);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AFF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Const, m_connector, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AFFReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Node, m_connector, m_source);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AFA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Node, m_connector, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_TRUE(iter3->Get(0) == m_source || iter3->Get(0) == m_target);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_TRUE(iter3->Get(2) == m_source || iter3->Get(2) == m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, m_connector, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFFReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_target, m_connector, m_source);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

class ScLoopTest : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = m_ctx->GenerateNode(ScType::ConstNode);
    m_target = m_source;
    m_connector = m_ctx->GenerateConnector(ScType::ConstCommonEdge, m_source, m_target);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_connector.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_connector;
};

TEST_F(ScLoopTest, FAF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstCommonEdge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, FAA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstCommonEdge, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, AAF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Const, ScType::ConstCommonEdge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, FFA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, m_connector, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, AFF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Const, m_connector, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, AFA)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(ScType::Node, m_connector, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_TRUE(iter3->Get(0) == m_source || iter3->Get(0) == m_target);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_TRUE(iter3->Get(2) == m_source || iter3->Get(2) == m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, FFF)
{
  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, m_connector, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_connector);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, CheckConnectorsGeneratedAfterEdgeLoop)
{
  ScAddr const & otherNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const & arcAddr1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, m_source, otherNodeAddr);
  ScAddr const & arcAddr2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, otherNodeAddr, m_source);

  {
    ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstCommonEdge, m_target);
    EXPECT_TRUE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), m_source);
    EXPECT_EQ(iter3->Get(1), m_connector);
    EXPECT_EQ(iter3->Get(2), m_target);

    EXPECT_FALSE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
  }

  {
    ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstPermPosArc, otherNodeAddr);
    EXPECT_TRUE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), m_source);
    EXPECT_EQ(iter3->Get(1), arcAddr1);
    EXPECT_EQ(iter3->Get(2), otherNodeAddr);

    EXPECT_FALSE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
  }

  {
    ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(otherNodeAddr, ScType::ConstPermPosArc, m_source);
    EXPECT_TRUE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), otherNodeAddr);
    EXPECT_EQ(iter3->Get(1), arcAddr2);
    EXPECT_EQ(iter3->Get(2), m_source);

    EXPECT_FALSE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
  }
}

TEST_F(ScLoopTest, CheckConnectorsGeneratedBeforeEdgeLoop)
{
  ScAddr const & otherNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const & arcAddr1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, m_source, otherNodeAddr);
  ScAddr const & arcAddr2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, otherNodeAddr, m_source);

  ScAddr const & edgeAddr2 = m_ctx->GenerateConnector(ScType::ConstCommonEdge, m_target, m_source);

  {
    ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstCommonEdge, m_target);
    EXPECT_TRUE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), m_source);
    EXPECT_EQ(iter3->Get(1), edgeAddr2);
    EXPECT_EQ(iter3->Get(2), m_target);

    EXPECT_TRUE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), m_source);
    EXPECT_EQ(iter3->Get(1), m_connector);
    EXPECT_EQ(iter3->Get(2), m_target);
  }

  {
    ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(m_source, ScType::ConstPermPosArc, otherNodeAddr);
    EXPECT_TRUE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), m_source);
    EXPECT_EQ(iter3->Get(1), arcAddr1);
    EXPECT_EQ(iter3->Get(2), otherNodeAddr);

    EXPECT_FALSE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
  }

  {
    ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(otherNodeAddr, ScType::ConstPermPosArc, m_source);
    EXPECT_TRUE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), otherNodeAddr);
    EXPECT_EQ(iter3->Get(1), arcAddr2);
    EXPECT_EQ(iter3->Get(2), m_source);

    EXPECT_FALSE(iter3->Next());

    EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
    EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
  }
}
