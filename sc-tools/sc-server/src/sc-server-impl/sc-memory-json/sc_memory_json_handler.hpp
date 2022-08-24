/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include "../sc_server_defines.hpp"
#include "../sc_server.hpp"

class ScMemoryJsonHandler
{
public:
  explicit ScMemoryJsonHandler(ScServer * server)
    : m_server(server)
  {
  }

  virtual std::string Handle(ScServerConnectionHandle const & hdl, std::string const & requestMessageText) = 0;

  virtual ~ScMemoryJsonHandler() = default;

protected:
  ScServer * m_server;
};
