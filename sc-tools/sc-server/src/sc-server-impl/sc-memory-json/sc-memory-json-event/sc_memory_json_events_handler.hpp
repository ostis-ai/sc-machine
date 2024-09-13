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
  using ScEventSubscriptionGenerateCallback = std::function<ScEventSubscription *(
      ScMemoryContext *,
      ScAddr const &,
      ScMemoryJsonEventsManager *,
      ScServer *,
      ScServerSessionId const &)>;
  explicit ScMemoryJsonEventsHandler(ScServer * server, ScMemoryContext * processCtx);

  ~ScMemoryJsonEventsHandler() override;

private:
  ScMemoryContext * m_context;
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
