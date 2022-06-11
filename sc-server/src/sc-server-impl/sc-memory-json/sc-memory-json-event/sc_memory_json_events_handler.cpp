#include "sc_memory_json_events_handler.hpp"

ScMemoryJsonEventsHandler::ScMemoryJsonEventsHandler(ScServer * server)
{
  m_context = new ScMemoryContext("sc-json-socket-events-handler");
  m_manager = ScMemoryJsonEventsManager::GetInstance();

  m_server = server;
  m_connections = m_server->GetConnections();
}

std::string ScMemoryJsonEventsHandler::Handle(ScServerConnectionHandle const & hdl, std::string const & requestMessageText)
{
  ScMemoryJsonPayload const & requestMessage = ScMemoryJsonPayload::parse(requestMessageText);
  ScMemoryJsonPayload requestPayload = requestMessage["payload"];

  ScMemoryJsonPayload responsePayload;
  if (requestPayload["create"].is_null() == SC_FALSE)
    responsePayload = HandleCreate(hdl, requestPayload["create"]);
  else if (requestPayload["delete"].is_null() == SC_FALSE)
    responsePayload = HandleDelete(hdl, requestPayload["delete"]);

  return GenerateResponseText(requestMessage["id"], SC_FALSE, SC_TRUE, responsePayload);
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleCreate(ScServerConnectionHandle const & hdl, ScMemoryJsonPayload const & message)
{
  auto const onEmitEvent = [this](
                               size_t id,
                               ScServerConnectionHandle const & handle,
                               ScAddr const & addr,
                               ScAddr const & edgeAddr,
                               ScAddr const & otherAddr) -> sc_bool {
    ScMemoryJsonPayload responsePayload = ScMemoryJsonPayload({addr.Hash(), edgeAddr.Hash(), otherAddr.Hash()});
    std::string responseText = GenerateResponseText(id, SC_TRUE, SC_TRUE, responsePayload);

    try
    {
      if (m_server != nullptr && !m_connections->empty() && m_connections->find(handle) != m_connections->end())
         m_server->Send(handle, responseText, ScServerMessageType::text);
    }
    catch (ScServerException const & e)
    {
      m_server->LogError(ScServerLogErrors::warn, "Connection loosed " + std::string(e.what()));
    }

    return SC_TRUE;
  };

  ScMemoryJsonPayload responsePayload;
  for (auto & atom : message)
  {
    std::string const & type = atom["type"];
    ScAddr const & addr = ScAddr(atom["addr"].get<size_t>());

    auto const & it = events.find(type);
    if (it != events.end())
    {
      auto * event =
          new ScEvent(*m_context, addr, it->second, bind(onEmitEvent, m_manager->Next(), hdl, ::_1, ::_2, ::_3));
      responsePayload.push_back(m_manager->Add(event));
    }
  }

  return responsePayload;
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleDelete(ScServerConnectionHandle const & hdl, ScMemoryJsonPayload const & message)
{
  SC_UNUSED(hdl);

  for (auto & atom : message)
    delete m_manager->Remove(atom.get<size_t>());

  return message;
}

std::string ScMemoryJsonEventsHandler::GenerateResponseText(
    size_t requestId,
    sc_bool event,
    sc_bool status,
    ScMemoryJsonPayload const & responsePayload)
{
  return ScMemoryJsonPayload({{"id", requestId}, {"event", event}, {"status", status}, {"payload", responsePayload}}).dump();
}
