/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/kpm/sc_module.hpp>

#include "../sc-server-impl/sc_server.hpp"
#include "sc_memory_config.hpp"

SC_MODULE(ScServerModule);

static std::shared_ptr<ScServer> m_server;
static ScParams ms_serverParams;
