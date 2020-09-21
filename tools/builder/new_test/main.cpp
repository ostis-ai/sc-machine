//
// Created by alexander on 21.09.20.
//

#define CATCH_CONFIG_MAIN

#include <sc-memory/sc_memory.hpp>
#include <sc-core/sc-store/sc_types.h>
#include <sc-memory/kpm/sc_agent.hpp>
#include "sc-test-framework/sc_test_unit.hpp"
#include "catch2/catch.hpp"
#include "../src/builder.hpp"

TEST_CASE("Alex test", "[alex test]")
{
    test::ScTestUnit::InitMemory("sc-memory.ini", "");
    //ScMemoryContext ctx(sc_access_lvl_make_min, "alex_test");
    //Builder builder;
    //builder.loadScsFile(ctx, "/home/alexander/work/0.6.0new/ostis/ims.ostis.kb/to_check/G0.scs");
    //ScAddr actionEdge=ctx.HelperFindBySystemIdtf("a");

    //ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, "", actionEdge);

    //REQUIRE(actionEdge.IsValid());
    //REQUIRE(x == 2);

    //ctx.Destroy();
    test::ScTestUnit::ShutdownMemory(false);
}
