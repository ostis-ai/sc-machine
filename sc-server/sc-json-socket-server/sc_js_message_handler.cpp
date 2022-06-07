#include "sc_js_message_handler.hpp"

void ScJSMessageHandler::OnMessage(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg)
{
  try
  {
    server->LogMessage(ScWSServerLogMessages::message_payload, "[request] " + msg->get_payload());
    auto const & responseText = server->HandleRequest(ScJSPayload::parse(msg->get_payload()));

    server->LogMessage(ScWSServerLogMessages::message_payload, "[response] " + responseText);
    server->Send(hdl, responseText, ScWSMessageType::text);
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
