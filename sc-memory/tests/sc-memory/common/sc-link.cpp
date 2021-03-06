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

TEST_F(ScLinkTest, operations)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_links_operations");

  ScAddr formulaAddr = ctx.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(ctx.HelperSetSystemIdtf("atomic_formula", formulaAddr));
  EXPECT_TRUE(formulaAddr == ctx.HelperFindBySystemIdtf("atomic_formula"));
  EXPECT_TRUE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));

  ScAddr node1 = ctx.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node1.IsValid());
  EXPECT_TRUE(ctx.HelperSetSystemIdtf("node1", node1));
  EXPECT_TRUE("node1" == ctx.HelperGetSystemIdtf(node1));

  ScAddr node2 = ctx.HelperResolveSystemIdtf("_node2", ScType::NodeVarStruct);
  EXPECT_TRUE(node2.IsValid());
  EXPECT_TRUE("_node2" == ctx.HelperGetSystemIdtf(node2));

  ctx.Destroy();
}

TEST_F(ScLinkTest, smoke)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_links_smoke");

  ScAddr formulaAddr = ctx.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(ctx.HelperSetSystemIdtf("atomic_formula", formulaAddr));
  EXPECT_TRUE(formulaAddr == ctx.HelperFindBySystemIdtf("atomic_formula"));
  EXPECT_TRUE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));
  EXPECT_TRUE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));

  ScAddr node1 = ctx.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node1.IsValid());
  EXPECT_FALSE(ctx.HelperSetSystemIdtf("atomic_formula", node1));
  EXPECT_TRUE(ctx.HelperGetSystemIdtf(node1).empty());

  node1 = ctx.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node1.IsValid());
  EXPECT_FALSE(node1 == ctx.HelperResolveSystemIdtf("atomic_formula", ScType::NodeConst));
  EXPECT_TRUE(formulaAddr == ctx.HelperResolveSystemIdtf("atomic_formula", ScType::NodeConst));
  EXPECT_TRUE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));

  EXPECT_TRUE(ctx.FindLinksByContent("atomic_formula").size() == 1);

  ScAddr linkAddr = ctx.CreateLink();
  ScLink link = ScLink(ctx, linkAddr);
  std::string str = "atomic_formula";
  link.Set(str);
  EXPECT_TRUE("atomic_formula" == link.GetAsString());

  ScAddrVector linkList = ctx.FindLinksByContent("atomic_formula");
  EXPECT_TRUE(linkList.size() == 2);
  EXPECT_TRUE(linkList[0].IsValid());
  EXPECT_TRUE(linkList[1].IsValid());

  str = "non_atomic_formula";
  link.Set(str);
  EXPECT_TRUE("non_atomic_formula" == link.GetAsString());

  EXPECT_TRUE(ctx.FindLinksByContent("atomic_formula").size() == 1);

  EXPECT_TRUE(ctx.FindLinksByContent("non_atomic_formula").size() == 1);

  ScAddr node2 = ctx.HelperResolveSystemIdtf("_node2", ScType::NodeVarStruct);
  EXPECT_TRUE(node2.IsValid());
  EXPECT_TRUE("_node2" == ctx.HelperGetSystemIdtf(node2));

  ctx.Destroy();
}

TEST_F(ScLinkTest, content_changed)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_links_content_changed");

  ScAddr linkAddr = ctx.CreateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link = ScLink(ctx, linkAddr);

  std::string str = "content1";
  EXPECT_TRUE(link.Set(str));

  EXPECT_TRUE(str == link.GetAsString());

  str = "content2";
  EXPECT_TRUE(link.Set(str));
  EXPECT_TRUE(str == link.GetAsString());

  str = "content3";
  EXPECT_TRUE(link.Set(str));
  EXPECT_TRUE(str == link.GetAsString());

  EXPECT_FALSE(ctx.FindLinksByContent("content3").empty());
  EXPECT_TRUE(ctx.FindLinksByContent("content1").empty());
  EXPECT_TRUE(ctx.FindLinksByContent("content1").empty());
  EXPECT_TRUE(ctx.FindLinksByContent("content2").empty());

  ctx.Destroy();
}

TEST_F(ScLinkTest, set_system_idtf)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_links_set_system_idtf");

  ScAddr timestamp = ctx.CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(ctx.HelperSetSystemIdtf("2022.03.21 16:07:47", timestamp));
  EXPECT_TRUE("2022.03.21 16:07:47" == ctx.HelperGetSystemIdtf(timestamp));

  EXPECT_FALSE(ctx.FindLinksByContent("2022.03.21 16:07:47").empty());

  timestamp = ctx.CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(ctx.HelperSetSystemIdtf("2022.03.21 16:07:48", timestamp));
  EXPECT_TRUE("2022.03.21 16:07:48" == ctx.HelperGetSystemIdtf(timestamp));

  EXPECT_FALSE(ctx.FindLinksByContent("2022.03.21 16:07:48").empty());
  EXPECT_FALSE(ctx.FindLinksByContent("2022.03.21 16:07:47").empty());

  ctx.Destroy();
}

TEST_F(ScLinkTest, find_by_substr)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_links_content_changed");

  ScAddr linkAddr = ctx.CreateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link1 = ScLink(ctx, linkAddr);
  std::string str = "content1";
  EXPECT_TRUE(link1.Set(str));
  EXPECT_TRUE(str == link1.GetAsString());

  linkAddr = ctx.CreateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link2 = ScLink(ctx, linkAddr);
  str = "continue";
  EXPECT_TRUE(link2.Set(str));
  EXPECT_TRUE(str == link2.GetAsString());

  linkAddr = ctx.CreateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link3 = ScLink(ctx, linkAddr);
  str = "contents_25";
  EXPECT_TRUE(link3.Set(str));
  EXPECT_TRUE(str == link3.GetAsString());

  EXPECT_FALSE(ctx.FindLinksByContent("content1").empty());
  EXPECT_FALSE(ctx.FindLinksByContent("continue").empty());
  EXPECT_FALSE(ctx.FindLinksByContent("contents_25").empty());

  EXPECT_TRUE(ctx.FindLinksByContentSubstring("content1").size() == 1);
  EXPECT_TRUE(ctx.FindLinksByContentSubstring("cont").size() == 3);
  EXPECT_TRUE(ctx.FindLinksByContentSubstring("content").size() == 2);
  EXPECT_TRUE(ctx.FindLinksByContentSubstring("con").size() == 3);
  EXPECT_TRUE(ctx.FindLinksByContentSubstring("content2").empty());
  EXPECT_TRUE(ctx.FindLinksByContentSubstring("contents").size() == 1);
  EXPECT_TRUE(ctx.FindLinksByContentSubstring("contents_2").size() == 1);
  EXPECT_TRUE(ctx.FindLinksByContentSubstring("contents_26").empty());

  ctx.Destroy();
}
