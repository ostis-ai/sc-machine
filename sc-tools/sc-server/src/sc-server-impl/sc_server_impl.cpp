#include "sc_server_impl.hpp"

#include "sc_server_action_defines.hpp"

#define forever while (SC_TRUE)

ScServerImpl::ScServerImpl(sc_memory_params const & params)
  : ScServer(params)
{
}

ScServerImpl::ScServerImpl(std::string const & host, ScServerPort port, std::string const & logPath, sc_memory_params const & params)
  : ScServer(host, port, logPath, params)
{
}

void ScServerImpl::Initialize()
{
  m_instance->set_open_handler(bind(&ScServerImpl::OnOpen, this, ::_1));
  m_instance->set_close_handler(bind(&ScServerImpl::OnClose, this, ::_1));
  m_instance->set_message_handler(bind(&ScServerImpl::OnMessage, this, ::_1, ::_2));

  m_instance->clear_access_channels(ScServerLogMessages::all);
  m_instance->clear_error_channels(ScServerLogErrors::all);
  LogMessage(ScServerLogMessages::app, "Clear channels");

  if (!m_logPath.empty())
  {
    m_log = new std::ofstream();
    m_log->open(m_logPath);
    m_instance->get_alog().set_ostream(m_log);
    m_instance->get_elog().set_ostream(m_log);
  }

  LogMessage(ScServerLogMessages::app, "Sc-server socket data");
  LogMessage(ScServerLogMessages::app, "\tHost name: ");
  LogMessage(ScServerLogMessages::app, "\tPort: " + std::to_string(m_port));
  LogMessage(ScServerLogMessages::app, "\tLogger: " + (m_logPath.empty() ? "console" : "file " + m_logPath));
}

void ScServerImpl::AfterInitialize()
{
  LogMessage(ScServerLogMessages::app, "Connection opened");
}

eternal void ScServerImpl::EmitActions()
{
  forever
  {
    ScServerUniqueLock lock(m_actionLock);

    while (m_actions->empty())
      m_actionCond.wait(lock);

    ScServerAction * action = m_actions->front();
    m_actions->pop();

    lock.unlock();

    ScServerLock guard(m_connectionLock);

    try
    {
      action->Emit();
    }
    catch (std::exception const & e)
    {
      LogError(ScServerLogErrors::devel, e.what());
    }
    delete action;
  }
}

void ScServerImpl::OnOpen(ScServerConnectionHandle const & hdl)
{
  {
    ScServerLock guard(m_actionLock);
    m_actions->push(new ScServerConnectAction(this, hdl));
  }
  m_actionCond.notify_one();
}

void ScServerImpl::OnClose(ScServerConnectionHandle const & hdl)
{
  {
    ScServerLock guard(m_actionLock);
    m_actions->push(new ScServerDisconnectAction(this, hdl));
  }
  m_actionCond.notify_one();
}

void ScServerImpl::OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg)
{
  {
    ScServerLock guard(m_actionLock);
    m_actions->push(new ScServerMessageAction(this, hdl, msg));
  }
  m_actionCond.notify_one();
}

void ScServerImpl::OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg)
{
  {
    ScServerLock guard(m_actionLock);
    m_actions->push(new ScServerEventCallbackAction(this, hdl, msg));
  }
  m_actionCond.notify_one();
}
