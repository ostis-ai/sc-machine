/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "nlKeynodes.hpp"

#include "nlDialogue.generated.hpp"

namespace nl
{

class ADialogueProcessMessageAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::msMainNLDialogueInstance, SC_EVENT_ADD_OUTPUT_ARC))
  SC_GENERATED_BODY()

private:
  SC_PROPERTY(Keynode("nrel_output_text"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr ms_kNrelOutputText;

  SC_PROPERTY(Keynode("command_generate_cmd_result_text"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_kCommandGenCmdResultText;
};

} // namespace nl
