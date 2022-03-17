#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

TEST(ScAddrTest, Hash)
{
  sc_addr a;
  a.offset = 123;
  a.seg = 654;

  ScAddr const addr1(a);
  auto const hash = addr1.Hash();
  ScAddr const addr2(hash);

  EXPECT_EQ(addr1, addr2);
}
