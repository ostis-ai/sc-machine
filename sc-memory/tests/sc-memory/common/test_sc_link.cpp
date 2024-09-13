#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_link.hpp"

#include "sc_test.hpp"

#include <algorithm>

template <typename Type>
void TestType(ScMemoryContext & ctx, Type const & value)
{
  ScAddr const linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());

  ScLink link(ctx, linkAddr);
  EXPECT_TRUE(link.IsValid());
  EXPECT_FALSE(link.IsType<Type>());

  EXPECT_TRUE(link.Set(value));
  EXPECT_EQ(link.Get<Type>(), value);
  EXPECT_TRUE(link.IsType<Type>());
}

using ScLinkTest = ScMemoryTest;

TEST_F(ScLinkTest, common)
{
  ScMemoryContext ctx;

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
  ScMemoryContext ctx;

  std::string const str = "test_string_value_2";
  ScAddr const linkAddr = ctx.GenerateLink();
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
  ScMemoryContext ctx;

  ScAddr const linkAddr = ctx.GenerateLink();
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

  EXPECT_TRUE(link.Set<float>(3.14));
  EXPECT_EQ(link.GetAsString(), "3.140000");

  EXPECT_TRUE(link.Set<double>(3.14));
  EXPECT_EQ(link.GetAsString(), "3.140000");
}

TEST_F(ScLinkTest, operations)
{
  ScMemoryContext ctx;

  ScAddr formulaAddr = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(ctx.SetElementSystemIdentifier("atomic_formula", formulaAddr));
  EXPECT_TRUE(formulaAddr == ctx.SearchElementBySystemIdentifier("atomic_formula"));
  EXPECT_TRUE("atomic_formula" == ctx.GetElementSystemIdentifier(formulaAddr));

  ScAddr node1 = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(node1.IsValid());
  EXPECT_TRUE(ctx.SetElementSystemIdentifier("node1", node1));
  EXPECT_TRUE("node1" == ctx.GetElementSystemIdentifier(node1));

  ScAddr node2 = ctx.ResolveElementSystemIdentifier("_node2", ScType::NodeVarStruct);
  EXPECT_TRUE(node2.IsValid());
  EXPECT_TRUE("_node2" == ctx.GetElementSystemIdentifier(node2));

  ctx.Destroy();
}

TEST_F(ScLinkTest, smoke)
{
  ScMemoryContext ctx;

  ScAddr formulaAddr = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(ctx.SetElementSystemIdentifier("atomic_formula", formulaAddr));
  EXPECT_TRUE(formulaAddr == ctx.SearchElementBySystemIdentifier("atomic_formula"));
  EXPECT_TRUE("atomic_formula" == ctx.GetElementSystemIdentifier(formulaAddr));
  EXPECT_TRUE("atomic_formula" == ctx.GetElementSystemIdentifier(formulaAddr));

  ScAddr node1 = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(node1.IsValid());
  EXPECT_FALSE(ctx.SetElementSystemIdentifier("atomic_formula", node1));
  EXPECT_TRUE(ctx.GetElementSystemIdentifier(node1).empty());

  node1 = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(node1.IsValid());
  EXPECT_FALSE(node1 == ctx.ResolveElementSystemIdentifier("atomic_formula", ScType::NodeConst));
  EXPECT_TRUE(formulaAddr == ctx.ResolveElementSystemIdentifier("atomic_formula", ScType::NodeConst));
  EXPECT_TRUE("atomic_formula" == ctx.GetElementSystemIdentifier(formulaAddr));

  EXPECT_TRUE(ctx.SearchLinksByContent("atomic_formula").size() == 1);

  ScAddr linkAddr = ctx.GenerateLink();
  ScLink link = ScLink(ctx, linkAddr);
  std::string str = "atomic_formula";
  link.Set(str);
  EXPECT_TRUE("atomic_formula" == link.GetAsString());

  ScAddrSet linkSet = ctx.SearchLinksByContent("atomic_formula");
  EXPECT_TRUE(linkSet.size() == 2);

  str = "non_atomic_formula";
  link.Set(str);
  EXPECT_TRUE("non_atomic_formula" == link.GetAsString());

  EXPECT_TRUE(ctx.SearchLinksByContent("atomic_formula").size() == 1);

  EXPECT_TRUE(ctx.SearchLinksByContent("non_atomic_formula").size() == 1);

  ScAddr node2 = ctx.ResolveElementSystemIdentifier("_node2", ScType::NodeVarStruct);
  EXPECT_TRUE(node2.IsValid());
  EXPECT_TRUE("_node2" == ctx.GetElementSystemIdentifier(node2));

  ctx.Destroy();
}

