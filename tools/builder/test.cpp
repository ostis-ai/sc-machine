/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/sc_memory.hpp>
#include <sc-core/sc-store/sc_types.h>
#include "sc-test-framework/sc_test_unit.hpp"
#include "catch2/catch.hpp"
#include "src/scs_translator.hpp"
#include "src/scs_loader.hpp"

TEST_CASE("Alex test", "[alex test]")
{
    test::ScTestUnit::InitMemory("sc-memory.ini", "");
    ScMemoryContext ctx(sc_access_lvl_make_min, "alex_test");

    ScsLoader scsLoader;
    scsLoader.loadScsFile(ctx, "/home/alexander/work/0.6.0new/ostis/ims.ostis.kb/to_check/G0.scs");
    ScAddr actionEdge=ctx.HelperFindBySystemIdtf("a");

//  ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, "", actionEdge);

  REQUIRE(actionEdge.IsValid());

  ScAddr node=ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node.IsValid());

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}


