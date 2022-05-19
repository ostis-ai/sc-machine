#include <gtest/gtest.h>

#include "sc-memory/sc_wait.hpp"

#include "agents_test_utils.hpp"

#include <thread>

#include "test_sc_agent_result.hpp"

ScAddr ATestResultOk::ms_keynodeTestResultOk;

// Implement just one test, because whole logic depends on ScAgentAction::GetResultCodeAddr funciton
// TODO: test for ScAgentAction::GetResultCodeAddr return results
SC_AGENT_ACTION_IMPLEMENTATION(ATestResultOk)
{
return SC_RESULT_OK;
}

TEST_F(ScAgentTest, ATestResultOk)
{
  for (size_t i = 0; i < 50; ++i)
  {
    ScAgentInit(true);
    ATestResultOk::InitGlobal();

    SC_AGENT_REGISTER(ATestResultOk);

    ScMemoryContext ctx(sc_access_lvl_make_min, "ATestResultOk");

    ScAddr const cmdAddr = ctx.CreateNode(ScType::NodeConst);
    EXPECT_TRUE(cmdAddr.IsValid());
    ScAddr const edge = ctx.CreateEdge(
          ScType::EdgeAccessConstPosPerm,
          ATestResultOk::ms_keynodeTestResultOk,
          cmdAddr);

    EXPECT_TRUE(edge.IsValid());

    ScWaitActionFinished waiter(ctx, cmdAddr);
    waiter.SetOnWaitStartDelegate([&cmdAddr]()
                                  {
                                    ScMemoryContext ctxLocal(sc_access_lvl_make_min, "ATestResultOk_init");
                                    ScAgentAction::InitiateCommand(ctxLocal, cmdAddr);
                                  });
    EXPECT_TRUE(waiter.Wait());

    // check result
    EXPECT_EQ(ScAgentAction::GetCommandResultCode(ctx, cmdAddr), SC_RESULT_OK);
    EXPECT_EQ(ScAgentAction::GetCommandState(ctx, cmdAddr), ScAgentAction::State::Finished);

    SC_AGENT_UNREGISTER(ATestResultOk);
  }
}

TEST_F(ScAgentTest, AgentResult_Codes)
{
  ScAgentInit(true);

  auto const CheckValue = [](sc_result resCode)
  {
    ScAddr const & addr = ScKeynodes::GetResultCodeAddr(resCode);
    sc_result const res = ScKeynodes::GetResultCodeByAddr(addr);
    EXPECT_EQ(resCode, res);
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
