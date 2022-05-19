#include <gtest/gtest.h>

#include "sc-memory/sc_object.hpp"

#include "sc_test.hpp"

#include "test_sc_object.hpp"

using ScCodegenTest = ScMemoryTest;

TEST_F(ScCodegenTest, smoke)
{
  ScAddr const addr1 = m_ctx->CreateNode(ScType::Const);
  EXPECT_TRUE(addr1.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("test_keynode1", addr1));

  ScAddr const addr2 = m_ctx->CreateNode(ScType::Var);
  EXPECT_TRUE(addr2.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("test_keynode2", addr2));

  ScAddr const addr3 = m_ctx->CreateNode(ScType::Var);
  EXPECT_TRUE(addr3.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("test_keynode3", addr3));

  n1::n2::TestObject obj1;
  obj1.Init();

  EXPECT_EQ(addr1, obj1.mTestKeynode1);
  EXPECT_EQ(addr2, obj1.mTestKeynode2);

  obj1.InitGlobal();
  EXPECT_EQ(addr3, obj1.mTestKeynode3);

  ScAddr const addrForce = m_ctx->HelperFindBySystemIdtf("test_keynode_force");
  EXPECT_TRUE(addrForce.IsValid());
  EXPECT_EQ(addrForce, obj1.mTestKeynodeForce);
}
