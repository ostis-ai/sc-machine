/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

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

  ~ScMemoryJsonEventsHandler() override;

private:
  ScMemoryContext * m_context;
  ScMemoryJsonEventsManager * m_manager;

  std::map<std::string, ScEvent::Type> events = {
      {"add_outgoing_edge", ScEvent::Type::AddOutputEdge},
      {"add_ingoing_edge", ScEvent::Type::AddInputEdge},
      {"remove_outgoing_edge", ScEvent::Type::RemoveOutputEdge},
      {"remove_ingoing_edge", ScEvent::Type::RemoveInputEdge},
      {"content_change", ScEvent::Type::ContentChanged},
      {"delete_element", ScEvent::Type::EraseElement},
  };

  ScMemoryJsonPayload HandleRequestPayload(
      ScServerConnectionHandle const & hdl,
      std::string const & requestType,
      ScMemoryJsonPayload const & requestPayload,
      ScMemoryJsonPayload & errorsPayload,
      sc_bool & status,
      sc_bool & isEvent) override;

  ScMemoryJsonPayload HandleCreate(
      ScServerConnectionHandle const & hdl, ScMemoryJsonPayload const & message, ScMemoryJsonPayload & errorsPayload);

  ScMemoryJsonPayload HandleDelete(
      ScServerConnectionHandle const & hdl, ScMemoryJsonPayload const & message, ScMemoryJsonPayload & errorsPayload);
};
