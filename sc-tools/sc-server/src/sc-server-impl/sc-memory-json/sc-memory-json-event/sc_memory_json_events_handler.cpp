/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_json_events_handler.hpp"

template <sc_event_type eventType>
ScEventSubscription * CreateSubscription(
    ScMemoryContext * context,
    ScAddr const & addr,
    ScMemoryJsonEventsManager * manager,
    ScServer * server,
    ScServerSessionId const & sessionId)
{
  auto const & onEmitEvent =
      [server](size_t id, ScServerSessionId const & handle, ScElementaryEvent const & event) -> sc_result
  {
    std::array<ScAddr, 3> const & eventTriple = event.GetTriple();

    ScMemoryJsonPayload const & responsePayload{
        eventTriple.at(0).Hash(), eventTriple.at(1).Hash(), eventTriple.at(2).Hash()};
    ScMemoryJsonPayload const & errorsPayload = ScMemoryJsonPayload::object({});
    sc_bool const isEvent = SC_TRUE;
    sc_bool const status = SC_TRUE;

    ScMemoryJsonPayload const & responseTextJson =
        ScMemoryJsonHandler::FormResponseMessage(id, isEvent, status, errorsPayload, responsePayload);
    std::string const responseText = responseTextJson.dump();

    if (server != nullptr)
      server->OnEvent(handle, responseText);

    return SC_RESULT_OK;
  };

  return new ScEventSubscriptionType<eventType>(
      *context,
      addr,
      (std::function<sc_result(ScEventTypeClass<eventType> const & event)>)bind(
          onEmitEvent, manager->Next(), sessionId, ::_1));
};

std::map<std::string, ScMemoryJsonEventsHandler::ScEventSubscriptionCreateCallback> const
    ScMemoryJsonEventsHandler::events = {
        {"add_outgoing_edge", CreateSubscription<sc_event_add_input_arc>},
        {"add_ingoing_edge", CreateSubscription<sc_event_add_output_arc>},
        {"remove_outgoing_edge", CreateSubscription<sc_event_remove_input_arc>},
        {"remove_ingoing_edge", CreateSubscription<sc_event_remove_output_arc>},
        {"content_change", CreateSubscription<sc_event_remove_element>},
        {"delete_element", CreateSubscription<sc_event_change_content>},
};

ScMemoryJsonEventsHandler::ScMemoryJsonEventsHandler(ScServer * server, ScMemoryContext * processCtx)
  : ScMemoryJsonHandler(server)
  , m_context(processCtx)
  , m_manager(ScMemoryJsonEventsManager::GetInstance())
{
}

ScMemoryJsonEventsHandler::~ScMemoryJsonEventsHandler() = default;

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleRequestPayload(
    ScServerSessionId const & sessionId,
    std::string const &,
    ScMemoryJsonPayload const & requestPayload,
    ScMemoryJsonPayload & errorsPayload,
    sc_bool & status,
    sc_bool & isEvent)
{
  status = SC_FALSE;
  isEvent = SC_FALSE;

  errorsPayload = ScMemoryJsonPayload::array({});

  ScMemoryJsonPayload responsePayload;
  if (requestPayload.find("create") != requestPayload.cend())
    responsePayload = HandleCreate(sessionId, requestPayload["create"], errorsPayload);
  else if (requestPayload.find("delete") != requestPayload.cend())
    responsePayload = HandleDelete(sessionId, requestPayload["delete"], errorsPayload);
  else
    errorsPayload = "Unknown event request";

  status = errorsPayload.empty();

  return responsePayload;
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleCreate(
    ScServerSessionId const & sessionId,
    ScMemoryJsonPayload const & message,
    ScMemoryJsonPayload &)
{
  ScMemoryJsonPayload responsePayload;
  for (auto & atom : message)
  {
    std::string const & type = atom["type"];
    ScAddr const & addr = ScAddr(atom["addr"].get<size_t>());

    auto const & it = events.find(type);
    if (it != events.end())
    {
      ScEventSubscription * subscription = it->second(m_context, addr, m_manager, m_server, sessionId);
      responsePayload.push_back(m_manager->Add(subscription));
    }
  }

  return responsePayload;
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleDelete(
    ScServerSessionId const &,
    ScMemoryJsonPayload const & message,
    ScMemoryJsonPayload &)
{
  for (auto & atom : message)
    delete m_manager->Remove(atom.get<size_t>());

  return message;
}
