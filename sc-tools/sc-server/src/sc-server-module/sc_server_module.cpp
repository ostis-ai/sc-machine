/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_module.hpp"

#include "../sc_server_setup.hpp"

SC_MODULE_REGISTER(ScServerModule);

std::shared_ptr<ScServer> ScServerModule::m_server;
ScParams ScServerModule::ms_serverParams;

sc_result ScServerModule::Initialize(ScMemoryContext *, ScAddr const &)
{
  // It is backward compatible logic. When all platform-dependent components will be configured from kb it will be
  // removed.
  ScConfig config{ScMemory::ms_configPath, {{"log_file"}}};
  ScConfigGroup serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    ms_serverParams.Insert({key, serverConfig[key]});

  RunServer(ms_serverParams, m_server);

  return SC_RESULT_OK;
}

sc_result ScServerModule::Shutdown(ScMemoryContext *)
{
  StopServer(m_server);
  m_server = nullptr;

  return SC_RESULT_OK;
}
