#include "sc_js_events_handler.hpp"

ScJSEventsHandler::ScJSEventsHandler()
{
  m_context = new ScMemoryContext("sc-json-socket-events-handler");
  m_manager = ScJSEventsManager::GetInstance();

  m_server = nullptr;
  m_hdl = nullptr;
}

std::string ScJSEventsHandler::Handle(std::string const & requestMessageText)
{
  ScJSPayload const & requestMessage = ScJSPayload::parse(requestMessageText);
  ScJSPayload requestPayload = requestMessage["payload"];

  ScJSPayload responsePayload;
  if (requestPayload["create"].is_null() == SC_FALSE)
    responsePayload = HandleCreate(requestPayload["create"]);
  else if (requestPayload["delete"].is_null() == SC_FALSE)
    responsePayload = HandleDelete(requestPayload["delete"]);

  return GenerateResponseText(requestMessage["id"], SC_FALSE, SC_TRUE, responsePayload);
}

ScJSPayload ScJSEventsHandler::HandleCreate(ScJSPayload const & message)
{
  auto const onEmitEvent = [this](
                               size_t id,
                               ScWSConnectionHandle * handle,
                               ScAddr const & addr,
                               ScAddr const & edgeAddr,
                               ScAddr const & otherAddr) -> sc_bool {
    ScJSPayload responsePayload = ScJSPayload({addr.Hash(), edgeAddr.Hash(), otherAddr.Hash()});
    std::string responseText = GenerateResponseText(id, SC_TRUE, SC_TRUE, responsePayload);

    try
    {
      m_server->send(*handle, responseText, ScWSMessageType::text);
    }
    catch (ScWSException const & e)
    {
      m_server->get_elog().write(ScWSServerLogErrors::warn, "Connection loosed " + std::string(e.what()));
    }

    return SC_TRUE;
  };

  ScJSPayload responsePayload;
  for (auto & atom : message)
  {
    std::string const & type = atom["type"];
    ScAddr const & addr = ScAddr(atom["addr"].get<size_t>());

    auto const & it = events.find(type);
    if (it != events.end())
    {
      auto * event =
          new ScEvent(*m_context, addr, it->second, bind(onEmitEvent, m_manager->Next(), m_hdl, ::_1, ::_2, ::_3));
      responsePayload.push_back(m_manager->Add(event));
    }
  }

  return responsePayload;
}

ScJSPayload ScJSEventsHandler::HandleDelete(ScJSPayload const & message)
{
  for (auto & atom : message)
    delete m_manager->Remove(atom.get<size_t>());

  return message;
}

std::string ScJSEventsHandler::GenerateResponseText(
    size_t requestId,
    sc_bool event,
    sc_bool status,
    ScJSPayload const & responsePayload)
{
  return ScJSPayload({{"id", requestId}, {"event", event}, {"status", status}, {"payload", responsePayload}}).dump();
}
