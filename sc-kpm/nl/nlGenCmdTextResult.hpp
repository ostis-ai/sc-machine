/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "nlGenCmdTextResult.generated.hpp"

namespace nl
{

class AGenCmdTextResult final : public ScAgentAction
{
  SC_CLASS(Agent, CmdClass("command_generate_cmd_result_text"))
  SC_GENERATED_BODY()
  
protected:
  
  // keynodes
  
  /// TODO: make support of current language
  SC_PROPERTY(Keynode("lang_en"), ForceCreate(ScType::NodeConstRole))
  static ScAddr ms_kLangEn;

  SC_PROPERTY(Keynode("command"))
  static ScAddr ms_kCommand;

  SC_PROPERTY(Keynode("nrel_text_templates"))
  static ScAddr ms_kNrelTextTemplates;

  SC_PROPERTY(Keynode("nrel_suitable_for"))
  static ScAddr ms_kNrelSuitableFor;

  SC_PROPERTY(Keynode("command_generate_text_from_template"))
  static ScAddr ms_kCommandGetTextTempl;
};

} // namespace nl
