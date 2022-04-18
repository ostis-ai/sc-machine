/*
 * Copyright (c) 2022 Intelligent Semantic Systems LLC, All rights reserved.
 */

#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "keynodes/coreKeynodes.hpp"
#include "utils/AgentUtils.hpp"

#include "GenerationByTemplateAgent.generated.hpp"

namespace scAgentsCommon
{
class GenerationByTemplateAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool checkActionClass(ScAddr const & actionAddr);

  void fillGenParams(ScAddr & parameterNodeAddr, ScTemplateParams & genParams);

  bool generateByTemplate(ScTemplateParams & genParams, ScAddr & ruleStatement);
};
}  // namespace scAgentsCommon
