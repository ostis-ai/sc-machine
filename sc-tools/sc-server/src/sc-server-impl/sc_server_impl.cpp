/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_impl.hpp"

#include "sc_server_action_defines.hpp"

ScServerImpl::ScServerImpl(std::string const & host, ScServerPort port, sc_bool parallelActions)
  : ScServer(host, port)
  , m_parallelActions(parallelActions)
  , m_actionsRun(SC_TRUE)
  , m_actions(new ScServerActions())
{
  ScMemoryJsonActionsHandler::InitializeActionClasses();
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
    ScServerUniqueLock actionLock(m_actionMutex);

    m_actionCond.wait(
        actionLock,
        [this]
        {
          return !m_actions->empty() || !m_actionsRun;
        });

    if (m_actionsRun == SC_FALSE)
      break;

    ScServerAction * action = m_actions->front();
    m_actions->pop();

    actionLock.unlock();

    try
    {
      action->Emit();
    }
    catch (std::exception const & e)
    {
      LogMessage(ScServerErrorLevel::error, e.what());
    }
    delete action;
  }
}

sc_bool ScServerImpl::IsWorkable()
{
  return m_actions->empty() == SC_FALSE;
}

void ScServerImpl::OnOpen(ScServerSessionId const & sessionId)
{
  auto * action = new ScServerConnectAction(this, sessionId);
  {
    ScServerLock connectionLock(m_connectionMutex);
    ScServerLock actionLock(m_actionMutex);
    m_actions->push(action);
  }
  m_actionCond.notify_one();
}

void ScServerImpl::OnClose(ScServerSessionId const & sessionId)
{
  auto * action = new ScServerDisconnectAction(this, sessionId);
  {
    ScServerLock connectionLock(m_connectionMutex);
    ScServerLock actionLock(m_actionMutex);
    m_actions->push(action);
  }
  m_actionCond.notify_one();
}

void ScServerImpl::OnMessage(ScServerSessionId const & sessionId, ScServerMessage const & msg)
{
  ScServerLock connectionLock(m_connectionMutex);
  if (!IsSessionValid(sessionId))
    return;

  auto * action = new ScServerMessageAction(this, sessionId, msg);

  if (m_parallelActions == SC_FALSE)
  {
    {
      ScServerLock actionLock(m_actionMutex);
      m_actions->push(action);
    }
    m_actionCond.notify_one();
  }
  else
  {
    // sc_storage_start_new_process();

    action->Emit();
    delete action;

    // sc_storage_end_new_process();
  }
}

void ScServerImpl::OnEvent(ScServerSessionId const & sessionId, std::string const & msg)
{
  ScServerLock connectionLock(m_connectionMutex);
  if (!IsSessionValid(sessionId))
    return;

  {
    ScServerLock actionLock(m_actionMutex);
    m_actions->push(new ScServerEventCallbackAction(this, sessionId, msg));
  }
  m_actionCond.notify_one();
}

ScServerImpl::~ScServerImpl()
{
  ScMemoryJsonActionsHandler::ClearActionClasses();
  delete m_actions;
}
