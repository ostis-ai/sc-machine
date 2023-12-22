/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc-memory/sc_memory.hpp"

#include "sc_server_defines.hpp"

#include "sc_server_action.hpp"
#include "sc_server_logger.hpp"

class ScServer
{
public:
  ScMemoryContext * m_context;

  explicit ScServer(std::string hostName, size_t port, sc_memory_params params);

  void Run();

  void Stop();

  void Shutdown();

  virtual void EmitActions() = 0;

  virtual sc_bool IsWorkable() = 0;

  std::string GetUri();

  ScServerUserProcesses * GetConnections();

  virtual sc_bool CheckIfUserProcessAuthorized(ScServerUserProcessId const & userProcessId) = 0;

  void SetChannels(ScServerLogLevel channels);

  void ClearChannels();

  void Send(ScServerUserProcessId const & userProcessId, std::string const & message, ScServerMessageType type);

  void ResetLogger(ScServerLogger * logger = nullptr);

  void LogMessage(ScServerLogLevel channel, std::string const & message);

  void CloseConnection(ScServerUserProcessId const & userProcessId, ScServerCloseCode code, std::string const & reason);

  virtual void OnEvent(ScServerUserProcessId const & userProcessId, std::string const & msg) = 0;

  virtual ~ScServer();

protected:
  std::atomic<sc_bool> m_isServerRun = SC_FALSE;
  std::string m_hostName;
  ScServerPort m_port;

  sc_bool m_memoryState;

  ScServerLogger * m_logger;
  ScServerCore * m_instance;
  ScServerUserProcesses * m_connections;

  virtual void Initialize() = 0;

  virtual void AfterInitialize() = 0;

  virtual void OnOpen(ScServerUserProcessId const & userProcessId) = 0;

  virtual void OnClose(ScServerUserProcessId const & userProcessId) = 0;

  virtual void OnMessage(ScServerUserProcessId const & userProcessId, ScServerMessage const & msg) = 0;

private:
  std::thread m_ioThread;
  std::thread m_actionsThread;
};
