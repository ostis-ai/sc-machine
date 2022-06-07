#pragma once

#include "sc-memory/sc_memory.hpp"

#include "sc_js_actions_handler.hpp"
#include "sc_js_lib.hpp"

class ScJSServer
{
public:
  explicit ScJSServer(size_t port, sc_memory_params params)
    : ScJSServer(port, "", params)
  {
  }

  explicit ScJSServer(size_t port, std::string const & logPath, sc_memory_params params)
    : ScJSServer(params)
  {
    m_port = port;
    m_logPath = logPath;
  }

  ScJSServer(sc_memory_params params)
  {
    m_instance = new ScWSServerCore();
    m_handler = new ScJSActionsHandler();

    ScMemory::Initialize(params);

    Initialize();
  }

  void Run()
  {
    SC_LOG_INFO("\nhost: ");
    SC_LOG_INFO("\nport: " + std::to_string(m_port));
    SC_LOG_INFO("\nlogger: " + (m_logPath.empty() ? "console" : "file " + m_logPath));

    m_instance->listen(m_port);
    m_instance->start_accept();
    m_instance->run();
  }

  void Stop()
  {
    m_instance->stop();
  }

  std::string HandleRequest(std::string const & requestMessage)
  {
    return m_handler->Handle(requestMessage);
  }

  void SetMessageHandler(ScWSMessageHandler handler)
  {
    SC_LOG_INFO("Set \"" + std::string(typeid(handler).name()) + "\" message handler");
    m_instance->set_message_handler(std::move(handler));
  }

  void Send(ScWSConnectionHandle const & hdl, std::string const & message, ScWSMessageType type)
  {
    m_instance->send(hdl, message, type);
  }

  void LogMessage(ScLogChannel channel, std::string const & message)
  {
    m_instance->get_alog().write(channel, message);
  }

  void LogError(ScLogChannel channel, std::string const & errorMessage)
  {
    m_instance->get_elog().write(channel, errorMessage);
  }

  void SetMessageChannels(ScLogChannel channels)
  {
    m_instance->set_access_channels(channels);
  }

  void SetErrorChannels(ScLogChannel channels)
  {
    m_instance->set_access_channels(channels);
  }

  ~ScJSServer()
  {
    delete m_instance;
    delete m_handler;

    ScMemory::Shutdown();
  }

private:
  size_t m_port = 8090;
  std::string m_logPath;

  ScWSServerCore * m_instance;
  ScJSActionsHandler * m_handler;

  void Initialize()
  {
    m_instance->clear_access_channels(ScWSServerLogMessages::all);
    SC_LOG_INFO("Clean channels");
    m_instance->init_asio();

    if (!m_logPath.empty())
    {
      std::ofstream log;
      log.open(m_logPath);
      m_instance->get_alog().set_ostream(&log);
      m_instance->get_elog().set_ostream(&log);
    }
  }
};
