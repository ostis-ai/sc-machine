/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_set.hpp>

using ScSetTest = ScMemoryTest;

TEST_F(ScSetTest, BasicAppendRemoveHas)
{
  ScSet set = m_ctx->GenerateSet();

  ScAddr addr1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr addr2 = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  EXPECT_TRUE(addr1.IsValid());
  EXPECT_TRUE(addr2.IsValid());

  // Append elementents
  EXPECT_TRUE(set.Append(addr1));
  EXPECT_TRUE(set.Append(addr2));
  EXPECT_TRUE(set.Has(addr1));
  EXPECT_TRUE(set.Has(addr2));

  // Double append should fail
  EXPECT_FALSE(set.Append(addr1));
  EXPECT_FALSE(set.Append(addr2));

  // Remove elementents
  EXPECT_TRUE(set.Remove(addr1));
  EXPECT_FALSE(set.Has(addr1));
  EXPECT_TRUE(set.Has(addr2));

  // Remove again should fail
  EXPECT_FALSE(set.Remove(addr1));

  // Remove second elementent
  EXPECT_TRUE(set.Remove(addr2));
  EXPECT_FALSE(set.Has(addr2));
  EXPECT_TRUE(set.IsEmpty());
}

TEST_F(ScSetTest, AppendWithRole)
{
  ScSet set = m_ctx->GenerateSet();

  ScAddr element = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr role = m_ctx->GenerateNode(ScType::ConstNodeRole);

  // Append with role
  EXPECT_TRUE(set.Append(element, role));
  // Double append with role should fail
  EXPECT_FALSE(set.Append(element, role));

  // Check via iterator5 that role is present
  ScIterator5Ptr iter = m_ctx->CreateIterator5(set, ScType::ConstPermPosArc, element, ScType::ConstPermPosArc, role);

  bool found = false;
  while (iter->Next())
  {
    EXPECT_FALSE(found);  // should be only one
    EXPECT_EQ(iter->Get(0), set);
    EXPECT_EQ(iter->Get(2), element);
    EXPECT_EQ(iter->Get(4), role);
    found = true;
  }
  EXPECT_TRUE(found);
}

TEST_F(ScSetTest, OperatorOverloads)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr addr1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr addr2 = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  // operator<<
  set << addr1 << addr2;
  EXPECT_TRUE(set.Has(addr1));
  EXPECT_TRUE(set.Has(addr2));

  // operator>>
  set >> addr1;
  EXPECT_FALSE(set.Has(addr1));
  EXPECT_TRUE(set.Has(addr2));
}

TEST_F(ScSetTest, IsEmptyAndGetPower)
{
  ScSet set = m_ctx->GenerateSet();
  EXPECT_TRUE(set.IsEmpty());
  EXPECT_EQ(set.GetPower(), 0u);

  ScAddr addr1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr addr2 = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(addr1);
  set.Append(addr2);

  EXPECT_FALSE(set.IsEmpty());
  EXPECT_EQ(set.GetPower(), 2u);

  set.Remove(addr1);
  set.Remove(addr2);

  EXPECT_TRUE(set.IsEmpty());
  EXPECT_EQ(set.GetPower(), 0u);
}

TEST_F(ScSetTest, NextAndReset)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr addr1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr addr2 = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  set.Append(addr1);
  set.Append(addr2);

  set.Reset();
  ScAddrSet roles;
  ScAddrSet foundElements;
  for (int i = 0; i < 2; ++i)
  {
    ScAddr const nextElement = set.Next(roles);
    if (nextElement.IsValid())
      foundElements.insert(nextElement);
  }
  EXPECT_EQ(foundElements.size(), 2u);
  EXPECT_TRUE(foundElements.count(addr1) || foundElements.count(addr2));

  // After two Next() calls, should be empty
  EXPECT_FALSE(set.Next().IsValid());

  // Reset and try again
  set.Reset();
  foundElements.clear();
  for (int i = 0; i < 2; ++i)
  {
    ScAddr const nextElement = set.Next();
    if (nextElement.IsValid())
      foundElements.insert(nextElement);
  }
  EXPECT_EQ(foundElements.size(), 2u);
}

TEST_F(ScSetTest, ForEachSet)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr addr1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr addr2 = m_ctx->GenerateNode(ScType::ConstNodeMaterial);
  ScAddr role = m_ctx->GenerateNode(ScType::ConstNodeRole);

  set.Append(addr1, role);
  set.Append(addr2);

  // Collect all elements and roles
  ScAddrSet elements;
  ScAddrSet roles;
  set.ForEach(
      [&](ScAddr const & arcToelement, ScAddr const & element, ScAddr const &, ScAddr const & roleAddr)
      {
        elements.insert(element);
        if (roleAddr.IsValid())
          roles.insert(roleAddr);
      });
  EXPECT_EQ(elements.size(), 2u);
  EXPECT_EQ(roles.size(), 1u);
  EXPECT_TRUE(roles.count(role));
}

TEST_F(ScSetTest, RemoveNonexistentElement)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr element = m_ctx->GenerateNode(ScType::ConstNodeClass);

  // Remove before append
  EXPECT_FALSE(set.Remove(element));
  EXPECT_FALSE(set.Has(element));
}

TEST_F(ScSetTest, MultipleRolesForOneElement)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr element = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr role1 = m_ctx->GenerateNode(ScType::ConstNodeRole);
  ScAddr role2 = m_ctx->GenerateNode(ScType::ConstNodeRole);

  EXPECT_TRUE(set.Append(element, role1));
  EXPECT_TRUE(set.Append(element, role2, ScType::ConstPermPosArc));

  // Use ForEach to check both roles
  ScAddrSet foundRoles;
  set.ForEach(
      [&](ScAddr const &, ScAddr const & e, ScAddr const &, ScAddr const & r)
      {
        if (e == element && r.IsValid())
          foundRoles.insert(r);
      });
  EXPECT_EQ(foundRoles.size(), 2u);
  EXPECT_TRUE(foundRoles.count(role1));
  EXPECT_TRUE(foundRoles.count(role2));
}

TEST_F(ScSetTest, OperatorChaining)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  // Chained insertion
  (set << a << b);
  EXPECT_TRUE(set.Has(a));
  EXPECT_TRUE(set.Has(b));

  // Chained removal
  (set >> a >> b);
  EXPECT_FALSE(set.Has(a));
  EXPECT_FALSE(set.Has(b));
}

TEST_F(ScSetTest, GetPowerMixed)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr b = m_ctx->GenerateNode(ScType::ConstNodeMaterial);

  EXPECT_EQ(set.GetPower(), 0u);
  set.Append(a);
  EXPECT_EQ(set.GetPower(), 1u);
  set.Append(b);
  EXPECT_EQ(set.GetPower(), 2u);
  set.Remove(a);
  EXPECT_EQ(set.GetPower(), 1u);
  set.Remove(b);
  EXPECT_EQ(set.GetPower(), 0u);
}

TEST_F(ScSetTest, ResetNextRobustness)
{
  ScSet set = m_ctx->GenerateSet();
  ScAddr a = m_ctx->GenerateNode(ScType::ConstNodeClass);
  set.Append(a);

  // Call Next before and after Reset
  set.Reset();
  EXPECT_EQ(set.Next(), a);
  EXPECT_FALSE(set.Next().IsValid());

  set.Reset();
  EXPECT_EQ(set.Next(), a);
}
