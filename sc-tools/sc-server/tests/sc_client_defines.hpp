/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

using ScClientCore = websocketpp::client<websocketpp::config::asio_client>;
using ScClientConnection = ScClientCore::connection_ptr;

using ScClientErrorCode = websocketpp::lib::error_code;