TEST_F(ScLinkTest, content_changed)
{
  ScMemoryContext ctx;

  ScAddr linkAddr = ctx.GenerateLink();
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

  EXPECT_FALSE(ctx.SearchLinksByContent("content3").empty());
  EXPECT_TRUE(ctx.SearchLinksByContent("content1").empty());
  EXPECT_TRUE(ctx.SearchLinksByContent("content1").empty());
  EXPECT_TRUE(ctx.SearchLinksByContent("content2").empty());

  ctx.Destroy();
}

TEST_F(ScLinkTest, set_system_idtf)
{
  ScMemoryContext ctx;

  ScAddr timestamp = ctx.GenerateNode(ScType::NodeConstClass);
  EXPECT_TRUE(ctx.SetElementSystemIdentifier("2022.03.21 16:07:47", timestamp));
  EXPECT_TRUE("2022.03.21 16:07:47" == ctx.GetElementSystemIdentifier(timestamp));

  EXPECT_FALSE(ctx.SearchLinksByContent("2022.03.21 16:07:47").empty());

  timestamp = ctx.GenerateNode(ScType::NodeConstClass);
  EXPECT_TRUE(ctx.SetElementSystemIdentifier("2022.03.21 16:07:48", timestamp));
  EXPECT_TRUE("2022.03.21 16:07:48" == ctx.GetElementSystemIdentifier(timestamp));

  EXPECT_FALSE(ctx.SearchLinksByContent("2022.03.21 16:07:48").empty());
  EXPECT_FALSE(ctx.SearchLinksByContent("2022.03.21 16:07:47").empty());

  ctx.Destroy();
}

TEST_F(ScLinkTest, find_links_by_substr)
{
  ScMemoryContext ctx;

  ScAddr linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link1 = ScLink(ctx, linkAddr);
  std::string str = "content1";
  EXPECT_TRUE(link1.Set(str));
  EXPECT_TRUE(str == link1.GetAsString());

  linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link2 = ScLink(ctx, linkAddr);
  str = "ton_content";
  EXPECT_TRUE(link2.Set(str));
  EXPECT_TRUE(str == link2.GetAsString());

  linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link3 = ScLink(ctx, linkAddr);
  str = "cotents_25";
  EXPECT_TRUE(link3.Set(str));
  EXPECT_TRUE(str == link3.GetAsString());

  EXPECT_FALSE(ctx.SearchLinksByContent("content1").empty());
  EXPECT_FALSE(ctx.SearchLinksByContent("ton_content").empty());
  EXPECT_FALSE(ctx.SearchLinksByContent("cotents_25").empty());

  EXPECT_TRUE(ctx.SearchLinksByContentSubstring("content1").size() == 1);
  EXPECT_TRUE(ctx.SearchLinksByContentSubstring("cont").size() == 3);
  EXPECT_TRUE(ctx.SearchLinksByContentSubstring("content").size() == 3);
  EXPECT_TRUE(ctx.SearchLinksByContentSubstring("content2").empty());
  EXPECT_TRUE(ctx.SearchLinksByContentSubstring("contents").empty());
  EXPECT_TRUE(ctx.SearchLinksByContentSubstring("cotents_2").size() == 1);
  EXPECT_TRUE(ctx.SearchLinksByContentSubstring("cotents_26").empty());

  ctx.Destroy();
}

TEST_F(ScLinkTest, find_strings_by_substr)
{
  ScMemoryContext ctx;

  ScAddr linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link1 = ScLink(ctx, linkAddr);
  std::string str = "some coten";
  EXPECT_TRUE(link1.Set(str));
  EXPECT_TRUE(str == link1.GetAsString());

  linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link2 = ScLink(ctx, linkAddr);
  str = "ton_content";
  EXPECT_TRUE(link2.Set(str));
  EXPECT_TRUE(str == link2.GetAsString());

  linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link3 = ScLink(ctx, linkAddr);
  str = "content_tents_25";
  EXPECT_TRUE(link3.Set(str));
  EXPECT_TRUE(str == link3.GetAsString());

  EXPECT_FALSE(ctx.SearchLinksContentsByContentSubstring("some coten").empty());
  EXPECT_FALSE(ctx.SearchLinksContentsByContentSubstring("ton_content").empty());
  EXPECT_FALSE(ctx.SearchLinksContentsByContentSubstring("content_tents_25").empty());

  auto strings = ctx.SearchLinksContentsByContentSubstring("some coten");
  EXPECT_TRUE(ctx.SearchLinksContentsByContentSubstring("some coten").size() == 1);
  EXPECT_TRUE(strings.count("some coten"));

  strings = ctx.SearchLinksContentsByContentSubstring("cont");
  EXPECT_TRUE(strings.size() == 3);
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "ton_content") != strings.end());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") != strings.end());

  strings = ctx.SearchLinksContentsByContentSubstring("25");
  EXPECT_TRUE(strings.size() == 1);
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") != strings.end());

  strings = ctx.SearchLinksContentsByContentSubstring("co");
  // there are another links as system identifiers of resolve keynodes after sc-server start
  EXPECT_TRUE(strings.size() >= 3);
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "some coten") != strings.end());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "ton_content") != strings.end());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") != strings.end());

  ctx.Destroy();
}

