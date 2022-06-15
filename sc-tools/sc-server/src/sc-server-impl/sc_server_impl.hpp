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
  explicit ScServerImpl(sc_memory_params const & params);

  explicit ScServerImpl(std::string const & host, ScServerPort port, std::string const & logPath, sc_memory_params const & params);

  eternal void EmitActions() override;

protected:
  ScServerMutex m_actionLock;
  ScServerMutex m_connectionLock;
  ScServerCondVar m_actionCond;

  void Initialize() override;

  void AfterInitialize() override;

  void OnOpen(ScServerConnectionHandle const & hdl) override;

  void OnClose(ScServerConnectionHandle const & hdl) override;

  void OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg) override;

  void OnEvent(ScServerConnectionHandle const & hdl, std::string const & msg) override;
};
