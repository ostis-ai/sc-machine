#pragma once

#include "sc_js_server.hpp"
#include "sc_js_message_handler.hpp"

class ScServerFactory
{
public:
  static ScJSServer * ConfigureScJSServer(sc_memory_params params)
  {
    auto * server = new ScJSServer(params);

    server->SetMessageChannels(
        ScWSServerLogMessages::connect | ScWSServerLogMessages::disconnect | ScWSServerLogMessages::app);
    server->SetErrorChannels(ScWSServerLogErrors::all);

    SC_LOG_INFO("[sc-server] Set default messages handler");
    server->SetMessageHandler(bind(&ScJSMessageHandler::OnMessage, server, ::_1, ::_2));

    return server;
  }
};
