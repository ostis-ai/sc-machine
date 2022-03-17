#include "test_sc_agent.hpp"

/// --------------------------------------
TestWaiter ATestAction::msWaiter;

SC_AGENT_ACTION_IMPLEMENTATION(ATestAction)
{
  msWaiter.Unlock();
  return SC_RESULT_ERROR;
}

/// --------------------------------------
ScAddr ATestAddInputEdge::msAgentKeynode;
TestWaiter ATestAddInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScAddr ATestAddOutputEdge::msAgentKeynode;
TestWaiter ATestAddOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScAddr ATestRemoveInputEdge::msAgentKeynode;
TestWaiter ATestRemoveInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScAddr ATestRemoveOutputEdge::msAgentKeynode;
TestWaiter ATestRemoveOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScAddr ATestRemoveElement::msAgentKeynode;
TestWaiter ATestRemoveElement::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveElement)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScAddr ATestContentChanged::msAgentKeynode;
TestWaiter ATestContentChanged::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestContentChanged)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}
