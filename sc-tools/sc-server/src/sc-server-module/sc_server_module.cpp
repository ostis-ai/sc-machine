/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_module.hpp"

#include "../sc_server_setup.hpp"

extern "C"
{
_SC_EXTERN sc_result
sc_module_initialize_with_init_memory_generated_structure(sc_addr const init_memory_generated_structure_addr)
{
  ScServerModuleInstance->Initialize(ScMemory::ms_globalContext, init_memory_generated_structure_addr);

  // It is backward compatible logic. When all platform-dependent components will be configured from kb it will be
  // removed.
  ScConfig config{ScMemory::ms_configPath, {{"log_file"}}};
  ScConfigGroup serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    ms_serverParams.Insert({key, serverConfig[key]});

  RunServer(ms_serverParams, m_server);

  return SC_RESULT_OK;
}

_SC_EXTERN sc_result sc_module_shutdown()
{
  StopServer(m_server);
  m_server = nullptr;

  ScServerModuleInstance->Shutdown(ScMemory::ms_globalContext);

  ScModule::ms_coreModule.m_modules.remove(ScServerModuleInstance);
  delete ScServerModuleInstance;
  ScServerModuleInstance = nullptr;

  return SC_RESULT_OK;
}
}
