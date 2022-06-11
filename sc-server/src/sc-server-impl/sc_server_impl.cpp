#include "sc_server_impl.hpp"

#include "sc_server_action_defines.hpp"

#define forever while (SC_TRUE)

void ScServerImpl::Initialize()
{
  m_instance->set_open_handler(bind(&ScServerImpl::OnOpen, this, ::_1));
  m_instance->set_close_handler(bind(&ScServerImpl::OnClose, this, ::_1));
  m_instance->set_message_handler(bind(&ScServerImpl::OnMessage, this, ::_1, ::_2));

  SC_LOG_INFO("[sc-server] Clear channels");
  m_instance->clear_access_channels(ScServerLogMessages::all);
  m_instance->clear_error_channels(ScServerLogErrors::all);

  if (!m_logPath.empty())
  {
    auto * log = new std::ofstream();
    log->open(m_logPath);
    m_instance->get_alog().set_ostream(log);
    m_instance->get_elog().set_ostream(log);
  }

  SC_LOG_INFO("[sc-server] Socket data");
  SC_LOG_INFO("\thost: ");
  SC_LOG_INFO("\tport: " + std::to_string(m_port));
  SC_LOG_INFO("\tlogger: " + (m_logPath.empty() ? "console" : "file " + m_logPath));
}

void ScServerImpl::AfterInitialize()
{
  SC_LOG_INFO("[sc-server] Connection opened");
}

eternal void ScServerImpl::EmitActions()
{
  forever
  {
    ScServerUniqueLock lock(m_action_lock);

    while (m_actions->empty())
      m_action_cond.wait(lock);

    ScServerAction * action = m_actions->front();
    m_actions->pop();

    lock.unlock();

    ScServerLock guard(m_connection_lock);

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
    ScServerLock guard(m_action_lock);
    m_actions->push(new ScServerConnectAction(this, hdl));
  }
  m_action_cond.notify_one();
}

void ScServerImpl::OnClose(ScServerConnectionHandle const & hdl)
{
  {
    ScServerLock guard(m_action_lock);
    m_actions->push(new ScServerDisconnectAction(this, hdl));
  }
  m_action_cond.notify_one();
}

void ScServerImpl::OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg)
{
  {
    ScServerLock guard(m_action_lock);
    m_actions->push(new ScServerMessageAction(this, hdl, msg));
  }
  m_action_cond.notify_one();
}

void ScServerImpl::OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg)
{
  {
    ScServerLock guard(m_action_lock);
    m_actions->push(new ScServerEventCallbackAction(this, hdl, msg));
  }
  m_action_cond.notify_one();
}
