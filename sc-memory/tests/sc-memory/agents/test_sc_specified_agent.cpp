#include "test_sc_specified_agent.hpp"

/// --------------------------------------

ScResult ATestSpecifiedAgent::DoProgram(ScElementaryEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------
