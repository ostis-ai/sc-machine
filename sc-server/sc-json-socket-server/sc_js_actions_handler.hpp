#pragma once

#include <string>

#include "sc_ws_actions_handler.hpp"
#include "sc_js_payload.hpp"
#include "sc-json-socket-action/sc_js_action_defines.hpp"

class ScJSActionsHandler : public ScWSActionsHandler
{
public:
  ScJSActionsHandler()
  {
    m_context = new ScMemoryContext("sc-json-socket-actions-handler");
  }

  std::string Handle(std::string const & requestMessageText) override;

  ~ScJSActionsHandler() override
  {
    delete m_context;

    for (auto const & it : m_actions)
      delete it.second;
  }

private:
  ScMemoryContext * m_context;

  std::map<std::string, ScJSAction *> m_actions = {
      {"keynodes", new ScJSHandleKeynodesAction()},
      {"create_elements", new ScJSCreateElementsAction()},
      {"check_elements", new ScJSCheckElementsAction()},
      {"delete_elements", new ScJSDeleteElementsAction()},
      {"search_template", new ScJSTemplateSearchAction()},
      {"generate_template", new ScJSTemplateGenerateAction()},
      {"content", new ScJSHandleContentAction()},
  };

  static std::string GenerateResponseText(size_t requestId, sc_bool status, ScJSPayload const & responsePayload);
};
