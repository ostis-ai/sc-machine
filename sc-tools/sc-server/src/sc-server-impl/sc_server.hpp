/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"

#include "sc_server_log.hpp"

class ScServer
{
public:
  explicit ScServer(
      std::string hostName,
      size_t port,
      std::string const & logType,
      std::string const & logFile,
      std::string const & logLevel,
      sc_memory_params params);

  void Run();

  void Stop();

  void Shutdown();

  virtual void EmitActions() = 0;

  virtual sc_bool IsWorkable() = 0;

  std::string GetUri();

  ScServerConnections * GetConnections();

  void Send(ScServerConnectionHandle const & hdl, std::string const & message, ScServerMessageType type);

  void LogMessage(ScServerLogChannel channel, std::string const & message);

  void LogError(ScServerLogChannel channel, std::string const & errorMessage);

  void ClearLogOptions();

  void SetMessageChannels(ScServerLogChannel channels);

  void SetErrorChannels(ScServerLogChannel channels);

  void CloseConnection(ScServerConnectionHandle const & hdl, ScServerCloseCode const code, std::string const & reason);

  virtual void OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg) = 0;

  virtual ~ScServer();

protected:
  std::atomic<sc_bool> m_isServerRun = SC_FALSE;
  std::string m_hostName;
  ScServerPort m_port;

  size_t m_updateStatisticsPeriod;
  size_t m_saveMemoryPeriod;

  ScMemoryContext * m_context;

  ScServerCore * m_instance;
  ScServerConnections * m_connections;

  ScServerLog * m_log;

  virtual void Initialize() = 0;

  virtual void AfterInitialize() = 0;

  void Timer(std::function<void()> const & callback, size_t callTime);

  void UpdateMemoryStatistics();

  void SaveMemory();

  virtual void OnOpen(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnClose(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg) = 0;

private:
  std::thread m_updateStatisticsThread;
  std::thread m_saveMemoryThread;

  std::thread m_ioThread;
  std::thread m_actionsThread;
};
