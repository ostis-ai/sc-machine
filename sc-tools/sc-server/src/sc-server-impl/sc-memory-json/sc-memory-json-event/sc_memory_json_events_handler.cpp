/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_json_events_handler.hpp"

ScMemoryJsonEventsHandler::ScMemoryJsonEventsHandler(ScServer * server)
  : ScMemoryJsonHandler(server)
{
  m_context = new ScMemoryContext("sc-json-socket-events-handler");
  m_manager = ScMemoryJsonEventsManager::GetInstance();
}

ScMemoryJsonEventsHandler::~ScMemoryJsonEventsHandler()
{
  m_context->Destroy();
  delete m_context;
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleRequestPayload(
    ScServerConnectionHandle const & hdl,
    std::string const & requestType,
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
    responsePayload = HandleCreate(hdl, requestPayload["create"], errorsPayload);
  else if (requestPayload.find("delete") != requestPayload.cend())
    responsePayload = HandleDelete(hdl, requestPayload["delete"], errorsPayload);
  else
    errorsPayload = "Unknown event request";

  status = errorsPayload.empty();

  return responsePayload;
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleCreate(
    ScServerConnectionHandle const & hdl,
    ScMemoryJsonPayload const & message,
    ScMemoryJsonPayload & errorsPayload)
{
  auto const & onEmitEvent = [](size_t id,
                              ScServer * server,
                              ScServerConnectionHandle const & handle,
                              ScAddr const & addr,
                              ScAddr const & edgeAddr,
                              ScAddr const & otherAddr) -> sc_bool {
    ScMemoryJsonPayload const & responsePayload = {addr.Hash(), edgeAddr.Hash(), otherAddr.Hash()};
    ScMemoryJsonPayload const & errorsPayload = ScMemoryJsonPayload::object({});
    sc_bool const event = SC_TRUE;
    sc_bool const status = SC_TRUE;

    ScMemoryJsonPayload const & responseTextJson
        = FormResponseMessage(id, event, status, errorsPayload, responsePayload);
    std::string const responseText = responseTextJson.dump();

    if (server != nullptr)
      server->OnEvent(handle, responseText);

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
      auto * event = new ScEvent(
          *m_context, addr, it->second, bind(onEmitEvent, m_manager->Next(), m_server, hdl, ::_1, ::_2, ::_3));
      responsePayload.push_back(m_manager->Add(event));
    }
  }

  return responsePayload;
}

ScMemoryJsonPayload ScMemoryJsonEventsHandler::HandleDelete(
    ScServerConnectionHandle const & hdl,
    ScMemoryJsonPayload const & message,
    ScMemoryJsonPayload & errorsPayload)
{
  SC_UNUSED(hdl);

  for (auto & atom : message)
    delete m_manager->Remove(atom.get<size_t>());

  return message;
}
