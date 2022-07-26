/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-server-impl/sc_server_impl.hpp"
#include "sc_memory_config.hpp"

class ScServerFactory
{
public:
  static std::unique_ptr<ScServer> ConfigureScServer(const ScParams & serverParams, sc_memory_params memoryParams);
};
