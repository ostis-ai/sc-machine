#pragma once

#include <utility>

#include "sc_server_action.hpp"
#include "sc_server.hpp"

class ScServerDisconnectAction : public ScServerAction
{
public:
  ScServerDisconnectAction(ScServer * server, ScServerConnectionHandle hdl)
      : m_server(server), m_hdl(std::move(hdl))
  {
  }

  void Emit() override
  {
    m_server->GetConnections()->erase(m_hdl);
  }

  ~ScServerDisconnectAction() override = default;

protected:
  ScServer * m_server;
  ScServerConnectionHandle m_hdl;
};
