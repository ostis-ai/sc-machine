/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_json_events_handler.hpp"

std::unordered_map<std::string, std::string> const
    ScMemoryJsonEventsHandler::m_deprecatedEventsIdtfsToSystemEventsIdtfs = {
        {"add_ingoing_edge", "sc_event_after_generate_incoming_arc"},
        {"add_outgoing_edge", "sc_event_after_generate_outgoing_arc"},
        {"remove_ingoing_edge", "sc_event_before_erase_incoming_arc"},
        {"remove_outgoing_edge", "sc_event_before_erase_outgoing_arc"},
        {"delete_element", "sc_event_before_erase_element"},
        {"content_change", "sc_event_before_change_link_content"},
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
    errorsPayload = "Unknown sc-event request.";

  status = errorsPayload.empty();

  return responsePayload;
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleCreate(
    ScServerSessionId const & sessionId,
    ScMemoryJsonPayload const & message,
    ScMemoryJsonPayload & errorsPayload)
{
  auto const & onEmitEvent =
      [](ScServer * server, size_t id, ScServerSessionId const & handle, ScElementaryEvent const & event)
  {
    auto const & [sourceAddr, connectorAddr, targetAddr] = event.GetTriple();

    ScMemoryJsonPayload const & responsePayload{sourceAddr.Hash(), connectorAddr.Hash(), targetAddr.Hash()};
    ScMemoryJsonPayload const & errorsPayload = ScMemoryJsonPayload::object({});
    sc_bool const isEvent = SC_TRUE;
    sc_bool const status = SC_TRUE;

    ScMemoryJsonPayload const & responseTextJson =
        ScMemoryJsonHandler::FormResponseMessage(id, isEvent, status, errorsPayload, responsePayload);
    std::string const responseText = responseTextJson.dump();

    if (server != nullptr)
      server->OnEvent(handle, responseText);
  };

  ScMemoryJsonPayload responsePayload;
  for (auto & atom : message)
  {
    std::string eventClass = atom["type"];
    ScAddr const & subscriptionElementAddr = ScAddr(atom["addr"].get<size_t>());

    auto const & it = m_deprecatedEventsIdtfsToSystemEventsIdtfs.find(eventClass);
    if (it == m_deprecatedEventsIdtfsToSystemEventsIdtfs.cend())
    {
      errorsPayload = "Unknown sc-event type with system identifier `" + eventClass + "`.";
      return responsePayload;
    }
    eventClass = it->second;

    ScAddr const & eventClassAddr = m_context->SearchElementBySystemIdentifier(eventClass);
    if (!eventClassAddr.IsValid())
    {
      errorsPayload = "Invalid sc-event type with system identifier `" + eventClass + "`.";
      return responsePayload;
    }

    ScEventSubscription * subscription = new ScElementaryEventSubscription<>(
        *m_context,
        eventClassAddr,
        subscriptionElementAddr,
        bind(onEmitEvent, m_server, m_manager->Next(), sessionId, ::_1));
    responsePayload.push_back(m_manager->Add(subscription));
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
