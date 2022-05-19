#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

class ScIterator3Test : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = m_ctx->CreateNode(ScType::Const);
    m_target = m_ctx->CreateNode(ScType::Var);
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

TEST_F(ScIterator3Test, smoke)
{
  EXPECT_TRUE(m_source.IsValid());
  EXPECT_TRUE(m_target.IsValid());
  EXPECT_TRUE(m_edge.IsValid());

  EXPECT_TRUE(m_ctx->IsElement(m_source));
  EXPECT_TRUE(m_ctx->IsElement(m_target));
  EXPECT_TRUE(m_ctx->IsElement(m_edge));
}

TEST_F(ScIterator3Test, f_a_f)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeAccessConstPosPerm, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);
}


TEST_F(ScIterator3Test, f_a_a)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(m_source, ScType::EdgeAccessConstPosPerm, ScType::Node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);
}

TEST_F(ScIterator3Test, a_a_f)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(sc_type_node, sc_type_arc_pos_const_perm, m_target);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);
}

TEST_F(ScIterator3Test, a_f_a)
{
  ScIterator3Ptr const iter3 = m_ctx->Iterator3(sc_type_node, m_edge, sc_type_node);
  EXPECT_TRUE(iter3->Next());

  EXPECT_EQ(iter3->Get(0), m_source);
  EXPECT_EQ(iter3->Get(1), m_edge);
  EXPECT_EQ(iter3->Get(2), m_target);
}
