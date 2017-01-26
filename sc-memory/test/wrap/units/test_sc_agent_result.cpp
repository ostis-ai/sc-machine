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
  ScAgentInit(true);
  ATestResultOk::initGlobal();

  SC_AGENT_REGISTER(ATestResultOk);
  
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestResultOk");

  ScAddr const cmdAddr = ctx.createNode(ScType::NodeConst);
  SC_CHECK(cmdAddr.isValid(), ());
  ScAddr const edge = ctx.createEdge(
    ScType::EdgeAccessConstPosPerm,
    ATestResultOk::ms_keynodeTestResultOk,
    cmdAddr);

  std::thread work_thread([&ctx, &cmdAddr]
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // initiate command
    ScMemoryContext ctxLocal(sc_access_lvl_make_min, "ATestResultOk_thread");
    ctxLocal.createEdge(ScType::EdgeAccessConstPosPerm, ScAgentAction::GetCommandInitiatedAddr(), cmdAddr);
  });

  work_thread.join();

  ScWaitActionFinished waiter(ctx, cmdAddr);
  SC_CHECK(waiter.Wait(), ());

  // check result
  ScIterator5Ptr it5 = ctx.iterator5(
    cmdAddr,
    ScType::EdgeDCommonConst,
    ScType::NodeConst,
    ScType::EdgeAccessConstPosPerm,
    ScAgentAction::GetNrelResultAddr());
  
  SC_CHECK(it5->next(), ());

  ScIterator3Ptr it = ctx.iterator3(
    ScAgentAction::GetResultCodeAddr(SC_RESULT_OK),
    ScType::EdgeAccessConstPosPerm,
    it5->value(2));

  SC_CHECK(it->next(), ());

  SC_AGENT_UNREGISTER(ATestResultOk);
}