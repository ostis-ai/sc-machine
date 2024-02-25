/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_module.hpp"

#include "../sc_server_setup.hpp"

SC_IMPLEMENT_MODULE(ScServerModule)

sc_result ScServerModule::InitializeImpl()
{
  ScConfig config{"", {"repo_path", "extensions_path", "log_file"}};
  auto serverConfig = config["sc-server"];

  ScParams serverParams;
  for (auto const & key : *serverConfig)
    serverParams.Insert({key, serverConfig[key]});

  RunServer(serverParams, m_server);

  return SC_RESULT_OK;
}

sc_result ScServerModule::ShutdownImpl()
{
  StopServer(m_server);

  return SC_RESULT_OK;
}
