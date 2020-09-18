/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"

TEST_CASE("ScMemoryContext_FindLinksByContent", "[test sc memory]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "ScMemoryContext_FindLinksByContent");

  try
  {
    ScAddr const linkAddr1 = ctx.CreateLink();
    REQUIRE(linkAddr1.IsValid());

    std::string const linkContent1 = "ScMemoryContext_FindLinksByContent_content_1";
    ScLink link1(ctx, linkAddr1);
    REQUIRE(link1.Set(linkContent1));

    ScAddrVector const result = ctx.FindLinksByContent(linkContent1);
    REQUIRE(result.size() == 1);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"ScMemoryContext_FindLinksByContent\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
