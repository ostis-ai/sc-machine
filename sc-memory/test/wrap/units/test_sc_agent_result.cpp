/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "test_sc_agent_result.hpp"

#include "sc-memory/cpp/sc_wait.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

#include <thread>

ScAddr ATestResultOk::ms_keynodeTestResultOk;

// Implement just one test, because whole logic depends on ScAgentAction::GetResultCodeAddr funciton
// TODO: test for ScAgentAction::GetResultCodeAddr return results
SC_AGENT_ACTION_IMPLEMENTATION(ATestResultOk)
{
  return SC_RESULT_OK;
}

UNIT_TEST(ATestResultOk)
{
  // uncomment to run more complex test
  //for (size_t i = 0; i < 50; ++i)
  {
    ScAgentInit(true);
    ATestResultOk::InitGlobal();

    SC_AGENT_REGISTER(ATestResultOk);

    ScMemoryContext ctx(sc_access_lvl_make_min, "ATestResultOk");

    ScAddr const cmdAddr = ctx.CreateNode(ScType::NodeConst);
    SC_CHECK(cmdAddr.IsValid(), ());
    ScAddr const edge = ctx.CreateEdge(
      ScType::EdgeAccessConstPosPerm,
      ATestResultOk::ms_keynodeTestResultOk,
      cmdAddr);

    SC_CHECK(edge.IsValid(), ());

    ScWaitActionFinished waiter(ctx, cmdAddr);
    waiter.SetOnWaitStartDelegate([&cmdAddr]()
    {
      ScMemoryContext ctxLocal(sc_access_lvl_make_min, "ATestResultOk_init");
      ScAgentAction::InitiateCommand(ctxLocal, cmdAddr);
    });
    SC_CHECK(waiter.Wait(), ());

    // check result
    SC_CHECK_EQUAL(ScAgentAction::GetCommandResultCode(ctx, cmdAddr), SC_RESULT_OK, ());
    SC_CHECK_EQUAL(ScAgentAction::GetCommandState(ctx, cmdAddr), ScAgentAction::State::Finished, ());

    SC_AGENT_UNREGISTER(ATestResultOk);
  }
}

UNIT_TEST(AgentResultsCommon)
{
  ScAgentInit(true);

  auto const CheckValue = [](sc_result resCode)
  {
    ScAddr const & addr = ScKeynodes::GetResultCodeAddr(resCode);
    sc_result const res = ScKeynodes::GetResultCodeByAddr(addr);
    SC_CHECK_EQUAL(resCode, res, ());
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
}
