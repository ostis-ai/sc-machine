/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_impl.hpp"

#include "sc_server_action_defines.hpp"

ScServerImpl::ScServerImpl(sc_memory_params const & params)
  : ScServerImpl("127.0.0.1", 8090, "", params)
{
}

ScServerImpl::ScServerImpl(std::string const & host, ScServerPort port, std::string const & logPath, sc_memory_params const & params)
  : ScServer(host, port, logPath, params), m_actionsRun(SC_TRUE), m_actions(new ScServerActions())
{
}

void ScServerImpl::Initialize()
{
  m_instance->set_open_handler(bind(&ScServerImpl::OnOpen, this, ::_1));
  m_instance->set_close_handler(bind(&ScServerImpl::OnClose, this, ::_1));
  m_instance->set_message_handler(bind(&ScServerImpl::OnMessage, this, ::_1, ::_2));

  if (!m_logPath.empty())
  {
    m_log = new std::ofstream();
    m_log->open(m_logPath);
    m_instance->get_alog().set_ostream(m_log);
    m_instance->get_elog().set_ostream(m_log);
  }

  {
    LogMessage(ScServerLogMessages::app, "Sc-server socket data");
    LogMessage(ScServerLogMessages::app, "\tHost name: " + m_hostName);
    LogMessage(ScServerLogMessages::app, "\tPort: " + std::to_string(m_port));
    LogMessage(ScServerLogMessages::app, "\tLogger: " + (m_logPath.empty() ? "console" : "file " + m_logPath));
  }
}

void ScServerImpl::AfterInitialize()
{
  m_actionsRun = SC_FALSE;
  m_actionCond.notify_one();
}

void ScServerImpl::EmitActions()
{
  while (m_actionsRun == SC_TRUE)
  {
    ScServerUniqueLock lock(m_actionLock);

    while (m_actions->empty() && m_actionsRun)
      m_actionCond.wait(lock);

    if (m_actionsRun == SC_FALSE)
      break;

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

ScServerImpl::~ScServerImpl()
{
  delete m_actions;
}
