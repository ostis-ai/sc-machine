/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/coreKeynodes.hpp"
#include "../generated/ANumberComparison.generated.hpp"

namespace scAgentsCommon
{

class ANumberComparison : public ScAgent
{
  SC_CLASS(Agent, Event(CoreKeynodes::question_numbers_comparison, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace scAgentsCommon
