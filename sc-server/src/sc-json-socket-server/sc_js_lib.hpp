#pragma once

#include <websocketpp/server.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

#include <set>

using ScWSServerCore = websocketpp::server<websocketpp::config::asio>;
using ScWSConnectionHandle = websocketpp::connection_hdl;
using ScWSServerConnections = std::set<ScWSConnectionHandle, std::owner_less<ScWSConnectionHandle>>;

using ScWSMessagePtr = ScWSServerCore::message_ptr;
using ScWSMessageType = websocketpp::frame::opcode::value;
using ScWSMessageHandler = ScWSServerCore::message_handler;
using ScWSOpenHandler = websocketpp::open_handler;
using ScWSCloseHandler = websocketpp::close_handler;

using ScWSException = websocketpp::exception;

using ScLogChannel = websocketpp::log::level;
using ScWSServerLogMessages = websocketpp::log::alevel;
using ScWSServerLogErrors = websocketpp::log::elevel;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::placeholders::_3;
