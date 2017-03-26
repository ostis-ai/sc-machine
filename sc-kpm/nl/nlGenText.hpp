/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "nlGenText.generated.hpp"

namespace nl
{

class AGenText final : public ScAgentAction
{
  SC_CLASS(Agent, CmdClass("command_generate_text_from_template"))
  SC_GENERATED_BODY()
  
protected:
  
};

} // namespace nl
