/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>

class ScIterator5Test : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = m_ctx->GenerateNode(ScType::ConstNode);
    m_target = m_ctx->GenerateNode(ScType::VarNode);
    m_connector = m_ctx->GenerateConnector(ScType::ConstPermPosArc, m_source, m_target);

    m_attr = m_ctx->GenerateNode(ScType::ConstNode);
    m_attrConnector = m_ctx->GenerateConnector(ScType::ConstPermPosArc, m_attr, m_connector);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_connector.IsValid());
    ASSERT_TRUE(m_attr.IsValid());
    ASSERT_TRUE(m_attrConnector.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_connector;
  ScAddr m_attr;
  ScAddr m_attrConnector;
};

TEST_F(ScIterator5Test, smoke)
{
  EXPECT_TRUE(m_source.IsValid());
  EXPECT_TRUE(m_target.IsValid());
  EXPECT_TRUE(m_connector.IsValid());
  EXPECT_TRUE(m_attr.IsValid());
  EXPECT_TRUE(m_attrConnector.IsValid());

  EXPECT_TRUE(m_ctx->IsElement(m_source));
  EXPECT_TRUE(m_ctx->IsElement(m_target));
  EXPECT_TRUE(m_ctx->IsElement(m_connector));
  EXPECT_TRUE(m_ctx->IsElement(m_attr));
  EXPECT_TRUE(m_ctx->IsElement(m_attrConnector));
}

TEST_F(ScIterator5Test, InvalidIndex)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(ScType::Node, ScType::ConstPermPosArc, m_target, ScType::ConstPermPosArc, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);
  EXPECT_THROW(iter5->Get(5), utils::ExceptionInvalidParams);

  ScAddrQuintuple quintuple = iter5->Get();
  EXPECT_EQ(quintuple[0], m_source);
  EXPECT_EQ(quintuple[1], m_connector);
  EXPECT_EQ(quintuple[2], m_target);
  EXPECT_EQ(quintuple[3], m_attrConnector);
  EXPECT_EQ(quintuple[4], m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAA)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(ScType::Node, ScType::ConstPermPosArc, m_target, ScType::ConstPermPosArc, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAA2)
{
  ScIterator5Ptr const iter5 = m_ctx->CreateIterator5(
      sc_type_node, sc_type_const_perm_pos_arc, m_target, sc_type_const_perm_pos_arc, sc_type_node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAF)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(ScType::Node, ScType::ConstPermPosArc, m_target, ScType::ConstPermPosArc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAF2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(sc_type_node, sc_type_const_perm_pos_arc, m_target, sc_type_const_perm_pos_arc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAA)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(m_source, ScType::ConstPermPosArc, ScType::Node, ScType::ConstPermPosArc, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAA2)
{
  ScIterator5Ptr const iter5 = m_ctx->CreateIterator5(
      m_source, sc_type_const_perm_pos_arc, sc_type_node, sc_type_const_perm_pos_arc, sc_type_node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAF)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(m_source, ScType::ConstPermPosArc, ScType::Node, ScType::ConstPermPosArc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAF2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(m_source, sc_type_const_perm_pos_arc, sc_type_node, sc_type_const_perm_pos_arc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAA)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(m_source, ScType::ConstPermPosArc, m_target, ScType::ConstPermPosArc, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAA2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(m_source, sc_type_const_perm_pos_arc, m_target, sc_type_const_perm_pos_arc, sc_type_node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAF)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(m_source, ScType::ConstPermPosArc, m_target, ScType::ConstPermPosArc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAF2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(m_source, sc_type_const_perm_pos_arc, m_target, sc_type_const_perm_pos_arc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAAAF)
{
  ScIterator5Ptr const iter5 =
      m_ctx->CreateIterator5(ScType::Node, ScType::ConstPermPosArc, ScType::VarNode, ScType::ConstPermPosArc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAAAF2)
{
  ScIterator5Ptr const iter5 = m_ctx->CreateIterator5(
      sc_type_node, sc_type_const_perm_pos_arc, sc_type_node, sc_type_const_perm_pos_arc, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_connector);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrConnector);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}
