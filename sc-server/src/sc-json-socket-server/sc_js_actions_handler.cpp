#include "sc_js_actions_handler.hpp"

std::string ScJSActionsHandler::Handle(std::string const & requestMessageText)
{
  ScJSPayload const & requestMessage = ScJSPayload::parse(requestMessageText);

  ScJSPayload const & requestType = requestMessage["type"];
  ScJSPayload const & requestPayload = requestMessage["payload"];

  sc_bool status = SC_TRUE;
  ScJSPayload responsePayload;
  auto const & it = m_actions.find(requestType);
  if (it != m_actions.end())
  {
    auto * action = it->second;
    responsePayload = action->Complete(m_context, requestPayload);
    status = SC_TRUE;
  }
  else
  {
    responsePayload = ScJSPayload::parse("Unsupported request type: " + requestType.get<std::string>());
    status = SC_FALSE;
  }

  return GenerateResponseText(requestMessage["id"], status, responsePayload);
}

std::string ScJSActionsHandler::GenerateResponseText(
    size_t requestId,
    sc_bool status,
    ScJSPayload const & responsePayload)
{
  return ScJSPayload({{"id", requestId}, {"event", SC_FALSE}, {"status", status}, {"payload", responsePayload}}).dump();
}