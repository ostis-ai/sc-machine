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

class ScServer
{
public:
  explicit ScServer(std::string hostName, size_t port, sc_memory_params params)
    : ScServer(std::move(hostName), port, "", params)
  {
  }

  explicit ScServer(std::string hostName, size_t port, std::string logPath, sc_memory_params params)
    : m_hostName(std::move(hostName)), m_port(port), m_logPath(std::move(logPath))
  {
    ScMemory::Initialize(params);

    m_instance = new ScServerCore();
    m_connections = new ScServerConnections();

    m_instance->clear_access_channels(ScServerLogMessages::all);
    m_instance->clear_error_channels(ScServerLogErrors::all);
  }

  void Run()
  {
    m_instance->init_asio();
    m_instance->set_reuse_addr(SC_TRUE);

    Initialize();

    m_instance->listen({boost::asio::ip::address::from_string(m_hostName), sc_uint16(m_port)});
    m_instance->start_accept();

    LogMessage(ScServerLogMessages::devel, "Start actions processing");
    m_actionsThread = std::thread(&ScServer::EmitActions, &*this);

    LogMessage(ScServerLogMessages::devel, "Start input-output processing");
    m_ioThread = std::thread(&ScServerCore::run, &*m_instance);
  }

  void Stop()
  {
    m_instance->stop_listening();

    AfterInitialize();

    if (m_actionsThread.joinable())
    {
      LogMessage(ScServerLogMessages::devel, "Close actions processing");
      m_actionsThread.join();
    }

    if (m_ioThread.joinable())
    {
      LogMessage(ScServerLogMessages::devel, "Close input-output processing");

      for (auto & it : *m_connections)
      {
        try
        {
          m_instance->close(it, websocketpp::close::status::normal, "");
        }
        catch (std::exception const & ex)
        {
          LogError(ScServerLogErrors::devel, ex.what());
        }
      }

      m_instance->stop();
      m_ioThread.join();
    }
  }

  void Shutdown()
  {
    LogMessage(ScServerLogMessages::devel, "Shutdown sc-server");

    delete m_instance;
    m_instance = nullptr;

    delete m_connections;
    m_connections = nullptr;

    delete m_log;
    m_log = nullptr;

    ScMemory::Shutdown();
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

  void SetMessageChannels(ScServerLogChannel channels)
  {
    m_instance->set_access_channels(channels);
  }

  void SetErrorChannels(ScServerLogChannel channels)
  {
    m_instance->set_access_channels(channels);
  }

  virtual void OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg) = 0;

  virtual ~ScServer()
  {
    Shutdown();
  }

protected:
  std::string m_hostName;
  ScServerPort m_port;
  std::string m_logPath;

  std::ofstream * m_log{};

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
