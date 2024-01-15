#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

class ScIterator3Test : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = m_ctx->CreateNode(ScType::NodeConst);
    m_target = m_ctx->CreateNode(ScType::NodeVar);
    m_edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_source, m_target);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_edge.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_edge;
};

TEST_F(ScIterator3Test, Smoke)
{
  EXPECT_TRUE(m_source.IsValid());
  EXPECT_TRUE(m_target.IsValid());
  EXPECT_TRUE(m_edge.IsValid());

  EXPECT_TRUE(m_ctx->IsElement(m_source));
  EXPECT_TRUE(m_ctx->IsElement(m_target));
  EXPECT_TRUE(m_ctx->IsElement(m_edge));
}

TEST_F(ScIterator3Test, InvalidIndex)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeAccessConstPosPerm, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);
  EXPECT_THROW(iter3->Get(3), utils::ExceptionInvalidParams);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FAF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeAccessConstPosPerm, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FAA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeAccessConstPosPerm, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AAF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(sc_type_node, sc_type_arc_pos_const_perm, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AFA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(sc_type_node, m_edge, sc_type_node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FFA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, m_edge, sc_type_node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, AFF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(sc_type_node, m_edge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScIterator3Test, FFF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, m_edge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
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

    m_source = m_ctx->CreateNode(ScType::NodeConst);
    m_target = m_ctx->CreateNode(ScType::NodeVar);
    m_edge = m_ctx->CreateEdge(ScType::EdgeUCommonConst, m_source, m_target);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_edge.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_edge;
};

TEST_F(ScEdgeTest, FAF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeUCommonConst, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FAA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeUCommonConst, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FAAReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_target, ScType::EdgeUCommonConst, ScType::Const);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AAF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Const, ScType::EdgeUCommonConst, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AAFReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Node, ScType::EdgeUCommonConst, m_source);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, m_edge, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFAReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_target, m_edge, ScType::Const);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AFF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Const, m_edge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AFFReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Node, m_edge, m_source);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_source);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, AFA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Node, m_edge, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_TRUE(iter3->Get(0) == m_source || iter3->Get(0) == m_target);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_TRUE(iter3->Get(2) == m_source || iter3->Get(2) == m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, m_edge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScEdgeTest, FFFReverse)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_target, m_edge, m_source);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_target);
  EXPECT_EQ(iter3->Get(1), m_edge);
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

    m_source = m_ctx->CreateNode(ScType::NodeConst);
    m_target = m_source;
    m_edge = m_ctx->CreateEdge(ScType::EdgeUCommonConst, m_source, m_target);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_edge.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_edge;
};

TEST_F(ScLoopTest, FAF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeUCommonConst, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, FAA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeUCommonConst, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, AAF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Const, ScType::EdgeUCommonConst, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, FFA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, m_edge, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, AFF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Const, m_edge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, AFA)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(ScType::Node, m_edge, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_TRUE(iter3->Get(0) == m_source || iter3->Get(0) == m_target);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_TRUE(iter3->Get(2) == m_source || iter3->Get(2) == m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}

TEST_F(ScLoopTest, FFF)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, m_edge, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);

  EXPECT_FALSE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter3->Get(2), ScAddr::Empty);
}
