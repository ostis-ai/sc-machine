#pragma once

#include "sc_server.hpp"

using ScServerMutex = std::mutex;
using ScServerUniqueLock = std::unique_lock<ScServerMutex>;
using ScServerLock = std::lock_guard<ScServerMutex>;
using ScServerCondVar = std::condition_variable;

#define eternal [[noreturn]]

class ScServerImpl : public ScServer
{
public:
  explicit ScServerImpl(ScServerPort port, std::string const & logPath, sc_memory_params params)
      : ScServer(port, logPath, params)
  {
  }

  eternal void EmitActions() override;

protected:
  ScServerMutex m_action_lock;
  ScServerMutex m_connection_lock;
  ScServerCondVar m_action_cond;

  void Initialize() override;

  void AfterInitialize() override;

  void OnOpen(ScServerConnectionHandle const & hdl) override;

  void OnClose(ScServerConnectionHandle const & hdl) override;

  void OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg) override;

  void OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg) override;
};
