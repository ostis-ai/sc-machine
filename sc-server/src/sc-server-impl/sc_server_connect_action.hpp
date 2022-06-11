#pragma once

#include <utility>

#include "sc_server_action.hpp"
#include "sc_server.hpp"

class ScServerConnectAction : public ScServerAction
{
public:
  ScServerConnectAction(ScServer * server, ScServerConnectionHandle hdl)
      : ScServerAction(std::move(hdl)), m_server(server)
  {
  }

  void Emit() override
  {
    m_server->GetConnections()->insert(m_hdl);
  }

  ~ScServerConnectAction() override = default;

protected:
  ScServer * m_server;
};
