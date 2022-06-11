#pragma once

#include "sc-memory/sc_memory.hpp"

#include "sc_js_lib.hpp"
#include "sc_js_actions_handler.hpp"
#include "sc_js_events_handler.hpp"

class ScJSServer
{
public:
  explicit ScJSServer(sc_memory_params params)
    : ScJSServer(8090, "", params)
  {
  }

  explicit ScJSServer(size_t port, sc_memory_params params)
    : ScJSServer(port, "", params)
  {
  }

  explicit ScJSServer(size_t port, std::string const & logPath, sc_memory_params params)
  {
    m_port = port;
    m_logPath = logPath;

    ScMemory::Initialize(params);

    m_instance = new ScWSServerCore();
    m_connections = new ScWSServerConnections();

    m_handler = new ScJSActionsHandler();
    m_events_handler = new ScJSEventsHandler(m_instance, m_connections);

    Initialize();
  }

  void Run()
  {
    SC_LOG_INFO("[sc-server] Socket data");
    SC_LOG_INFO("\thost: ");
    SC_LOG_INFO("\tport: " + std::to_string(m_port));
    SC_LOG_INFO("\tlogger: " + (m_logPath.empty() ? "console" : "file " + m_logPath));

    m_instance->listen(m_port);
    m_instance->start_accept();
    SC_LOG_INFO("[sc-server] Connection opened");
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

  std::string HandleEvent(std::string const & requestMessage, ScWSConnectionHandle & hdl)
  {
    m_events_handler->SetConnection(&hdl);

    if (m_events_handler->IsConnectionValid())
      return m_events_handler->Handle(requestMessage);

    return "";
  }

  void SetMessageHandler(ScWSMessageHandler handler)
  {
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
    delete m_events_handler;

    ScMemory::Shutdown();
  }

private:
  size_t m_port;
  std::string m_logPath;

  ScWSServerCore * m_instance;
  ScWSServerConnections * m_connections;

  ScJSActionsHandler * m_handler;
  ScJSEventsHandler * m_events_handler;

  void Initialize()
  {
    SC_LOG_INFO("[sc-server] Clear channels");
    m_instance->clear_access_channels(ScWSServerLogMessages::all);
    m_instance->clear_error_channels(ScWSServerLogErrors::all);
    m_instance->init_asio();

    if (!m_logPath.empty())
    {
      auto * log = new std::ofstream();
      log->open(m_logPath);
      m_instance->get_alog().set_ostream(log);
      m_instance->get_elog().set_ostream(log);
    }

    m_instance->set_open_handler(bind([this](ScWSConnectionHandle const & hdl){m_connections->insert(hdl);}, ::_1));
    m_instance->set_close_handler(bind([this](ScWSConnectionHandle const & hdl){m_connections->erase(hdl);}, ::_1));
  }
};
