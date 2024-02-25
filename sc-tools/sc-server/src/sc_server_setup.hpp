/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-server-impl/sc_server.hpp"

#include "sc-config/sc_config.hpp"
#include "sc_memory_config.hpp"

void PrintStartMessage();

sc_bool RunServer(ScParams const & serverParams, std::shared_ptr<ScServer> & server);

sc_bool StopServer(std::shared_ptr<ScServer> const & server);

sc_int BuildAndRunServer(sc_int argc, sc_char * argv[]);
