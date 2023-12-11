/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/kpm/sc_agent.hpp"

#include <nlohmann/json.hpp>

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "GetDecompositionAgent.generated.hpp"

using json = nlohmann::json;

namespace scSearch
{
class GetDecompositionAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool checkActionClass(ScAddr const & actionNode);

  json getJSONDecomposition(ScAddrVector const &, size_t, ScAddr const &, ScAddr const &);

  static ScAddrVector getDecomposition(ScAddr const &, ScAddr const &);
};

}  // namespace scSearch
