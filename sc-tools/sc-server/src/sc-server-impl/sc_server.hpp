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
  explicit ScServer(std::string hostName, size_t port, sc_memory_params params);

  void Run();

  void Stop();

  void Shutdown();

  virtual void EmitActions() = 0;

  virtual sc_bool IsWorkable() = 0;

  std::string GetUri();

  ScServerConnections * GetConnections();

  void SetChannels(ScServerLogLevel channels);

  void ClearChannels();

  void Send(ScServerConnectionHandle const & hdl, std::string const & message, ScServerMessageType type);

  void ResetLogger(ScServerLogger * logger = nullptr);

  void LogMessage(ScServerLogLevel channel, std::string const & message);

  void CloseConnection(ScServerConnectionHandle const & hdl, ScServerCloseCode code, std::string const & reason);

  virtual void OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg) = 0;

  virtual ~ScServer();

protected:
  std::atomic<sc_bool> m_isServerRun = SC_FALSE;
  std::string m_hostName;
  ScServerPort m_port;

  sc_bool m_memoryState;
  ScMemoryContext * m_context;

  ScServerLogger * m_logger;
  ScServerCore * m_instance;
  ScServerConnections * m_connections;

  virtual void Initialize() = 0;

  virtual void AfterInitialize() = 0;

  virtual void OnOpen(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnClose(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg) = 0;

private:
  std::thread m_ioThread;
  std::thread m_actionsThread;
};
