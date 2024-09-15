#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

class ScIterator3CoreTest : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const);
    m_target = sc_memory_link_new2(**m_ctx, sc_type_node_link | sc_type_const);
    m_connector = sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, m_source, m_target);

    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_source));
    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_target));
    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_connector));
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  sc_addr m_source;
  sc_addr m_target;
  sc_addr m_connector;
};

TEST_F(ScIterator3CoreTest, sc_iterator3_invalid)
{
  sc_iterator3 * it = nullptr;
  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 3)));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScIterator3CoreTest, sc_iterator3_invalid_index)
{
  sc_iterator3 * it =
      sc_iterator3_f_a_a_new(**m_ctx, m_source, sc_type_const_perm_pos_arc, sc_type_node_link | sc_type_const);
  EXPECT_NE(it, nullptr);

  EXPECT_TRUE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 3)));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScIterator3CoreTest, sc_iterator3_f_a_a)
{
  sc_iterator3 * it =
      sc_iterator3_f_a_a_new(**m_ctx, m_source, sc_type_const_perm_pos_arc, sc_type_node_link | sc_type_const);
  EXPECT_NE(it, nullptr);

  EXPECT_TRUE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScIterator3CoreTest, sc_iterator3_f_a_f)
{
  sc_iterator3 * it = sc_iterator3_f_a_f_new(**m_ctx, m_source, sc_type_const_perm_pos_arc, m_target);
  EXPECT_NE(it, nullptr);

  EXPECT_TRUE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScIterator3CoreTest, sc_iterator3_a_a_f)
{
  sc_iterator3 * it =
      sc_iterator3_a_a_f_new(**m_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, m_target);
  EXPECT_NE(it, nullptr);

  EXPECT_TRUE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScIterator3CoreTest, sc_iterator3_f_f_a)
{
  sc_iterator3 * it = sc_iterator3_f_f_a_new(**m_ctx, m_source, m_connector, sc_type_node_link | sc_type_const);
  EXPECT_NE(it, nullptr);

  EXPECT_TRUE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScIterator3CoreTest, sc_iterator3_a_f_f)
{
  sc_iterator3 * it = sc_iterator3_a_f_f_new(**m_ctx, sc_type_node | sc_type_const, m_connector, m_target);
  EXPECT_NE(it, nullptr);

  EXPECT_TRUE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScIterator3CoreTest, sc_iterator3_f_f_f)
{
  sc_iterator3 * it = sc_iterator3_f_f_f_new(**m_ctx, m_source, m_connector, m_target);
  EXPECT_NE(it, nullptr);

  EXPECT_TRUE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator3_value(it, 2), m_target));

  EXPECT_FALSE(sc_iterator3_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator3_value(it, 2)));

  sc_iterator3_free(it);
}

TEST_F(ScMemoryTest, sc_iterator3_search_structure)
{
  sc_addr const structure_addr1 = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const | sc_type_node_structure);
  sc_addr const set_addr1 = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const);
  sc_addr const structure_addr2 = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const | sc_type_node_structure);
  sc_addr const structure_addr3 = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const | sc_type_node_structure);
  sc_addr const set_addr2 = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const);

  sc_addr const node_addr = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const);
  sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, structure_addr1, node_addr);
  sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, set_addr1, node_addr);
  sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, structure_addr2, node_addr);
  sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, structure_addr3, node_addr);
  sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, set_addr2, node_addr);

  sc_iterator3 * it3 = sc_iterator3_a_a_f_new(
      **m_ctx, sc_type_node | sc_type_const | sc_type_node_structure, sc_type_const_perm_pos_arc, node_addr);
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_FALSE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  it3 = sc_iterator3_a_a_f_new(**m_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, node_addr);
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_FALSE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_memory_element_free(**m_ctx, structure_addr2);

  it3 = sc_iterator3_a_a_f_new(
      **m_ctx, sc_type_node | sc_type_const | sc_type_node_structure, sc_type_const_perm_pos_arc, node_addr);
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_FALSE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  it3 = sc_iterator3_a_a_f_new(**m_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, node_addr);
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_FALSE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);
}

