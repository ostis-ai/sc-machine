/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"

extern "C"
{
#include "sc-core/sc-store/sc-string-tree/sc_string_tree.h"
}

TEST_CASE("sc-strings-complex", "[test sc-string tree complex]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc-strings-complex");

  sc_addr addr1 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str1 = "concept_set1";
  sc_string_tree_append(addr1, str1, strlen(str1));
  REQUIRE(strcmp(str1, sc_string_tree_get_sc_string(addr1)) == 0);
  REQUIRE(sc_string_tree_is_in(str1));

  sc_addr addr2 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str2 = "concept_letter";
  sc_string_tree_append(addr2, str2, strlen(str2));
  REQUIRE(strcmp(str2, sc_string_tree_get_sc_string(addr2)) == 0);
  REQUIRE(sc_string_tree_is_in(str2));
  REQUIRE(sc_string_tree_is_in(str1));

  sc_addr addr3 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str3 = "sc_node_norole_relation";
  sc_string_tree_append(addr3, str3, strlen(str3));
  REQUIRE(strcmp(str3, sc_string_tree_get_sc_string(addr3)) == 0);
  REQUIRE(sc_string_tree_is_in(str3));
  REQUIRE(sc_string_tree_is_in(str2));
  REQUIRE(sc_string_tree_is_in(str1));

  sc_addr addr4 = sc_memory_link_new(ctx.GetRealContext());
  sc_char * str4 = "sc_node_role_relation";
  sc_string_tree_append(addr4, str4, strlen(str4));
  REQUIRE(strcmp(str4, sc_string_tree_get_sc_string(addr4)) == 0);
  REQUIRE(sc_string_tree_is_in(str4));
  REQUIRE(sc_string_tree_is_in(str3));
  REQUIRE(sc_string_tree_is_in(str2));
  REQUIRE(sc_string_tree_is_in(str1));

  REQUIRE(sc_string_tree_remove(str4));
  REQUIRE(!sc_string_tree_is_in(str4));
  REQUIRE(sc_string_tree_is_in(str3));
  REQUIRE(sc_string_tree_is_in(str2));
  REQUIRE(sc_string_tree_is_in(str1));

  REQUIRE(sc_string_tree_remove(str3));
  REQUIRE(!sc_string_tree_is_in(str4));
  REQUIRE(!sc_string_tree_is_in(str3));
  REQUIRE(sc_string_tree_is_in(str2));
  REQUIRE(sc_string_tree_is_in(str1));

  REQUIRE(sc_string_tree_remove(str2));
  REQUIRE(!sc_string_tree_is_in(str4));
  REQUIRE(!sc_string_tree_is_in(str3));
  REQUIRE(!sc_string_tree_is_in(str2));
  REQUIRE(sc_string_tree_is_in(str1));

  REQUIRE(sc_string_tree_remove(str1));
  REQUIRE(!sc_string_tree_is_in(str4));
  REQUIRE(!sc_string_tree_is_in(str3));
  REQUIRE(!sc_string_tree_is_in(str2));
  REQUIRE(!sc_string_tree_is_in(str1));

  sc_string_tree_show();

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(true);
}

TEST_CASE("sc-links", "[test sc-links common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc-links common");

  ScAddr formulaAddr = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(ctx.HelperSetSystemIdtf("atomic_formula", formulaAddr));
  sc_string_tree_show();
  ScAddr gotten = ctx.HelperFindBySystemIdtf("atomic_formula");
  REQUIRE(formulaAddr == gotten);
  std::string idtf = ctx.HelperGetSystemIdtf(formulaAddr);
  REQUIRE(idtf == "atomic_formula");

  ScAddr addr = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(ctx.HelperSetSystemIdtf("node1", addr));

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(true);
}
