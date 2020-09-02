/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "catch2/catch.hpp"

#include "test_sc_agent_commands.hpp"

#include "sc-memory/cpp/sc_wait.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

#include <thread>

ScAddr ATestCommandEmit::ms_keynodeParam1;
ScAddr ATestCommandEmit::ms_keynodeParam2;

utils::ScLock testInitLock;
SC_AGENT_ACTION_IMPLEMENTATION(ATestCommandEmit)
{
  if (GetParam(requestAddr, 0) == ATestCommandEmit::ms_keynodeParam1 &&
      GetParam(requestAddr, 1) == ATestCommandEmit::ms_keynodeParam2)
  {
    testInitLock.Unlock();
    return SC_RESULT_OK;
  }

  testInitLock.Unlock();
  return SC_RESULT_ERROR;
}

TEST_CASE("ATestCommandEmit", "[test sc agent commands]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext * ctx = new ScMemoryContext(sc_access_lvl_make_min, "ATestCommandEmit");
  ATestCommandEmit::InitGlobal();

  SC_AGENT_REGISTER(ATestCommandEmit);

  testInitLock.Lock();
  ScAddr const cmd = ScAgentAction::CreateCommand(
        *ctx,
        ATestCommandEmit::GetCommandClassAddr(),
        { ATestCommandEmit::ms_keynodeParam1, ATestCommandEmit::ms_keynodeParam2 });

  REQUIRE(cmd.IsValid());

  ScWaitActionFinished * waiter = new ScWaitActionFinished(*ctx, cmd);
  waiter->SetOnWaitStartDelegate([&]()
                                 {
                                   ScAgentAction::InitiateCommand(*ctx, cmd);
                                 });
  waiter->Wait();
  REQUIRE(ScAgentAction::GetCommandResultCode(*ctx, cmd) == SC_RESULT_OK);

  SC_AGENT_UNREGISTER(ATestCommandEmit);

  ctx->Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