class ScIterator5CoreTest : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const);
    m_target = sc_memory_link_new2(**m_ctx, sc_type_node_link | sc_type_const);
    m_connector = sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, m_source, m_target);
    m_attr = sc_memory_node_new(**m_ctx, sc_type_node | sc_type_const);
    m_attrEdge = sc_memory_arc_new(**m_ctx, sc_type_const_perm_pos_arc, m_attr, m_connector);

    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_source));
    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_target));
    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_connector));
    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_attr));
    ASSERT_TRUE(SC_ADDR_IS_NOT_EMPTY(m_attrEdge));
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  sc_addr m_source;
  sc_addr m_target;
  sc_addr m_connector;
  sc_addr m_attr;
  sc_addr m_attrEdge;
};

TEST_F(ScIterator5CoreTest, sc_iterator5_invalid)
{
  sc_iterator5 * it = nullptr;
  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_FALSE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 5)));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_invalid_index)
{
  sc_iterator5 * it = sc_iterator5_f_a_a_a_a_new(
      **m_ctx,
      m_source,
      sc_type_const_perm_pos_arc,
      sc_type_node_link | sc_type_const,
      sc_type_const_perm_pos_arc,
      sc_type_node | sc_type_const);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 5)));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_f_a_a_a_a)
{
  sc_iterator5 * it = sc_iterator5_f_a_a_a_a_new(
      **m_ctx,
      m_source,
      sc_type_const_perm_pos_arc,
      sc_type_node_link | sc_type_const,
      sc_type_const_perm_pos_arc,
      sc_type_node | sc_type_const);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_f_a_f_a_a)
{
  sc_iterator5 * it = sc_iterator5_f_a_f_a_a_new(
      **m_ctx,
      m_source,
      sc_type_const_perm_pos_arc,
      m_target,
      sc_type_const_perm_pos_arc,
      sc_type_node | sc_type_const);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_f_a_f_a_f)
{
  sc_iterator5 * it = sc_iterator5_f_a_f_a_f_new(
      **m_ctx, m_source, sc_type_const_perm_pos_arc, m_target, sc_type_const_perm_pos_arc, m_attr);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_f_a_a_a_f)
{
  sc_iterator5 * it = sc_iterator5_f_a_a_a_f_new(
      **m_ctx,
      m_source,
      sc_type_const_perm_pos_arc,
      sc_type_node_link | sc_type_const,
      sc_type_const_perm_pos_arc,
      m_attr);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_a_a_f_a_a)
{
  sc_iterator5 * it = sc_iterator5_a_a_f_a_a_new(
      **m_ctx,
      sc_type_node | sc_type_const,
      sc_type_const_perm_pos_arc,
      m_target,
      sc_type_const_perm_pos_arc,
      sc_type_node | sc_type_const);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_a_a_f_a_f)
{
  sc_iterator5 * it = sc_iterator5_a_a_f_a_f_new(
      **m_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, m_target, sc_type_const_perm_pos_arc, m_attr);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}

TEST_F(ScIterator5CoreTest, sc_iterator5_a_a_a_a_f)
{
  sc_iterator5 * it = sc_iterator5_a_a_a_a_f_new(
      **m_ctx,
      sc_type_node | sc_type_const,
      sc_type_const_perm_pos_arc,
      sc_type_node_link | sc_type_const,
      sc_type_const_perm_pos_arc,
      m_attr);
  EXPECT_NE(it, nullptr);
  EXPECT_TRUE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 0), m_source));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 1), m_connector));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 2), m_target));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 3), m_attrEdge));
  EXPECT_TRUE(SC_ADDR_IS_EQUAL(sc_iterator5_value(it, 4), m_attr));

  EXPECT_FALSE(sc_iterator5_next(it));

  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 0)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 1)));
  EXPECT_TRUE(SC_ADDR_IS_EMPTY(sc_iterator5_value(it, 2)));

  sc_iterator5_free(it);
}
