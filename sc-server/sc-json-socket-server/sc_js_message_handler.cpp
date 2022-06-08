#include "sc_js_message_handler.hpp"

void ScJSMessageHandler::OnMessage(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg)
{
  try
  {
    if (IsEvent(msg->get_payload()))
      OnEvent(server, hdl, msg);
    else
      OnAction(server, hdl, msg);
  }
  catch (ScWSException const & e)
  {
    server->LogError(ScWSServerLogErrors::library, e.m_msg);
  }
  catch (utils::ScException const & e)
  {
    server->LogError(ScWSServerLogErrors::library, e.Description());
  }
}

void ScJSMessageHandler::OnAction(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg)
{
  server->LogMessage(ScWSServerLogMessages::message_payload, "[request] " + msg->get_payload());
  auto const & responseText = server->HandleRequest(msg->get_payload());

  server->LogMessage(ScWSServerLogMessages::message_payload, "[response] " + responseText);
  server->Send(hdl, responseText, ScWSMessageType::text);
}

void ScJSMessageHandler::OnEvent(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg)
{
  server->LogMessage(ScWSServerLogMessages::message_payload, "[event] " + msg->get_payload());
  auto const & responseText = server->HandleEvent(msg->get_payload(), const_cast<ScWSConnectionHandle &>(hdl));

  server->LogMessage(ScWSServerLogMessages::message_payload, "[event response] " + responseText);
  server->Send(hdl, responseText, ScWSMessageType::text);
}

