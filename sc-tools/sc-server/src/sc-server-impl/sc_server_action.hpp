#pragma once

#include "sc_server_defines.hpp"

class ScServerAction
{
public:
  explicit ScServerAction(ScServerConnectionHandle hdl)
    : m_hdl(std::move(hdl))
  {
  }

  virtual void Emit() = 0;

  ScServerConnectionHandle GetHandle()
  {
    return m_hdl;
  }

  virtual ~ScServerAction() = default;

protected:
  ScServerConnectionHandle m_hdl;
};
