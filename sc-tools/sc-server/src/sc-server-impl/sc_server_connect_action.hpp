/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc_server_action.hpp"
#include "sc_server.hpp"

class ScServerConnectAction : public ScServerAction
{
public:
  ScServerConnectAction(ScServer * server, ScServerUserProcessId userProcessId)
    : ScServerAction(std::move(userProcessId))
    , m_server(server)
  {
  }

  void Emit() override
  {
    ScAddr const & sessionAddr = m_server->m_context->CreateNode(ScType::NodeConst);
    m_server->m_context->CreateEdge(ScType::EdgeAccessConstPosTemp, ScKeynodes::kUserProcess, sessionAddr);
    m_server->GetConnections()->insert({m_userProcessId, sessionAddr});
  }

  ~ScServerConnectAction() override = default;

protected:
  ScServer * m_server;
};
