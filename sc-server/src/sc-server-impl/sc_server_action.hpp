#pragma once

#include "sc_server_defines.hpp"

class ScServerAction
{
public:
  virtual void Emit() = 0;

  virtual ~ScServerAction() = default;
};
