#pragma once

#include <string>
#include "../sc_server_defines.hpp"
#include "../sc_server.hpp"

class ScMemoryJsonHandler
{
public:
  virtual std::string Handle(ScServerConnectionHandle const & hdl, std::string const & requestMessageText) = 0;

  virtual ~ScMemoryJsonHandler() = default;
};
