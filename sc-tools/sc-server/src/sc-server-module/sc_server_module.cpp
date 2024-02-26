/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_module.hpp"

#include "../sc_server_setup.hpp"

ScParams ScServerModule::sServerParams;

SC_IMPLEMENT_MODULE(ScServerModule)

sc_result ScServerModule::InitializeImpl()
{
  // It is backward compatible logic. When all platform-dependent components will be configured from kb it will be
  // removed.
  ScConfig config{ScMemory::ms_configPath, {{"log_file"}}};
  ScConfigGroup serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    sServerParams.Insert({key, serverConfig[key]});

  RunServer(sServerParams, m_server);

  return SC_RESULT_OK;
}

sc_result ScServerModule::ShutdownImpl()
{
  StopServer(m_server);

  return SC_RESULT_OK;
}
