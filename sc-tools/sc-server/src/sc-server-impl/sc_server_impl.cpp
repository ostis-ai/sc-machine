/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_impl.hpp"

#include "sc_server_action_defines.hpp"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8090

#define DEFAULT_LOG_TYPE "Console"
#define DEFAULT_LOG_FILE ""
#define DEFAULT_LOG_LEVEL "Debug"

ScServerImpl::ScServerImpl(sc_memory_params const & params)
  : ScServerImpl(DEFAULT_HOST, DEFAULT_PORT, DEFAULT_LOG_TYPE, DEFAULT_LOG_FILE, DEFAULT_LOG_LEVEL, SC_FALSE, params)
{
}

ScServerImpl::ScServerImpl(
    std::string const & host,
    ScServerPort port,
    std::string const & logType,
    std::string const & logFile,
    std::string const & logLevel,
    sc_bool syncActions,
    sc_memory_params const & params)
  : ScServer(host, port, logType, logFile, logLevel, params),
  m_syncActions(syncActions),
  m_actionsRun(SC_TRUE),
  m_actions(new ScServerActions())
{
}

void ScServerImpl::Initialize()
{
  m_instance->set_open_handler(bind(&ScServerImpl::OnOpen, this, ::_1));
  m_instance->set_close_handler(bind(&ScServerImpl::OnClose, this, ::_1));
  m_instance->set_message_handler(bind(&ScServerImpl::OnMessage, this, ::_1, ::_2));
}

void ScServerImpl::AfterInitialize()
{
  while (m_actions->empty() == SC_FALSE)
    ;

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

sc_bool ScServerImpl::IsWorkable()
{
  return m_actions->empty() == SC_FALSE;
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
  if (m_syncActions == SC_TRUE)
  {
    {
      ScServerLock guard(m_actionLock);
      m_actions->push(new ScServerMessageAction(this, hdl, msg));
    }
    m_actionCond.notify_one();
  }
  else
    ScServerMessageAction(this, hdl, msg).Emit();
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
