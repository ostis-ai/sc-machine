#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "sc_test.hpp"

using ScStructTest = ScMemoryTest;

TEST_F(ScStructTest, AppendIterateElements)
{
  ScStructure structure = m_ctx->CreateStructure();

  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstMaterial);
  EXPECT_TRUE(addr2.IsValid());

  structure << addr1 << addr2;
  EXPECT_TRUE(structure.HasElement(addr1));
  EXPECT_FALSE(structure.Append(addr1));
  EXPECT_TRUE(structure.HasElement(addr2));

  structure >> addr1;

  EXPECT_FALSE(structure.HasElement(addr1));
  EXPECT_TRUE(structure.HasElement(addr2));

  structure >> addr2;

  EXPECT_FALSE(structure.HasElement(addr1));
  EXPECT_FALSE(structure.HasElement(addr2));
  EXPECT_TRUE(structure.IsEmpty());

  // attributes
  ScAddr const attrAddr = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(attrAddr.IsValid());

  EXPECT_TRUE(structure.Append(addr1, attrAddr));
  EXPECT_FALSE(structure.Append(addr1, attrAddr));
  ScIterator5Ptr iter5 = m_ctx->Iterator5(
      structure, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, attrAddr);

  bool found = false;
  while (iter5->Next())
  {
    EXPECT_FALSE(found);  // one time
    EXPECT_EQ(iter5->Get(0), structure);
    EXPECT_EQ(iter5->Get(2), addr1);
    EXPECT_EQ(iter5->Get(4), attrAddr);
    found = true;
  }
  EXPECT_TRUE(found);
}

TEST_F(ScStructTest, AppendItSelf)
{
  ScSet set = m_ctx->CreateSet();
  EXPECT_TRUE(set.IsValid());

  ScSet setCopy = set;
  EXPECT_TRUE(setCopy.IsValid());
  EXPECT_TRUE(set.IsValid());

  setCopy = set;
  EXPECT_TRUE(setCopy.IsValid());
  EXPECT_TRUE(set.IsValid());

  setCopy = setCopy;
  EXPECT_TRUE(setCopy.IsValid());
  EXPECT_TRUE(set.IsValid());

  ScAddr const & nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  setCopy << nodeAddr;

  ScTemplate templ;
  templ.Triple(setCopy, ScType::EdgeAccessVarPosPerm, ScType::NodeVar);
  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));

  result.ForEach(
      [&](ScTemplateSearchResultItem const & item)
      {
        setCopy << item;
      });

  EXPECT_TRUE(setCopy.HasElement(setCopy));
}
