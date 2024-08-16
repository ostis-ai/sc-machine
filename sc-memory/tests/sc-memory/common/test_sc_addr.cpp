#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

TEST(ScAddrTest, Hash)
{
  sc_addr a;
  a.offset = 123;
  a.seg = 654;

  ScAddr addr1(a);
  auto const hash = addr1.Hash();
  ScAddr const addr2(hash);

  EXPECT_EQ(addr1, addr2);

  addr1.Reset();
  EXPECT_EQ(addr1, ScAddr::Empty);
}

TEST(ScAddrTest, ScAddrToValueUnorderedMap)
{
  ScAddrToValueUnorderedMap<int> testMap;
  sc_addr a;
  a.offset = 123;
  a.seg = 654;

  ScAddr addr1(a);

  sc_addr b;
  b.offset = 321;
  b.seg = 456;

  ScAddr addr2(b);

  EXPECT_EQ(testMap.size(), 0u);

  testMap.emplace(addr1, 1);
  EXPECT_EQ(testMap.size(), 1u);
  EXPECT_TRUE(testMap.count(addr1));
  EXPECT_FALSE(testMap.count(addr2));

  testMap.emplace(addr2, 2);
  EXPECT_EQ(testMap.size(), 2u);
  EXPECT_TRUE(testMap.count(addr1));
  EXPECT_TRUE(testMap.count(addr2));
}
