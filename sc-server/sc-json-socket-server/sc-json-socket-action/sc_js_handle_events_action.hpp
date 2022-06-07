#pragma once

#include "sc_js_action.hpp"

#include "sc-memory/sc_event.hpp"


class ScJSHandleEventsAction : public ScJSAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    if (requestPayload.find("create") != requestPayload.end())
    {
      requestPayload = requestPayload["create"];

      ScJSPayload responsePayload;

      for (auto & atom : requestPayload)
      {
        std::string const & type = atom["type"];
        ScAddr const & addr = ScAddr(atom["type"].get<size_t>());

        auto const & it = events.find(type);
        if (it != events.end())
          ScEvent(*context, addr, it->second);
      }
      return responsePayload;
    }

    return {};
  }

protected:
  std::map<std::string, ScEvent::Type> events = {
        { "add_outgoing_edge", ScEvent::Type::AddOutputEdge },
        { "add_ingoing_edge", ScEvent::Type::AddInputEdge },
        { "remove_outgoing_edge", ScEvent::Type::RemoveOutputEdge },
        { "remove_ingoing_edge", ScEvent::Type::RemoveInputEdge },
        { "content_change", ScEvent::Type::ContentChanged },
        { "delete_element", ScEvent::Type::EraseElement },
  };
};
