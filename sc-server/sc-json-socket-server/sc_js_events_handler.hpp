#pragma once

#include <string>

#include "sc-memory/sc_memory.hpp"

#include "sc_js_lib.hpp"
#include "sc_js_payload.hpp"
#include "sc_ws_actions_handler.hpp"
#include "sc-json-socket-action/sc_js_events.hpp"

class ScJSEventsHandler : public ScWSActionsHandler
{
public:
  ScJSEventsHandler();

  std::string Handle(std::string const & requestMessageText) override;

  void SetServer(ScWSServerCore * server)
  {
    m_server = server;
  }

  void SetConnection(ScWSConnectionHandle * hdl)
  {
    m_hdl = hdl;
  }

  sc_bool IsConnectionValid()
  {
    return m_hdl != nullptr;
  }

  ~ScJSEventsHandler() override
  {
    delete m_context;
  }

private:
  ScMemoryContext * m_context;
  ScJSEventsManager * m_manager;

  ScWSServerCore * m_server;
  ScWSConnectionHandle * m_hdl;

  std::map<std::string, ScEvent::Type> events = {
      {"add_outgoing_edge", ScEvent::Type::AddOutputEdge},
      {"add_ingoing_edge", ScEvent::Type::AddInputEdge},
      {"remove_outgoing_edge", ScEvent::Type::RemoveOutputEdge},
      {"remove_ingoing_edge", ScEvent::Type::RemoveInputEdge},
      {"content_change", ScEvent::Type::ContentChanged},
      {"delete_element", ScEvent::Type::EraseElement},
  };

  ScJSPayload HandleCreate(ScJSPayload const & message);

  ScJSPayload HandleDelete(ScJSPayload const & message);

  static std::string GenerateResponseText(
      size_t requestId,
      sc_bool event,
      sc_bool status,
      ScJSPayload const & responsePayload);
};
