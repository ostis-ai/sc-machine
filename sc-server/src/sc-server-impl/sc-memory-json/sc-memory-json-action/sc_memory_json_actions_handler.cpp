#include "sc_memory_json_actions_handler.hpp"

std::string ScMemoryJsonActionsHandler::Handle(ScServerConnectionHandle const & hdl, std::string const & requestMessageText)
{
  ScMemoryJsonPayload const & requestMessage = ScMemoryJsonPayload::parse(requestMessageText);

  ScMemoryJsonPayload const & requestType = requestMessage["type"];
  ScMemoryJsonPayload const & requestPayload = requestMessage["payload"];

  sc_bool status = SC_TRUE;
  ScMemoryJsonPayload responsePayload;
  auto const & it = m_actions.find(requestType);
  if (it != m_actions.end())
  {
    auto * action = it->second;
    responsePayload = action->Complete(m_context, requestPayload);
    status = SC_TRUE;
  }
  else
  {
    responsePayload = ScMemoryJsonPayload::parse("Unsupported request type: " + requestType.get<std::string>());
    status = SC_FALSE;
  }

  return GenerateResponseText(requestMessage["id"], status, responsePayload);
}

std::string ScMemoryJsonActionsHandler::GenerateResponseText(
    size_t requestId,
    sc_bool status,
    ScMemoryJsonPayload const & responsePayload)
{
  return ScMemoryJsonPayload({{"id", requestId}, {"event", SC_FALSE}, {"status", status}, {"payload", responsePayload}}).dump();
}