/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server.hpp"

using ScServerMutex = std::mutex;
using ScServerUniqueLock = std::unique_lock<ScServerMutex>;
using ScServerLock = std::lock_guard<ScServerMutex>;
using ScServerCondVar = std::condition_variable;

using ScServerActions = std::queue<ScServerAction *>;

class ScServerImpl : public ScServer
{
public:
  explicit ScServerImpl(
      std::string const & host,
      ScServerPort port,
      sc_bool parallelActions,
      sc_memory_params const & params);

  void EmitActions() override;

  sc_bool IsWorkable() override;

  ~ScServerImpl() override;

protected:
  ScServerMutex m_actionLock;
  ScServerMutex m_connectionLock;
  ScServerCondVar m_actionCond;
  sc_bool m_parallelActions;

  std::atomic<sc_bool> m_actionsRun;
  ScServerActions * m_actions;

  void Initialize() override;

  void AfterInitialize() override;

  void OnOpen(ScServerSessionId const & sessionId) override;

  void OnClose(ScServerSessionId const & sessionId) override;

  void OnMessage(ScServerSessionId const & sessionId, ScServerMessage const & msg) override;

  void OnEvent(ScServerSessionId const & sessionId, std::string const & msg) override;
};
