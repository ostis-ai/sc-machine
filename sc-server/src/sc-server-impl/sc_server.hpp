#pragma once

#include "sc-memory/sc_memory.hpp"

#include "sc_server_defines.hpp"

#include "sc_server_action.hpp"

using ScServerActions = std::queue<ScServerAction *>;

class ScServer
{
public:
  explicit ScServer(sc_memory_params params)
    : ScServer(8090, "", params)
  {
  }

  explicit ScServer(size_t port, sc_memory_params params)
    : ScServer(port, "", params)
  {
  }

  explicit ScServer(size_t port, std::string const & logPath, sc_memory_params params)
  {
    m_port = port;
    m_logPath = logPath;

    ScMemory::Initialize(params);

    m_instance = new ScServerCore();
    m_actions = new ScServerActions();
    m_connections = new ScServerConnections();
  }

  void Run()
  {
    Initialize();

    m_instance->init_asio();
    m_instance->listen(m_port);
    m_instance->start_accept();

    AfterInitialize();

    std::thread t(bind(&ScServer::EmitActions, &*this));

    m_instance->run();

    t.join();
  }

  void Stop()
  {
    m_instance->stop();
  }

  virtual void EmitActions() = 0;

  ScServerConnections * GetConnections()
  {
    return m_connections;
  }

  ScServerActions * GetActions()
  {
    return m_actions;
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
    delete m_instance;
    m_instance = nullptr;

    delete m_actions;
    m_actions = nullptr;
    delete m_connections;
    m_connections = nullptr;

    ScMemory::Shutdown();
  }

protected:
  ScServerPort m_port;
  std::string m_logPath;

  ScServerCore * m_instance;
  ScServerActions * m_actions;
  ScServerConnections * m_connections;

  virtual void Initialize() = 0;

  virtual void AfterInitialize() = 0;

  virtual void OnOpen(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnClose(ScServerConnectionHandle const & hdl) = 0;

  virtual void OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg) = 0;
};
