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

TEST(ScAddrTest, ScAddrLessFunc)
{
  {
    ScAddr addr1{{1, 2}};
    ScAddr addr2{{1, 2}};

    EXPECT_FALSE(ScAddrLessFunc()(addr1, addr2));
  }
  {
    ScAddr addr1{{2, 2}};
    ScAddr addr2{{1, 2}};

    EXPECT_FALSE(ScAddrLessFunc()(addr1, addr2));
  }
  {
    ScAddr addr1{{1, 2}};
    ScAddr addr2{{2, 2}};

    EXPECT_TRUE(ScAddrLessFunc()(addr1, addr2));
  }
  {
    ScAddr addr1{{2, 2}};
    ScAddr addr2{{2, 1}};

    EXPECT_FALSE(ScAddrLessFunc()(addr1, addr2));
  }
  {
    ScAddr addr1{{2, 1}};
    ScAddr addr2{{2, 2}};

    EXPECT_TRUE(ScAddrLessFunc()(addr1, addr2));
  }
}
