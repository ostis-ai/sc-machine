/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "keynodes/coreKeynodes.hpp"
#include "agents/ANumberComparison.hpp"

#include "sc_agents_common.hpp"

using namespace scAgentsCommon;

SC_IMPLEMENT_MODULE(SCAgentsCommonModule)

sc_result SCAgentsCommonModule::InitializeImpl()
{

  if (!CoreKeynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(ANumberComparison)

  return SC_RESULT_OK;
}

sc_result SCAgentsCommonModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(ANumberComparison)

  return SC_RESULT_OK;
}
