#pragma once

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

using ScClientCore = websocketpp::client<websocketpp::config::asio_client>;
using ScClientConnection = ScClientCore::connection_ptr;

using ScClientErrorCode = websocketpp::lib::error_code;
