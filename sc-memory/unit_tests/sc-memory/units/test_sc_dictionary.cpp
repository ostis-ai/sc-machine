/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <iomanip>
#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/sc_link.hpp"

extern "C"
{
#include "sc-core/sc-store/sc_fs_storage.h"
#include "sc-core/sc-store/sc-container/sc-dictionary/sc_dictionary.h"
}

TEST_CASE("sc-strings-complex", "[test sc-string tree complex]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc-strings-complex");

  sc_dictionary * dictionary;
  REQUIRE(sc_dictionary_initialize(&dictionary));

  sc_addr addr1 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str1 = "concept_set1";
  sc_dictionary_append(dictionary, str1, strlen(str1), &addr1);
  REQUIRE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr2 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str2 = "concept_letter";
  sc_dictionary_append(dictionary, str2, strlen(str2), &addr2);
  REQUIRE(sc_dictionary_is_in(dictionary, str2));
  REQUIRE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr3 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str3 = "sc_node_norole_relation";
  sc_dictionary_append(dictionary, str3, strlen(str3), &addr3);
  REQUIRE(sc_dictionary_is_in(dictionary, str3));
  REQUIRE(sc_dictionary_is_in(dictionary, str2));
  REQUIRE(sc_dictionary_is_in(dictionary, str1));

  sc_addr addr4 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str4 = "sc_node_role_relation";
  sc_dictionary_append(dictionary, str4, strlen(str4), &addr4);
  REQUIRE(sc_dictionary_is_in(dictionary, str4));
  REQUIRE(sc_dictionary_is_in(dictionary, str3));
  REQUIRE(sc_dictionary_is_in(dictionary, str2));
  REQUIRE(sc_dictionary_is_in(dictionary, str1));

  REQUIRE(sc_dictionary_remove(dictionary, str4));
  REQUIRE(!sc_dictionary_is_in(dictionary, str4));
  REQUIRE(sc_dictionary_is_in(dictionary, str3));
  REQUIRE(sc_dictionary_is_in(dictionary, str2));
  REQUIRE(sc_dictionary_is_in(dictionary, str1));

  REQUIRE(sc_dictionary_remove(dictionary, str3));
  REQUIRE(!sc_dictionary_is_in(dictionary, str4));
  REQUIRE(!sc_dictionary_is_in(dictionary, str3));
  REQUIRE(sc_dictionary_is_in(dictionary, str2));
  REQUIRE(sc_dictionary_is_in(dictionary, str1));

  REQUIRE(sc_dictionary_remove(dictionary, str2));
  REQUIRE(!sc_dictionary_is_in(dictionary, str4));
  REQUIRE(!sc_dictionary_is_in(dictionary, str3));
  REQUIRE(!sc_dictionary_is_in(dictionary, str2));
  REQUIRE(sc_dictionary_is_in(dictionary, str1));

  REQUIRE(sc_dictionary_remove(dictionary, str1));
  REQUIRE(!sc_dictionary_is_in(dictionary, str4));
  REQUIRE(!sc_dictionary_is_in(dictionary, str3));
  REQUIRE(!sc_dictionary_is_in(dictionary, str2));
  REQUIRE(!sc_dictionary_is_in(dictionary, str1));

  sc_dictionary_show(dictionary);

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("sc-links", "[test sc-links common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc-links common");

  ScAddr formulaAddr = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(ctx.HelperSetSystemIdtf("atomic_formula", formulaAddr));
  REQUIRE(formulaAddr == ctx.HelperFindBySystemIdtf("atomic_formula"));
  REQUIRE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));

  ScAddr node1 = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node1.IsValid());
  REQUIRE(ctx.HelperSetSystemIdtf("node1", node1));
  REQUIRE("node1" == ctx.HelperGetSystemIdtf(node1));

  ScAddr node2 = ctx.HelperResolveSystemIdtf("_node2", ScType::NodeVarStruct);
  REQUIRE(node2.IsValid());
  REQUIRE("_node2" == ctx.HelperGetSystemIdtf(node2));

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("sc-links-smoke", "[test sc-links smoke]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc-links smoke");

  ScAddr formulaAddr = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(ctx.HelperSetSystemIdtf("atomic_formula", formulaAddr));
  REQUIRE(formulaAddr == ctx.HelperFindBySystemIdtf("atomic_formula"));
  REQUIRE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));
  REQUIRE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));

  ScAddr node1 = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node1.IsValid());
  REQUIRE(!ctx.HelperSetSystemIdtf("atomic_formula", node1));
  REQUIRE(ctx.HelperGetSystemIdtf(node1).empty());

  node1 = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node1.IsValid());
  REQUIRE(node1 != ctx.HelperResolveSystemIdtf("atomic_formula", ScType::NodeConst));
  REQUIRE(formulaAddr == ctx.HelperResolveSystemIdtf("atomic_formula", ScType::NodeConst));
  REQUIRE("atomic_formula" == ctx.HelperGetSystemIdtf(formulaAddr));

  REQUIRE(ctx.FindLinksByContent("atomic_formula").size() == 1);

  ScAddr linkAddr = ctx.CreateLink();
  ScLink link = ScLink(ctx, linkAddr);
  std::string str = "atomic_formula";
  link.Set(str);
  REQUIRE("atomic_formula" == link.GetAsString());

  ScAddrVector linkList = ctx.FindLinksByContent("atomic_formula");
  REQUIRE(linkList.size() == 2);
  REQUIRE(linkList[0].IsValid());
  REQUIRE(linkList[1].IsValid());

  str = "non_atomic_formula";
  link.Set(str);
  REQUIRE("non_atomic_formula" == link.GetAsString());

  REQUIRE(ctx.FindLinksByContent("atomic_formula").size() == 1);

  REQUIRE(ctx.FindLinksByContent("non_atomic_formula").size() == 1);

  ScAddr node2 = ctx.HelperResolveSystemIdtf("_node2", ScType::NodeVarStruct);
  REQUIRE(node2.IsValid());
  REQUIRE("_node2" == ctx.HelperGetSystemIdtf(node2));

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("sc-link-content-changed", "[test sc-links content changed]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc-link content changed");

  ScAddr linkAddr = ctx.CreateLink();
  REQUIRE(linkAddr.IsValid());
  ScLink link = ScLink(ctx, linkAddr);

  std::string str = "content1";
  REQUIRE(link.Set(str));

  REQUIRE(str == link.GetAsString());

  str = "content2";
  REQUIRE(link.Set(str));
  REQUIRE(str == link.GetAsString());

  str = "content3";
  REQUIRE(link.Set(str));
  REQUIRE(str == link.GetAsString());

  REQUIRE(!ctx.FindLinksByContent("content3").empty());
  REQUIRE(ctx.FindLinksByContent("content1").empty());
  REQUIRE(ctx.FindLinksByContent("content1").empty());
  REQUIRE(ctx.FindLinksByContent("content2").empty());

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("set system idtf", "[test set system idtf]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "set system idtf");

  ScAddr timestamp = ctx.CreateNode(ScType::NodeConstClass);
  REQUIRE(ctx.HelperSetSystemIdtf("2022.03.21 16:07:47", timestamp));
  REQUIRE("2022.03.21 16:07:47" == ctx.HelperGetSystemIdtf(timestamp));

  REQUIRE(!ctx.FindLinksByContent("2022.03.21 16:07:47").empty());

  timestamp = ctx.CreateNode(ScType::NodeConstClass);
  REQUIRE(ctx.HelperSetSystemIdtf("2022.03.21 16:07:48", timestamp));
  REQUIRE("2022.03.21 16:07:48" == ctx.HelperGetSystemIdtf(timestamp));

  REQUIRE(!ctx.FindLinksByContent("2022.03.21 16:07:48").empty());
  REQUIRE(!ctx.FindLinksByContent("2022.03.21 16:07:47").empty());

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
