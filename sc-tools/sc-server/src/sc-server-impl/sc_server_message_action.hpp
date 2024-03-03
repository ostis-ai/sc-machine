/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc-memory/sc_keynodes.hpp"

#include "sc_server_action.hpp"
#include "sc_server.hpp"
#include "sc-memory-json/sc_memory_json_payload.hpp"
#include "sc-memory-json/sc_memory_json_handler.hpp"
#include "sc-memory-json/sc-memory-json-action/sc_memory_json_actions_handler.hpp"
#include "sc-memory-json/sc-memory-json-event/sc_memory_json_events_handler.hpp"

class ScServerMessageAction : public ScServerAction
{
public:
  ScServerMessageAction(ScServer * server, ScServerSessionId const & sessionId, ScServerMessage msg)
    : ScServerAction(sessionId)
    , m_server(server)
    , m_msg(std::move(msg))
  {
    ScMemoryContext * sessionCtx = m_server->GetSessionContext(sessionId);
    m_actionsHandler = new ScMemoryJsonActionsHandler(server, sessionCtx);
    m_eventsHandler = new ScMemoryJsonEventsHandler(server, sessionCtx);
  }

  void HandleEmit()
  {
    std::string const & messageType = GetMessageType(m_msg);

    if (IsHealthCheck(messageType))
      OnHealthCheck(m_sessionId, m_msg);
    else if (IsConnectionInfo(messageType))
      OnConnectionInfo(m_sessionId, m_msg);
    else if (IsEvent(messageType))
      OnEvent(m_sessionId, m_msg);
    else
      OnAction(m_sessionId, m_msg);
  }

  void Emit() override
  {
    try
    {
      HandleEmit();
    }
    catch (ScServerException const & e)
    {
      m_server->LogMessage(ScServerErrorLevel::error, e.m_msg);
    }
    catch (utils::ScException const & e)
    {
      m_server->LogMessage(ScServerErrorLevel::error, e.Description());
    }
  }

  void OnAction(ScServerSessionId const & sessionId, ScServerMessage const & msg)
  {
    m_server->LogMessage(ScServerErrorLevel::debug, "[request] " + msg->get_payload());
    auto const & responseText = m_actionsHandler->Handle(sessionId, msg->get_payload());

    m_server->LogMessage(ScServerErrorLevel::debug, "[response] " + responseText);
    m_server->Send(sessionId, responseText, ScServerMessageType::text);
  }

  void OnEvent(ScServerSessionId const & sessionId, ScServerMessage const & msg)
  {
    m_server->LogMessage(ScServerErrorLevel::debug, "[event] " + msg->get_payload());
    auto const & responseText = m_eventsHandler->Handle(sessionId, msg->get_payload());

    m_server->LogMessage(ScServerErrorLevel::debug, "[event response] " + responseText);
    m_server->Send(sessionId, responseText, ScServerMessageType::text);
  }

  void OnHealthCheck(ScServerSessionId const & sessionId, ScServerMessage const & msg)
  {
    SC_UNUSED(msg);

    ScMemoryJsonPayload response;
    try
    {
      auto * context = new ScMemoryContext(ScKeynodes::kMySelf);

      ScAddr const & tempAddr = context->CreateNode(ScType::NodeConst);
      context->EraseElement(tempAddr);

      delete context;

      response = "OK";
      m_server->LogMessage(ScServerErrorLevel::info, "I'm alive...");
    }
    catch (utils::ScException const & e)
    {
      SC_LOG_ERROR(e.Description());
      response = "NO";
      m_server->LogMessage(ScServerErrorLevel::info, "I've died...");
    }

    m_server->Send(sessionId, response.dump(), ScServerMessageType::text);
    m_server->CloseConnection(sessionId, websocketpp::close::status::normal, "Status checked");
  }

  void OnConnectionInfo(ScServerSessionId const & sessionId, ScServerMessage const & msg)
  {
    SC_UNUSED(msg);

    ScAddr const & userAddr = m_server->GetSessionContext(sessionId)->GetUserAddr();
    ScMemoryJsonPayload response{{"connection_id", (sc_uint64)sessionId.lock().get()}, {"user_addr", userAddr.Hash()}};

    m_server->Send(sessionId, response.dump(), ScServerMessageType::text);
  }

  ~ScServerMessageAction() override
  {
    delete m_actionsHandler;
    delete m_eventsHandler;
  };

protected:
  ScServer * m_server;
  ScServerMessage m_msg;

  ScMemoryJsonHandler * m_actionsHandler;
  ScMemoryJsonHandler * m_eventsHandler;

  static std::string GetMessageType(ScServerMessage const & msg)
  {
    if (ScMemoryJsonPayload::accept(msg->get_payload()))
    {
      ScMemoryJsonPayload const & payload = ScMemoryJsonPayload::parse(msg->get_payload());
      return payload.contains("type") ? payload["type"].get<std::string>() : "";
    }

    return "";
  }

  static sc_bool IsEvent(std::string const & messageType)
  {
    return messageType == "events";
  }

  static sc_bool IsHealthCheck(std::string const & messageType)
  {
    return messageType == "healthcheck";
  }

  static sc_bool IsConnectionInfo(std::string const & messageType)
  {
    return messageType == "connection_info";
  }
};
