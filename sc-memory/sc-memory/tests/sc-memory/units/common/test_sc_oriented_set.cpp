/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_oriented_set.hpp>

using ScOrientedSetTest = ScMemoryTest;

TEST_F(ScOrientedSetTest, AppendAndOrder)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);
  ScAddr c = m_ctx->GenerateNode(ScType::ConstNode);

  EXPECT_TRUE(set.Append(a));
  EXPECT_TRUE(set.Append(b));
  EXPECT_TRUE(set.Append(c));

  set.Reset();
  ScAddrVector order;
  for (int i = 0; i < 3; ++i)
  {
    ScAddr Next = set.Next();
    if (Next.IsValid())
      order.push_back(Next);
  }

  ASSERT_EQ(order.size(), 3u);
  EXPECT_EQ(order[0], a);
  EXPECT_EQ(order[1], b);
  EXPECT_EQ(order[2], c);
}

TEST_F(ScOrientedSetTest, AppendWithRole)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr element = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr role = m_ctx->GenerateNode(ScType::ConstNodeRole);

  EXPECT_TRUE(set.Append(element, role));

  set.Reset();
  ScAddrUnorderedSet roles;
  ScAddr Next = set.Next(roles);
  EXPECT_EQ(Next, element);
  EXPECT_EQ(roles.size(), 3u);
  EXPECT_TRUE(roles.count(role));
}

TEST_F(ScOrientedSetTest, GetLast)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  set.Append(b);

  ScAddr last = set.GetLast();
  EXPECT_EQ(last, b);
}

TEST_F(ScOrientedSetTest, ForEachOrder)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  set.Append(b);

  ScAddrVector elements;
  set.ForEach(
      [&](ScAddr const &, ScAddr const & element, ScAddr const &, ScAddr const &)
      {
        elements.push_back(element);
      });

  ASSERT_EQ(elements.size(), 2u);
  EXPECT_EQ(elements[0], a);
  EXPECT_EQ(elements[1], b);
}

TEST_F(ScOrientedSetTest, NextAndReset)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);

  set.Append(a);

  set.Reset();
  EXPECT_EQ(set.Next(), a);
  EXPECT_FALSE(set.Next().IsValid());

  set.Reset();
  EXPECT_EQ(set.Next(), a);
}

TEST_F(ScOrientedSetTest, EmptySet)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  set.Reset();
  EXPECT_FALSE(set.Next().IsValid());

  std::size_t count = 0;
  set.ForEach(
      [&](ScAddr const &, ScAddr const &, ScAddr const &, ScAddr const &)
      {
        ++count;
      });
  EXPECT_EQ(count, 0u);
}

TEST_F(ScOrientedSetTest, RemoveAndReAddOrder)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  set.Append(b);

  set.Remove(a);
  set.Append(a);

  ScAddrVector order;
  set.ForEach(
      [&](ScAddr const &, ScAddr const & element, ScAddr const &, ScAddr const &)
      {
        order.push_back(element);
      });

  ASSERT_EQ(order.size(), 2u);
  EXPECT_EQ(order[0], b);  // b should now be first
  EXPECT_EQ(order[1], a);  // a should now be last
}

TEST_F(ScOrientedSetTest, RemoveOnlyElement)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);

  set.Append(a);
  EXPECT_TRUE(set.Remove(a));
  EXPECT_FALSE(set.GetLast().IsValid());

  set.Reset();
  EXPECT_FALSE(set.Next().IsValid());
}

TEST_F(ScOrientedSetTest, RemoveFirstElement)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  set.Append(b);

  EXPECT_TRUE(set.Remove(a));
  set.Reset();
  EXPECT_EQ(set.Next(), b);
  EXPECT_FALSE(set.Next().IsValid());
  EXPECT_EQ(set.GetLast(), b);
}

TEST_F(ScOrientedSetTest, RemoveLastElement)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  set.Append(b);

  EXPECT_TRUE(set.Remove(b));
  set.Reset();
  EXPECT_EQ(set.Next(), a);
  EXPECT_FALSE(set.Next().IsValid());
  EXPECT_EQ(set.GetLast(), a);
}

