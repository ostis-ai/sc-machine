/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/kpm/sc_agent.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/keynodes/eraseElementsKeynodes.hpp"

#include "EraseElementsAgent.generated.hpp"

namespace scAgentsCommon
{
class EraseElementsAgent : public ScAgent
{
  SC_CLASS(Agent, Event(CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

protected:
  bool checkActionClass(ScAddr const & actionNode);
};

}  // namespace scAgentsCommon
