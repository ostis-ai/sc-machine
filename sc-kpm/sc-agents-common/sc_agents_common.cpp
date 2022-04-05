/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "keynodes/coreKeynodes.hpp"

#include "sc_agents_common.hpp"

using namespace scAgentsCommon;

SC_IMPLEMENT_MODULE(SCAgentsCommonModule)

sc_result SCAgentsCommonModule::InitializeImpl()
{
  if (!CoreKeynodes::InitGlobal())
    return SC_RESULT_ERROR;

  return SC_RESULT_OK;
}

sc_result SCAgentsCommonModule::ShutdownImpl()
{
  return SC_RESULT_OK;
}
