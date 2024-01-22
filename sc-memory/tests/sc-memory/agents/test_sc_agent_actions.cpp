#include <gtest/gtest.h>

#include "sc-memory/sc_wait.hpp"

#include "test_action.hpp"

#include "agents_test_utils.hpp"

ScAddr ATestActionEmit::ms_keynodeParam1;
ScAddr ATestActionEmit::ms_keynodeParam2;

utils::ScLock testInitLock;

SC_AGENT_ACTION_IMPLEMENTATION(ATestActionEmit)
{
  if (GetParam(requestAddr, 0) == ATestActionEmit::ms_keynodeParam1
      && GetParam(requestAddr, 1) == ATestActionEmit::ms_keynodeParam2)
  {
    testInitLock.Unlock();
    return SC_RESULT_OK;
  }

  testInitLock.Unlock();
  return SC_RESULT_ERROR;
}

TEST_F(ScAgentTest, action_emit)
{
  ATestActionEmit::InitGlobal();

  SC_AGENT_REGISTER(ATestActionEmit);

  EXPECT_TRUE(ScAgentAction::GetNrelResultAddr().IsValid());

  testInitLock.Lock();
  ScAddr const cmd = ScAgentAction::CreateCommand(
      *m_ctx,
      ATestActionEmit::GetCommandClassAddr(),
      {ATestActionEmit::ms_keynodeParam1, ATestActionEmit::ms_keynodeParam2});

  EXPECT_TRUE(cmd.IsValid());

  EXPECT_TRUE(ScAgentAction::InitiateCommandWait(*m_ctx, cmd));
  EXPECT_EQ(ScAgentAction::GetCommandResultCode(*m_ctx, ScAddr::Empty), SC_RESULT_UNKNOWN);
  EXPECT_EQ(ScAgentAction::GetCommandResultCode(*m_ctx, cmd), SC_RESULT_OK);
  EXPECT_FALSE(ScAgentAction::IsCommandInitiated(*m_ctx, cmd));
  EXPECT_FALSE(ScAgentAction::IsCommandInProgress(*m_ctx, cmd));
  EXPECT_TRUE(ScAgentAction::IsCommandFinished(*m_ctx, cmd));

  SC_AGENT_UNREGISTER(ATestActionEmit);
}
