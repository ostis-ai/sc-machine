#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "sc_test.hpp"

using ScStructTest = ScMemoryTest;

TEST_F(ScStructTest, common)
{
  ScAddr const structAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(structAddr.IsValid());

  ScStruct st(*m_ctx, structAddr);

  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstMaterial);
  EXPECT_TRUE(addr2.IsValid());

  st << addr1 << addr2;
  EXPECT_TRUE(st.HasElement(addr1));
  EXPECT_TRUE(st.HasElement(addr2));

  st >> addr1;

  EXPECT_FALSE(st.HasElement(addr1));
  EXPECT_TRUE(st.HasElement(addr2));

  st >> addr2;

  EXPECT_FALSE(st.HasElement(addr1));
  EXPECT_FALSE(st.HasElement(addr2));
  EXPECT_TRUE(st.IsEmpty());

  // attributes
  ScAddr const attrAddr = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(attrAddr.IsValid());

  SC_CHECK(st.Append(addr1, attrAddr), ());
  ScIterator5Ptr iter5 = m_ctx->Iterator5(
      structAddr,
      ScType::EdgeAccessConstPosPerm,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      attrAddr);

  bool found = false;
  while (iter5->Next())
  {
    EXPECT_FALSE(found);	// one time
    EXPECT_EQ(iter5->Get(0), structAddr);
    EXPECT_EQ(iter5->Get(2), addr1);
    EXPECT_EQ(iter5->Get(4), attrAddr);
    found = true;
  }
  EXPECT_TRUE(found);
}
