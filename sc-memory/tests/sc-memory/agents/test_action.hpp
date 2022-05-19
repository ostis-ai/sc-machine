#pragma once

#include "sc-memory/kpm/sc_agent.hpp"

#include "test_action.generated.hpp"

class ATestActionEmit : public ScAgentAction
{
  SC_CLASS(Agent, CmdClass("action_test_init__"))
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("test_param_1"), ForceCreate(ScType::NodeConstMaterial))
  static ScAddr ms_keynodeParam1;

  SC_PROPERTY(Keynode("test_param_2"), ForceCreate(ScType::NodeConst))
  static ScAddr ms_keynodeParam2;
};
