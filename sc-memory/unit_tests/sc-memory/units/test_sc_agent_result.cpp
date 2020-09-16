/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "catch2/catch.hpp"
#include "test_sc_agent_result.hpp"
#include "sc-memory/sc_wait.hpp"
#include "sc-test-framework/sc_test_unit.hpp"

ScAddr ATestResultOk::ms_keynodeTestResultOk;

// Implement just one test, because whole logic depends on ScAgentAction::GetResultCodeAddr funciton
// TODO: test for ScAgentAction::GetResultCodeAddr return results
SC_AGENT_ACTION_IMPLEMENTATION(ATestResultOk)
{
  return SC_RESULT_OK;
}

TEST_CASE("ATestResultOk", "[test sc agent result]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  // uncomment to run more complex test
  //for (size_t i = 0; i < 50; ++i)
  {
    ScAgentInit(true);
    ATestResultOk::InitGlobal();

    SC_AGENT_REGISTER(ATestResultOk);

    ScMemoryContext ctx(sc_access_lvl_make_min, "ATestResultOk");
    try
    {
      ScAddr const cmdAddr = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(cmdAddr.IsValid());
      ScAddr const edge = ctx.CreateEdge(
            ScType::EdgeAccessConstPosPerm,
            ATestResultOk::ms_keynodeTestResultOk,
            cmdAddr);

      REQUIRE(edge.IsValid());

      ScWaitActionFinished waiter(ctx, cmdAddr);
      waiter.SetOnWaitStartDelegate(
            [&cmdAddr]()
            {
              ScMemoryContext ctxLocal(sc_access_lvl_make_min, "ATestResultOk_init");
              ScAgentAction::InitiateCommand(ctxLocal, cmdAddr);
            });
      REQUIRE(waiter.Wait());

      // check result
      REQUIRE(ScAgentAction::GetCommandResultCode(ctx, cmdAddr) == SC_RESULT_OK);
      REQUIRE(ScAgentAction::GetCommandState(ctx, cmdAddr) == ScAgentAction::State::Finished);
    } catch (...)
    {
      SC_LOG_ERROR("Test \"ATestResultOk\" failed")
    }
    SC_AGENT_UNREGISTER(ATestResultOk);

    ctx.Destroy();
  }

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("AgentResultsCommon", "[test sc agent result]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  try
  {
    ScAgentInit(true);

    auto const CheckValue = [](sc_result resCode)
    {
      ScAddr const & addr = ScKeynodes::GetResultCodeAddr(resCode);
      sc_result const res = ScKeynodes::GetResultCodeByAddr(addr);
      REQUIRE(resCode == res);
    };

    CheckValue(SC_RESULT_UNKNOWN);
    CheckValue(SC_RESULT_NO);
    CheckValue(SC_RESULT_ERROR);
    CheckValue(SC_RESULT_OK);
    CheckValue(SC_RESULT_ERROR_INVALID_PARAMS);
    CheckValue(SC_RESULT_ERROR_INVALID_TYPE);
    CheckValue(SC_RESULT_ERROR_IO);
    CheckValue(SC_RESULT_ERROR_INVALID_STATE);
    CheckValue(SC_RESULT_ERROR_NOT_FOUND);
    CheckValue(SC_RESULT_ERROR_NO_WRITE_RIGHTS);
    CheckValue(SC_RESULT_ERROR_NO_READ_RIGHTS);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"AgentResultsCommon\" failed")
  }

  test::ScTestUnit::ShutdownMemory(false);
}
