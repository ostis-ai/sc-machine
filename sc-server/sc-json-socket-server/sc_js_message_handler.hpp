#pragma once

#include "sc_js_lib.hpp"
#include "sc_js_server.hpp"
#include "sc_js_actions_handler.hpp"

class ScJSMessageHandler
{
public:
  static void OnMessage(ScJSServer * server, ScWSConnectionHandle const & hdl, ScWSMessagePtr const & msg);
};
