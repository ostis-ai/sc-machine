#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_link.hpp"

#include "sc_test.hpp"

template <typename Type> void TestType(ScMemoryContext & ctx, Type const & value)
{
  ScAddr const linkAddr = ctx.CreateLink();
  EXPECT_TRUE(linkAddr.IsValid());

  ScLink link(ctx, linkAddr);
  EXPECT_FALSE(link.IsType<Type>());

  EXPECT_TRUE(link.Set(value));
  EXPECT_EQ(link.Get<Type>(), value);
  EXPECT_TRUE(link.IsType<Type>());
}

using ScLinkTest = ScMemoryTest;

TEST_F(ScLinkTest, common)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_common");

  TestType<std::string>(ctx, "test_string_value");
  TestType<double>(ctx, 5.0);
  TestType<float>(ctx, 10.f);
  TestType<int8_t>(ctx, -57);
  TestType<int16_t>(ctx, -6757);
  TestType<int32_t>(ctx, -672357);
  TestType<int64_t>(ctx, -672423457);
  TestType<uint8_t>(ctx, 57);
  TestType<uint16_t>(ctx, 6757);
  TestType<uint32_t>(ctx, 672357);
  TestType<uint64_t>(ctx, 672423457);
}

TEST_F(ScLinkTest, reuse)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_reuse");

  std::string const str = "test_string_value_2";
  ScAddr const linkAddr = ctx.CreateLink();
  EXPECT_TRUE(linkAddr.IsValid());

  ScLink link(ctx, linkAddr);
  EXPECT_FALSE(link.IsType<std::string>());

  EXPECT_TRUE(link.Set(str));
  EXPECT_TRUE(link.IsType<std::string>());
  EXPECT_EQ(link.Get<std::string>(), str);

  uint64_t value = 32049809;
  EXPECT_TRUE(link.Set(value));
  EXPECT_TRUE(link.IsType<uint64_t>());
  EXPECT_FALSE(link.IsType<std::string>());
  EXPECT_EQ(link.Get<uint64_t>(), value);
}

TEST_F(ScLinkTest, as_string)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_as_string");

  ScAddr const linkAddr = ctx.CreateLink();
  EXPECT_TRUE(linkAddr.IsValid());

  ScLink link(ctx, linkAddr);

  EXPECT_TRUE(link.Set<std::string>("76"));
  EXPECT_EQ(link.GetAsString(), "76");

  EXPECT_TRUE(link.Set<int8_t>(-76));
  EXPECT_EQ(link.GetAsString(), "-76");

  EXPECT_TRUE(link.Set<uint8_t>(76));
  EXPECT_EQ(link.GetAsString(), "76");

  EXPECT_TRUE(link.Set<int16_t>(-760));
  EXPECT_EQ(link.GetAsString(), "-760");

  EXPECT_TRUE(link.Set<uint16_t>(760));
  EXPECT_EQ(link.GetAsString(), "760");

  EXPECT_TRUE(link.Set<int32_t>(-76000));
  EXPECT_EQ(link.GetAsString(), "-76000");

  EXPECT_TRUE(link.Set<uint32_t>(76000));
  EXPECT_EQ(link.GetAsString(), "76000");

  EXPECT_TRUE(link.Set<int64_t>(-7600000000));
  EXPECT_EQ(link.GetAsString(), "-7600000000");

  EXPECT_TRUE(link.Set<uint64_t>(7600000000));
  EXPECT_EQ(link.GetAsString(), "7600000000");
}
