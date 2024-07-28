#include "test_sc_specificated_agent.hpp"

/// --------------------------------------

ScResult ATestSpecificatedAgent::DoProgram(ScElementaryEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------
