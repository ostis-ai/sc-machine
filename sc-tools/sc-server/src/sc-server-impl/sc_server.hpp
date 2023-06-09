/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc-memory/sc_memory.hpp"

#include "sc_server_defines.hpp"
#include "sc_server_log.hpp"

#include "sc_server_action.hpp"

class ScServer
{
public:
  explicit ScServer(
      std::string hostName,
      size_t port,
      std::string const & logType,
      std::string const & logFile,
      std::string const & logLevel,
      sc_memory_params params)
    : m_hostName(std::move(hostName))
    , m_port(port)
  {
    m_updateStatisticsPeriod = params.update_period;
    m_saveMemoryPeriod = params.save_period;

    m_instance = new ScServerCore();
    {
      LogMessage(ScServerLogMessages::app, "Sc-server socket data");
      LogMessage(ScServerLogMessages::app, "\tHost name: " + m_hostName);
      LogMessage(ScServerLogMessages::app, "\tPort: " + std::to_string(m_port));
    }
    {
      LogMessage(ScServerLogMessages::app, "Sc-server log");
      LogMessage(ScServerLogMessages::app, "\tLog type: " + logType);
      LogMessage(
          ScServerLogMessages::app,
          "\tLog file: " + ((logType != SC_SERVER_FILE_TYPE || logFile.empty()) ? "No" : logFile));
      LogMessage(ScServerLogMessages::app, "\tLog level: " + logLevel);
    }
    LogMessage(ScServerLogMessages::app, "Sc-server initialized");
    m_log = new ScServerLog(m_instance, logType, logFile, logLevel);

    m_memoryState = ScMemory::Initialize(params);
    if (m_memoryState)
      m_context = new ScMemoryContext("sc-server");

    m_connections = new ScServerConnections();
  }

  void Run()
  {
    if (!m_memoryState)
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Sc-server sc-memory is invalid");

    m_isServerRun = SC_TRUE;
    m_instance->init_asio();
    m_instance->set_reuse_addr(SC_TRUE);

    Initialize();

    m_instance->listen({boost::asio::ip::address::from_string(m_hostName), sc_uint16(m_port)});
    m_instance->start_accept();

    LogMessage(ScServerLogMessages::app, "Start lookup memory state");
    m_updateStatisticsThread = std::thread(&ScServer::UpdateMemoryStatistics, &*this);
    m_saveMemoryThread = std::thread(&ScServer::SaveMemory, &*this);

    LogMessage(ScServerLogMessages::app, "Start actions processing");
    m_actionsThread = std::thread(&ScServer::EmitActions, &*this);

    LogMessage(ScServerLogMessages::app, "Start input-output processing");
    m_ioThread = std::thread(&ScServerCore::run, &*m_instance);
  }

  void Stop()
  {
    m_isServerRun = SC_FALSE;
    m_instance->stop_listening();

    AfterInitialize();

    if (m_actionsThread.joinable())
    {
      LogMessage(ScServerLogMessages::app, "Close actions processing");
      m_actionsThread.join();
    }

    if (m_ioThread.joinable())
    {
      LogMessage(ScServerLogMessages::app, "Close input-output processing");

      for (auto & it : *m_connections)
      {
        try
        {
          m_instance->close(it, websocketpp::close::status::normal, "I finish work");
        }
        catch (std::exception const & ex)
        {
          LogError(ScServerLogErrors::devel, ex.what());
        }
      }

      m_instance->stop();
      m_ioThread.join();
    }

    if (m_updateStatisticsThread.joinable())
      m_updateStatisticsThread.join();

    if (m_saveMemoryThread.joinable())
      m_saveMemoryThread.join();
  }

  void Shutdown()
  {
    ClearLogOptions();

    delete m_connections;
    m_connections = nullptr;

    if (m_memoryState)
    {
      delete m_context;
    }
    ScMemory::Shutdown();

    LogMessage(ScServerLogMessages::app, "Shutdown sc-server");

    delete m_instance;
    m_instance = nullptr;

    delete m_log;
    m_log = nullptr;
  }

