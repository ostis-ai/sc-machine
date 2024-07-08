#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "sc_test.hpp"

using ScStructTest = ScMemoryTest;

TEST_F(ScStructTest, AppendIterateElements)
{
  ScStruct structAddr(*m_ctx);

  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstMaterial);
  EXPECT_TRUE(addr2.IsValid());

  structAddr << addr1 << addr2;
  EXPECT_TRUE(structAddr.HasElement(addr1));
  EXPECT_TRUE(structAddr.HasElement(addr2));

  structAddr >> addr1;

  EXPECT_FALSE(structAddr.HasElement(addr1));
  EXPECT_TRUE(structAddr.HasElement(addr2));

  structAddr >> addr2;

  EXPECT_FALSE(structAddr.HasElement(addr1));
  EXPECT_FALSE(structAddr.HasElement(addr2));
  EXPECT_TRUE(structAddr.IsEmpty());

  // attributes
  ScAddr const attrAddr = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(attrAddr.IsValid());

  EXPECT_TRUE(structAddr.Append(addr1, attrAddr));
  EXPECT_FALSE(structAddr.Append(addr1, attrAddr));
  ScIterator5Ptr iter5 = m_ctx->Iterator5(
      structAddr, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, attrAddr);

  bool found = false;
  while (iter5->Next())
  {
    EXPECT_FALSE(found);  // one time
    EXPECT_EQ(iter5->Get(0), structAddr);
    EXPECT_EQ(iter5->Get(2), addr1);
    EXPECT_EQ(iter5->Get(4), attrAddr);
    found = true;
  }
  EXPECT_TRUE(found);
}
