/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_module.hpp"

#include <cstdlib>

#include "sc_server_setup.hpp"

SC_MODULE_REGISTER(ScServerModule);

std::shared_ptr<ScServer> ScServerModule::m_server;
ScParams ScServerModule::ms_serverParams;

void ScServerModule::Initialize(ScMemoryContext *)
{
  // TODO(NikitaZotov): Configure all platform-dependent components from kb.

  sc_char const * host = std::getenv("SC_SERVER_HOST");
  if (host != nullptr)
    ms_serverParams.Insert({"host", host});

  sc_char const * port = std::getenv("SC_SERVER_PORT");
  if (port != nullptr)
    ms_serverParams.Insert({"port", port});

  ScConfig config{ScMemory::ms_configPath, {{"log_file"}}};
  ScConfigGroup serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    ms_serverParams.Insert({key, serverConfig[key]});

  RunServer(ms_serverParams, m_server);
}

void ScServerModule::Shutdown(ScMemoryContext *)
{
  StopServer(m_server);
  m_server = nullptr;
}
