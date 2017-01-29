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

    std::thread work_thread([&ctx, &cmdAddr]
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));

      // initiate command
      ScMemoryContext ctxLocal(sc_access_lvl_make_min, "ATestResultOk_thread");
      ctxLocal.CreateEdge(ScType::EdgeAccessConstPosPerm, ScAgentAction::GetCommandInitiatedAddr(), cmdAddr);
    });

    ScWaitActionFinished waiter(ctx, cmdAddr);
    SC_CHECK(waiter.Wait(), ());

    // check result
    ScIterator5Ptr it5 = ctx.Iterator5(
      cmdAddr,
      ScType::EdgeDCommonConst,
      ScType::NodeConst,
      ScType::EdgeAccessConstPosPerm,
      ScAgentAction::GetNrelResultAddr());

    SC_CHECK(it5->Next(), ());

    ScIterator3Ptr it = ctx.Iterator3(
      ScAgentAction::GetResultCodeAddr(SC_RESULT_OK),
      ScType::EdgeAccessConstPosPerm,
      it5->Get(2));

    SC_CHECK(it->Next(), ());

    work_thread.join();

    SC_AGENT_UNREGISTER(ATestResultOk);
  }
}

UNIT_TEST(AgentResultsCommon)
{
  ScAgentInit(true);

  auto const CheckValue = [](sc_result resCode)
  {
    ScAddr const & addr = ScAgentAction::GetResultCodeAddr(resCode);
    sc_result const res = ScAgentAction::GetResultCodeByAddr(addr);
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
