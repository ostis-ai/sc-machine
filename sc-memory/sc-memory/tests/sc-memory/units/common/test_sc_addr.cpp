/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>

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
