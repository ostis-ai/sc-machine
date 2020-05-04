/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "common/keynodes/CoreKeynodes.hpp"
#include "common/agents/ANumberComparison.hpp"
#include "inference/keynodes/InferenceKeynodes.hpp"
#include "inference/agents/AReverseInference.hpp"
#include "inference/agents/ATemplateGeneration.hpp"

#include "sc_agents_common.hpp"

using namespace scAgentsCommon;

SC_IMPLEMENT_MODULE(SCAgentsCommonModule)

sc_result SCAgentsCommonModule::InitializeImpl()
{

  if (!CoreKeynodes::InitGlobal())
    return SC_RESULT_ERROR;

  if (!InferenceKeynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(ANumberComparison)
  SC_AGENT_REGISTER(ATemplateGeneration)
  SC_AGENT_REGISTER(AReverseInference)

  return SC_RESULT_OK;
}

sc_result SCAgentsCommonModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(ANumberComparison)
  SC_AGENT_UNREGISTER(ATemplateGeneration)
  SC_AGENT_UNREGISTER(AReverseInference)

  return SC_RESULT_OK;
}
