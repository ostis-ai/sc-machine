/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server.hpp"

ScServer::ScServer(std::string hostName, size_t port, sc_memory_params params)
  : m_hostName(std::move(hostName))
  , m_port(port)
  , m_logger(nullptr)
{
  m_memoryState = ScMemory::Initialize(params);
  if (m_memoryState)
    m_context = new ScMemoryContext("sc-server");

  m_instance = new ScServerCore();
  ResetLogger();
  LogMessage(ScServerErrorLevel::info, "Initialize sc-server");

  {
    LogMessage(ScServerErrorLevel::info, "Socket data:");
    LogMessage(ScServerErrorLevel::info, "\tHost name: " + m_hostName);
    LogMessage(ScServerErrorLevel::info, "\tPort: " + std::to_string(m_port));
  }

  m_connections = new ScServerUserProcesses();
  LogMessage(ScServerErrorLevel::info, "Sc-server initialized");
}

void ScServer::Run()
{
  m_instance->init_asio();

  if (!m_memoryState)
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Sc-server sc-memory is invalid");

  m_isServerRun = SC_TRUE;
  m_instance->set_reuse_addr(SC_TRUE);

  Initialize();

  m_instance->listen({boost::asio::ip::address::from_string(m_hostName), sc_uint16(m_port)});
  m_instance->start_accept();

  LogMessage(ScServerErrorLevel::info, "Start actions processing");
  m_actionsThread = std::thread(&ScServer::EmitActions, &*this);

  LogMessage(ScServerErrorLevel::info, "Start input-output processing");
  m_ioThread = std::thread(&ScServerCore::run, &*m_instance);

  LogMessage(ScServerErrorLevel::info, "All inner processes started");
  LogMessage(ScServerErrorLevel::info, "Sc-server run");
}

void ScServer::Stop()
{
  m_isServerRun = SC_FALSE;
  LogMessage(ScServerErrorLevel::info, "Stop sc-server");
  m_instance->stop();

  if (m_instance->is_listening())
    m_instance->stop_listening();

  AfterInitialize();

  if (m_actionsThread.joinable())
  {
    LogMessage(ScServerErrorLevel::info, "Stop actions processing");
    m_actionsThread.join();
  }

  if (m_ioThread.joinable())
  {
    LogMessage(ScServerErrorLevel::info, "Stop input-output processing");

    for (auto & it : *m_connections)
    {
      try
      {
        m_instance->close(it.first, websocketpp::close::status::normal, "Sc-server is finishing work");
      }
      catch (std::exception const & ex)
      {
        LogMessage(ScServerErrorLevel::error, ex.what());
      }
    }

    m_instance->stop();
    m_ioThread.join();
  }

  LogMessage(ScServerErrorLevel::info, "All inner processes stopped");
  LogMessage(ScServerErrorLevel::info, "Sc-server stopped");
}

void ScServer::Shutdown()
{
  LogMessage(ScServerErrorLevel::info, "Shutdown sc-server");

  LogMessage(ScServerErrorLevel::info, "Clear connections");
  delete m_connections;
  m_connections = nullptr;

  LogMessage(ScServerErrorLevel::info, "Sc-server shutdown");

  delete m_instance;
  m_instance = nullptr;

  if (m_memoryState)
    delete m_context;
  ScMemory::Shutdown();
}

std::string ScServer::GetUri()
{
  return "ws://" + m_hostName + ":" + std::to_string(m_port);
}

ScServerUserProcesses * ScServer::GetConnections()
{
  return m_connections;
}

void ScServer::Send(ScServerUserProcessId const & userProcessId, std::string const & message, ScServerMessageType type)
{
  m_instance->send(userProcessId, message, type);
}

void ScServer::SetChannels(ScServerLogLevel channels)
{
  m_instance->set_error_channels(channels);
}

void ScServer::ClearChannels()
{
  m_instance->clear_access_channels(ScServerErrorLevel::all);
  m_instance->clear_error_channels(ScServerErrorLevel::all);
}

void ScServer::ResetLogger(ScServerLogger * logger)
{
  LogMessage(ScServerErrorLevel::info, "Apply logger");
  if (m_logger)
  {
    delete m_logger;
    m_logger = nullptr;
  }

  m_logger = logger;
  if (m_logger)
  {
    m_instance->get_elog().set_ostream(m_logger->GetStream());
  }
  else
  {
    m_instance->get_elog().set_ostream();
  }
}

void ScServer::LogMessage(ScServerLogLevel channel, std::string const & message)
{
  m_instance->get_elog().write(channel, message);
}

void ScServer::CloseConnection(
    ScServerUserProcessId const & userProcessId,
    ScServerCloseCode const code,
    std::string const & reason)
{
  m_instance->close(userProcessId, code, reason);
}

ScServer::~ScServer()
{
  Shutdown();
}