  virtual void EmitActions() = 0;

  virtual sc_bool IsWorkable() = 0;

  std::string GetUri()
  {
    return "ws://" + m_hostName + ":" + std::to_string(m_port);
  }

  ScServerConnections * GetConnections()
  {
    return m_connections;
  }

  void Send(ScServerConnectionHandle const & hdl, std::string const & message, ScServerMessageType type)
  {
    m_instance->send(hdl, message, type);
  }

  void LogMessage(ScServerLogChannel channel, std::string const & message)
  {
    m_instance->get_alog().write(channel, message);
  }

  void LogError(ScServerLogChannel channel, std::string const & errorMessage)
  {
    m_instance->get_elog().write(channel, errorMessage);
  }

  void ClearLogOptions()
  {
    delete m_log;
    m_log = new ScServerLog(m_instance, SC_SERVER_CONSOLE_TYPE, "", SC_SERVER_INFO_LEVEL);
  }

  void SetMessageChannels(ScServerLogChannel channels)
  {
    m_instance->set_access_channels(channels);
  }

  void SetErrorChannels(ScServerLogChannel channels)
  {
    m_instance->set_access_channels(channels);
  }

  void CloseConnection(ScServerConnectionHandle const & hdl, ScServerCloseCode const code, std::string const & reason)
  {
    m_instance->close(hdl, code, reason);
  }

  virtual void OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg) = 0;

  virtual ~ScServer()
  {
    Shutdown();
  }

protected:
  std::atomic<sc_bool> m_isServerRun = SC_FALSE;
  std::string m_hostName;
  ScServerPort m_port;

  size_t m_updateStatisticsPeriod;
  size_t m_saveMemoryPeriod;

  sc_bool m_memoryState;
  ScMemoryContext * m_context;

  ScServerCore * m_instance;
  ScServerConnections * m_connections;

  ScServerLog * m_log;

  virtual void Initialize() = 0;

  virtual void AfterInitialize() = 0;

  void Timer(std::function<void()> const & callback, size_t callTime)
  {
    size_t currentTime = 0;
    size_t delta = 1;
    while (m_isServerRun)
    {
      if (currentTime >= callTime)
      {
        callback();
        currentTime = 0;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(delta * 1000));
      currentTime += delta;
    }
  }

  void UpdateMemoryStatistics()
  {
    auto const PrintLine = [](std::string const & name, uint32_t num, float percent) {
      SC_LOG_INFO(name << ": " << num << " (" << percent << "%)");
    };

    auto const PrintStatistics = [this, PrintLine]() {
      ScMemoryContext::Stat const & stats = m_context->CalculateStat();

      SC_LOG_INFO("Statistics");
      size_t const allElements = stats.GetAllNum();
      PrintLine("Nodes", stats.m_nodesNum, float(stats.m_nodesNum) / float(allElements) * 100);
      PrintLine("Edges", stats.m_edgesNum, float(stats.m_edgesNum) / float(allElements) * 100);
      PrintLine("Links", stats.m_linksNum, float(stats.m_linksNum) / float(allElements) * 100);
      SC_LOG_INFO("Total: " << allElements);
    };

    Timer([&PrintStatistics]() {
      SC_LOG_INFO("Dump sc-memory statistics");
      PrintStatistics();
    }, m_saveMemoryPeriod);
  }

  void SaveMemory()
  {
    Timer([this]() {
      SC_LOG_INFO("Save memory by period");
      m_context->Save();
    }, m_saveMemoryPeriod);
  }

  virtual void OnOpen(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnClose(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg) = 0;

private:
  std::thread m_updateStatisticsThread;
  std::thread m_saveMemoryThread;

  std::thread m_ioThread;
  std::thread m_actionsThread;
};
