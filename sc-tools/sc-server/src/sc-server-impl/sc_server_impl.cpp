/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_impl.hpp"

#include "sc_server_action_defines.hpp"

extern "C"
{
#include "sc-core/sc-store/sc_storage.h"
#include "sc-core/sc-store/sc_storage_private.h"
#include "sc-core/sc-store/sc-base/sc_thread.h"
}

ScServerImpl::ScServerImpl(
    std::string const & host,
    ScServerPort port,
    sc_bool parallelActions,
    sc_memory_params const & params)
  : ScServer(host, port, params)
  , m_parallelActions(parallelActions)
  , m_actionsRun(SC_TRUE)
  , m_actions(new ScServerActions())
{
}

void ScServerImpl::Initialize()
{
  ScMemoryJsonActionsHandler::InitializeActionClasses();

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

  ScMemoryJsonActionsHandler::ClearActionClasses();
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
  auto session = m_instance->get_con_from_hdl(sessionId);
  std::string const & resourceData = session->get_resource();

  auto * action = new ScServerConnectAction(this, sessionId, resourceData);
  {
    ScServerLock guard(m_actionLock);
    m_actions->push(action);
  }
  m_actionCond.notify_one();
}

void ScServerImpl::OnClose(ScServerSessionId const & sessionId)
{
  auto * action = new ScServerDisconnectAction(this, sessionId);
  {
    ScServerLock guard(m_actionLock);
    m_actions->push(action);
  }
  m_actionCond.notify_one();
}

void ScServerImpl::OnMessage(ScServerSessionId const & sessionId, ScServerMessage const & msg)
{
  auto * action = new ScServerMessageAction(this, sessionId, msg);

  if (m_parallelActions == SC_FALSE)
  {
    {
      ScServerLock guard(m_actionLock);
      m_actions->push(action);
    }
    m_actionCond.notify_one();
  }
  else
  {
    sc_storage_start_new_process();

    action->Emit();

    sc_storage_end_new_process();
  }
}

void ScServerImpl::OnEvent(ScServerSessionId const & sessionId, std::string const & msg)
{
  {
    ScServerLock guard(m_actionLock);
    m_actions->push(new ScServerEventCallbackAction(this, sessionId, msg));
  }
  m_actionCond.notify_one();
}

ScServerImpl::~ScServerImpl()
{
  delete m_actions;
}
