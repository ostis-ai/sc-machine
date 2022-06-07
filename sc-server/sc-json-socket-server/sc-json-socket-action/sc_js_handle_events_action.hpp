#pragma once

#include "sc_js_action.hpp"
#include "sc_js_events.hpp"

#include "sc-memory/sc_event.hpp"

class ScJSHandleEventsAction : public ScJSAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    ScJSPayload responsePayload;
    auto * eventsManager = ScJSEventsManager::GetInstance();

    if (requestPayload.find("create") != requestPayload.end())
    {
      requestPayload = requestPayload["create"];

      for (auto & atom : requestPayload)
      {
        std::string const & type = atom["type"];
        ScAddr const & addr = ScAddr(atom["addr"].get<size_t>());

        auto const & it = events.find(type);
        if (it != events.end())
        {
          auto * event = new ScEvent(*context, addr, it->second);
          responsePayload.push_back(eventsManager->Add(event));
        }
      }
    }
    else if (requestPayload.find("delete") != requestPayload.end())
    {
      requestPayload = requestPayload["delete"];

      for (auto & atom : requestPayload)
        delete eventsManager->Remove(atom.get<size_t>());
    }

    return responsePayload;
  }

protected:
  std::map<std::string, ScEvent::Type> events = {
      {"add_outgoing_edge", ScEvent::Type::AddOutputEdge},
      {"add_ingoing_edge", ScEvent::Type::AddInputEdge},
      {"remove_outgoing_edge", ScEvent::Type::RemoveOutputEdge},
      {"remove_ingoing_edge", ScEvent::Type::RemoveInputEdge},
      {"content_change", ScEvent::Type::ContentChanged},
      {"delete_element", ScEvent::Type::EraseElement},
  };
};
