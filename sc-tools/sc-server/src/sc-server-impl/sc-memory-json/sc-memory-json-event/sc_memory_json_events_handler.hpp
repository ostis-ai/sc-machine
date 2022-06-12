#pragma once

#include <string>

#include "sc-memory/sc_memory.hpp"

#include "../sc_memory_json_handler.hpp"
#include "../sc_memory_json_payload.hpp"

#include "sc_memory_json_events_manager.hpp"

#include "../../sc_server_defines.hpp"
#include "../../sc_server.hpp"

class ScMemoryJsonEventsHandler : public ScMemoryJsonHandler
{
public:
  explicit ScMemoryJsonEventsHandler(ScServer * server);

  std::string Handle(ScServerConnectionHandle const & hdl, std::string const & requestMessageText) override;

  ~ScMemoryJsonEventsHandler() override
  {
    delete m_context;
  }

private:
  ScMemoryContext * m_context;
  ScMemoryJsonEventsManager * m_manager;

  ScServer * m_server;

  std::map<std::string, ScEvent::Type> events = {
      {"add_outgoing_edge", ScEvent::Type::AddOutputEdge},
      {"add_ingoing_edge", ScEvent::Type::AddInputEdge},
      {"remove_outgoing_edge", ScEvent::Type::RemoveOutputEdge},
      {"remove_ingoing_edge", ScEvent::Type::RemoveInputEdge},
      {"content_change", ScEvent::Type::ContentChanged},
      {"delete_element", ScEvent::Type::EraseElement},
  };

  ScMemoryJsonPayload HandleCreate(ScServerConnectionHandle const & hdl, ScMemoryJsonPayload const & message);

  ScMemoryJsonPayload HandleDelete(ScServerConnectionHandle const & hdl, ScMemoryJsonPayload const & message);

  static std::string GenerateResponseText(
      size_t requestId,
      sc_bool event,
      sc_bool status,
      ScMemoryJsonPayload const & responsePayload);
};
