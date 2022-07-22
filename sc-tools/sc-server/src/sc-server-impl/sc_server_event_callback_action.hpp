/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc_server_action.hpp"
#include "sc_server.hpp"
#include "sc-memory-json/sc_memory_json_payload.hpp"

class ScServerEventCallbackAction : public ScServerAction
{
public:
  ScServerEventCallbackAction(ScServer * server, ScServerConnectionHandle hdl, std::string msg)
    : ScServerAction(std::move(hdl))
    , m_server(server)
    , m_msg(std::move(msg))
  {
  }

  void Emit() override
  {
    if (m_server != nullptr)
      m_server->Send(m_hdl, m_msg, ScServerMessageType::text);
  }

  ~ScServerEventCallbackAction() override = default;

protected:
  ScServer * m_server;
  std::string m_msg;
};