TEST_F(ScOrientedSetTest, RemoveMiddleElement)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);
  ScAddr c = m_ctx->GenerateNode(ScType::ConstNode);

  set.Append(a);
  set.Append(b);
  set.Append(c);

  EXPECT_TRUE(set.Remove(b));

  ScAddrVector order;
  set.ForEach(
      [&](ScAddr const &, ScAddr const & elem, ScAddr const &, ScAddr const &)
      {
        order.push_back(elem);
      });

  ASSERT_EQ(order.size(), 2u);
  EXPECT_EQ(order[0], a);
  EXPECT_EQ(order[1], c);
  EXPECT_EQ(set.GetLast(), c);
}

TEST_F(ScOrientedSetTest, RemoveNonexistentElement)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  EXPECT_FALSE(set.Remove(b));
  EXPECT_EQ(set.GetLast(), a);
}

TEST_F(ScOrientedSetTest, RemoveElementWithRole)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr role = m_ctx->GenerateNode(ScType::ConstNodeRole);

  set.Append(a, role);
  EXPECT_TRUE(set.Remove(a));

  set.Reset();
  EXPECT_FALSE(set.Next().IsValid());
  EXPECT_FALSE(set.GetLast().IsValid());
}

TEST_F(ScOrientedSetTest, RemoveAllSequentially)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddrVector nodes;
  for (int i = 0; i < 5; ++i)
    nodes.push_back(m_ctx->GenerateNode(ScType::ConstNodeClass));

  for (auto & n : nodes)
    set.Append(n);

  for (auto & n : nodes)
    EXPECT_TRUE(set.Remove(n));

  set.Reset();
  EXPECT_FALSE(set.Next().IsValid());
  EXPECT_FALSE(set.GetLast().IsValid());
}

TEST_F(ScOrientedSetTest, RemoveUpdatesSequenceCorrectly)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);
  ScAddr c = m_ctx->GenerateNode(ScType::ConstNode);

  set.Append(a);
  set.Append(b);
  set.Append(c);

  EXPECT_TRUE(set.Remove(a));
  EXPECT_TRUE(set.Remove(c));

  set.Reset();
  EXPECT_EQ(set.Next(), b);
  EXPECT_FALSE(set.Next().IsValid());
  EXPECT_EQ(set.GetLast(), b);
}

TEST_F(ScOrientedSetTest, MultipleSequentialResets)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  set.Append(b);

  for (int i = 0; i < 5; ++i)
  {
    set.Reset();
    EXPECT_EQ(set.Next(), a);
    EXPECT_EQ(set.Next(), b);
    EXPECT_FALSE(set.Next().IsValid());
  }
}

TEST_F(ScOrientedSetTest, ForEachEarlyExit)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(a);
  set.Append(b);

  int count = 0;
  bool shouldStop = false;
  set.ForEach(
      [&](ScAddr const &, ScAddr const & element, ScAddr const &, ScAddr const &)
      {
        ++count;
        if (element == a)
          shouldStop = true;
        if (shouldStop)
          return;
      });
  // ForEach does not natively support break, so count should be 2 (all elements)
  EXPECT_EQ(count, 2);
}

TEST_F(ScOrientedSetTest, RemoveAndReappendWithRole)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  ;
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr role = m_ctx->GenerateNode(ScType::ConstNodeRole);

  set.Append(a, role);
  set.Remove(a);
  set.Append(a, role);

  ScAddrVector elements;
  ScAddrVector roles;
  set.ForEach(
      [&](ScAddr const &, ScAddr const & elem, ScAddr const &, ScAddr const & r)
      {
        elements.push_back(elem);
        if (r.IsValid())
          roles.push_back(r);
      });

  ASSERT_EQ(elements.size(), 1u);
  ASSERT_EQ(roles.size(), 1u);
  EXPECT_EQ(elements[0], a);
  EXPECT_EQ(roles[0], role);
}
