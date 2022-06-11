#pragma once

#include <websocketpp/server.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

#include <set>

using ScServerPort = size_t;
using ScServerCore = websocketpp::server<websocketpp::config::asio>;
using ScServerConnectionHandle = websocketpp::connection_hdl;
using ScServerConnections = std::set<ScServerConnectionHandle, std::owner_less<ScServerConnectionHandle>>;

using ScServerMessage = ScServerCore::message_ptr;
using ScServerMessageType = websocketpp::frame::opcode::value;

using ScServerException = websocketpp::exception;

using ScServerLogChannel = websocketpp::log::level;
using ScServerLogMessages = websocketpp::log::alevel;
using ScServerLogErrors = websocketpp::log::elevel;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::placeholders::_3;
