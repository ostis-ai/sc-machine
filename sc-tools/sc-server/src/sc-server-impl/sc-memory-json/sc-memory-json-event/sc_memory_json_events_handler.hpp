/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <unordered_map>

#include "sc-server-impl/sc-memory-json/sc_memory_json_handler.hpp"
#include "sc-server-impl/sc-memory-json/sc_memory_json_payload.hpp"

#include "sc_memory_json_events_manager.hpp"

#include "sc-server-impl/sc_server_defines.hpp"
#include "sc-server-impl/sc_server.hpp"

class ScAgentContext;

class ScMemoryJsonEventsHandler : public ScMemoryJsonHandler
{
public:
  using ScEventSubscriptionGenerateCallback = std::function<ScEventSubscription *(
      ScAgentContext *,
      ScAddr const &,
      ScMemoryJsonEventsManager *,
      ScServer *,
      ScServerSessionId const &)>;
  explicit ScMemoryJsonEventsHandler(ScServer * server, ScAgentContext * processCtx);

  ~ScMemoryJsonEventsHandler() override;

private:
  ScAgentContext * m_context;
  ScMemoryJsonEventsManager * m_manager;

  static std::unordered_map<std::string, std::string> const m_deprecatedEventsIdtfsToSystemEventsIdtfs;

  ScMemoryJsonPayload HandleRequestPayload(
      ScServerSessionId const & sessionId,
      std::string const & requestType,
      ScMemoryJsonPayload const & requestPayload,
      ScMemoryJsonPayload & errorsPayload,
      sc_bool & status,
      sc_bool & isEvent) override;

  ScMemoryJsonPayload HandleGenerate(
      ScServerSessionId const & sessionId,
      ScMemoryJsonPayload const & message,
      ScMemoryJsonPayload & errorsPayload);

  ScMemoryJsonPayload HandleDelete(
      ScServerSessionId const & sessionId,
      ScMemoryJsonPayload const & message,
      ScMemoryJsonPayload & errorsPayload);
};
