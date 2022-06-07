#pragma once

#include <websocketpp/server.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

#include <functional>

#include "sc_js_handler.hpp"

using ScWebSocketServerCore = websocketpp::server<websocketpp::config::asio>;
using ScWebSocketMessagePtr = ScWebSocketServerCore::message_ptr;
using ScWebSocketConnectionHandle = websocketpp::connection_hdl;
using ScWebSocketException = websocketpp::exception;
using ScWebSocketMessageType = websocketpp::frame::opcode::value;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class ScJSServer
{
public:
  ScJSServer()
  {
    m_instance = new ScWebSocketServerCore();

    Initialize();
  }

  void Run(size_t port) {
    m_instance->listen(port);
    m_instance->start_accept();
    m_instance->set_message_handler(bind(&OnMessage, m_instance, ::_1, ::_2));
    m_instance->run();
  }

  void Stop()
  {
    m_instance->stop();
  }

  ~ScJSServer()
  {
    delete m_instance;
  }

private:
  ScWebSocketServerCore * m_instance;

  static void OnMessage(
        ScWebSocketServerCore * server,
        ScWebSocketConnectionHandle const & hdl,
        ScWebSocketMessagePtr const & msg)
  {
    try
    {
      std::cout << msg->get_payload() << std::endl;
      auto const & responsePayload = ScJSHandler().Handle(ScJSPayload::parse(msg->get_payload()));

      std::string const & responseText = responsePayload.dump();
      server->send(hdl, responseText, ScWebSocketMessageType::text);
      std::cout << responseText << std::endl;
    } catch (ScWebSocketException const & e)
    {

    }
  }

  void Initialize()
  {
    m_instance->clear_access_channels(websocketpp::log::alevel::all);
    m_instance->set_access_channels(websocketpp::log::alevel::connect);
    m_instance->set_access_channels(websocketpp::log::alevel::disconnect);
    m_instance->set_access_channels(websocketpp::log::alevel::app);
    m_instance->set_error_channels(websocketpp::log::alevel::all);

//    std::ofstream log;
//    log.open("output.log");
//    m_instance->get_alog().set_ostream(&log);
//    m_instance->get_elog().set_ostream(&log);

    m_instance->init_asio();
  }
};