TEST_F(ScLinkTest, find_strings_by_substr_as_prefix)
{
  ScMemoryContext ctx;

  ScAddr linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link1 = ScLink(ctx, linkAddr);
  std::string str = "some coten";
  EXPECT_TRUE(link1.Set(str));
  EXPECT_TRUE(str == link1.GetAsString());

  linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link2 = ScLink(ctx, linkAddr);
  str = "ton_content";
  EXPECT_TRUE(link2.Set(str));
  EXPECT_TRUE(str == link2.GetAsString());

  linkAddr = ctx.GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());
  ScLink link3 = ScLink(ctx, linkAddr);
  str = "content_tents_25";
  EXPECT_TRUE(link3.Set(str));
  EXPECT_TRUE(str == link3.GetAsString());

  auto strings = ctx.SearchLinksContentsByContentSubstring("cont", 4);
  EXPECT_TRUE(strings.size() == 1);  // found by prefix substring
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "ton_content") == strings.end());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") != strings.end());

  strings = ctx.SearchLinksContentsByContentSubstring("cont", 3);
  EXPECT_TRUE(strings.size() == 3);  // found by substring, not prefix
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "ton_content") != strings.end());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") != strings.end());

  strings = ctx.SearchLinksContentsByContentSubstring("cont", 5);
  EXPECT_TRUE(strings.size() == 1);  // found by prefix substring
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "ton_content") == strings.end());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") != strings.end());

  strings = ctx.SearchLinksContentsByContentSubstring("25", 2);
  EXPECT_TRUE(strings.empty());  // found by prefix substring
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") == strings.end());

  strings = ctx.SearchLinksContentsByContentSubstring("25", 1);
  EXPECT_TRUE(strings.size() == 1);  // found by substring, not prefix
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") != strings.end());

  strings = ctx.SearchLinksContentsByContentSubstring("25", 3);
  EXPECT_TRUE(strings.empty());  // found by prefix substring
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "content_tents_25") == strings.end());

  ctx.Destroy();
}

TEST_F(ScLinkTest, links_deletion)
{
  ScMemoryContext ctx;

  ScAddr const & linkAddr1 = ctx.GenerateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr1.IsValid());
  EXPECT_TRUE(ctx.SetLinkContent(linkAddr1, "link_content_example"));
  EXPECT_TRUE(ctx.SearchLinksByContent("link_content_example").size() == 1);

  ScAddr const & linkAddr2 = ctx.GenerateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr2.IsValid());
  EXPECT_TRUE(ctx.SetLinkContent(linkAddr2, "link_content_example"));
  EXPECT_TRUE(ctx.SearchLinksByContent("link_content_example").size() == 2);

  EXPECT_TRUE(ctx.EraseElement(linkAddr1));
  EXPECT_TRUE(ctx.SearchLinksByContent("link_content_example").size() == 1);

  EXPECT_TRUE(ctx.EraseElement(linkAddr2));
  EXPECT_TRUE(ctx.SearchLinksByContent("link_content_example").empty());

  ctx.Destroy();
}

TEST_F(ScLinkTest, link_deletion_repeteded)
{
  ScMemoryContext ctx;

  ScAddr const & linkAddr1 = ctx.GenerateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr1.IsValid());
  EXPECT_TRUE(ctx.SetLinkContent(linkAddr1, "link_content_example"));
  EXPECT_TRUE(ctx.SearchLinksByContent("link_content_example").size() == 1);

  EXPECT_TRUE(ctx.EraseElement(linkAddr1));
  EXPECT_TRUE(ctx.SearchLinksByContent("link_content_example").empty());

  EXPECT_FALSE(ctx.EraseElement(linkAddr1));
  EXPECT_TRUE(ctx.SearchLinksByContent("link_content_example").empty());

  ctx.Destroy();
}

TEST_F(ScLinkTest, empty_link_deletion)
{
  ScMemoryContext ctx;

  ScAddr const & linkAddr1 = ctx.GenerateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr1.IsValid());
  EXPECT_TRUE(ctx.SetLinkContent(linkAddr1, ""));
  EXPECT_TRUE(ctx.SearchLinksByContent("").size() == 1);

  EXPECT_TRUE(ctx.EraseElement(linkAddr1));
  EXPECT_TRUE(ctx.SearchLinksByContent("").empty());

  EXPECT_FALSE(ctx.EraseElement(linkAddr1));
  EXPECT_TRUE(ctx.SearchLinksByContent("").empty());

  ctx.Destroy();
}
