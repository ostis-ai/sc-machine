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

  sc_bool CheckIfUserProcessAuthorized(ScServerUserProcessId const & userProcessId) override;

  ~ScServerImpl() override;

protected:
  ScServerMutex m_actionLock;
  ScServerMutex m_connectionLock;
  ScServerCondVar m_actionCond;
  sc_bool m_parallelActions;

  std::atomic<sc_bool> m_actionsRun;
  ScServerActions * m_actions;

  ScEvent * m_authorizeUserProcessSubscription;
  ScEvent * m_unauthorizeUserProcessSubscription;
  std::set<ScAddr, ScAddrLessFunc> m_authorizedUserProcesses;

  void Initialize() override;

  void AfterInitialize() override;

  void OnOpen(ScServerUserProcessId const & userProcessId) override;

  void OnClose(ScServerUserProcessId const & userProcessId) override;

  void OnMessage(ScServerUserProcessId const & userProcessId, ScServerMessage const & msg) override;

  void OnEvent(ScServerUserProcessId const & userProcessId, std::string const & msg) override;
};
