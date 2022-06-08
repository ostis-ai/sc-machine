#pragma once

#include "sc_js_lib.hpp"
#include "sc_js_server.hpp"
#include "sc_js_actions_handler.hpp"

class ScJSMessageHandler
{
public:
  static void OnMessage(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg);

  static void OnAction(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg);

  static void OnEvent(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg);

private:
  static sc_bool IsEvent(std::string const & message)
  {
    ScJSPayload const & payload = ScJSPayload::parse(message);
    return payload["type"] == "events";
  }
};
