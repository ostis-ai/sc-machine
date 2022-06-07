#pragma once

#include <websocketpp/server.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

using ScWSServerCore = websocketpp::server<websocketpp::config::asio>;
using ScWSConnectionHandle = websocketpp::connection_hdl;

using ScWSMessagePtr = ScWSServerCore::message_ptr;
using ScWSMessageType = websocketpp::frame::opcode::value;
using ScWSMessageHandler = ScWSServerCore::message_handler;

using ScWSException = websocketpp::exception;

using ScLogChannel = websocketpp::log::level;
using ScWSServerLogMessages = websocketpp::log::alevel;
using ScWSServerLogErrors = websocketpp::log::elevel;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
