/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "test_sc_agent_commands.generated.hpp"

class ATestCommandEmit : public ScAgentAction
{
  SC_CLASS(Agent, CmdClass("command_test_init__"))
  SC_GENERATED_BODY()


public:
  SC_PROPERTY(Keynode("test_param_1"), ForceCreate(ScType::NodeConstMaterial))
  static ScAddr ms_keynodeParam1;

  SC_PROPERTY(Keynode("test_param_2"), ForceCreate(ScType::NodeConst))
  static ScAddr ms_keynodeParam2;
};
