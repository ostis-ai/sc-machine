/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/kpm/sc_agent.hpp>

#include "inference/keynodes/InferenceKeynodes.hpp"

#include "sc-kpm/sc-agents-common/generated/AReverseInference.generated.hpp"

using namespace std;

namespace scAgentsCommon
{

class AReverseInference : public ScAgent
{
  SC_CLASS(Agent, Event(InferenceKeynodes::question_reverse_inference, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool generateStatement(ScAddr & statement, const ScTemplateParams & templateParams);

  void createTemplateParams(ScAddr & statement, vector<ScAddr> argumentList, ScTemplateParams & templateParam);

  vector<ScAddr> getRequestKeyElementList(ScAddr & request);

  vector<ScAddr> findRulesWithKeyElement(ScAddr & keyElement);
};

} // namespace scAgentsCommon
