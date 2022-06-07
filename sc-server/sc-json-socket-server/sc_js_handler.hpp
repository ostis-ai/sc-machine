#pragma once

#include <string>
#include "sc_js_payload.hpp"
#include "sc-json-socket-action/sc_js_action_defines.hpp"


class ScJSHandler {
public:
  ScJSHandler()
  {
    m_context = new ScMemoryContext("sc-json-socket-handler");
  }

  ScJSPayload Handle(ScJSPayload const & requestMassage)
  {
    ScJSPayload const & requestType = requestMassage["type"];
    ScJSPayload const & requestPayload = requestMassage["payload"];

    sc_bool status = SC_FALSE;
    ScJSPayload responsePayload;
    auto const & it = m_actions.find(requestType);
    if (it != m_actions.end())
    {
      auto * action = it->second;
      responsePayload = action->Complete(m_context, requestPayload);
      status = responsePayload.is_null() == SC_FALSE;
    }
    else
      responsePayload = "Unsupported request type: " + requestType.get<std::string>();

    return {
          { "id", requestMassage["id"] },
          { "event", SC_FALSE },
          { "status", status },
          { "payload", responsePayload }
    };
  }

  ~ScJSHandler()
  {
    delete m_context;

    for (auto const & it : m_actions)
      delete it.second;
  }

private:
  ScMemoryContext * m_context;

  std::map<std::string, ScJSAction * > m_actions = {
        { "keynodes", new ScJSHandleKeynodesAction() },
        { "create_elements", new ScJSCreateElementsAction() },
        { "check_elements", new ScJSCheckElementsAction() },
        { "delete_elements", new ScJSDeleteElementsAction() },
        { "search_template", new ScJSTemplateSearchAction() },
        { "generate_template", new ScJSTemplateGenerateAction() },
        { "content", new ScJSHandleContentAction() },
        { "events", new ScJSHandleEventsAction() },
  };
};
