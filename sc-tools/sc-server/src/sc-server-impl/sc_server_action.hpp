/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server_defines.hpp"

class ScServerAction
{
public:
  explicit ScServerAction(ScServerUserProcessId userProcessId)
    : m_userProcessId(std::move(userProcessId))
  {
  }

  virtual void Emit() = 0;

  virtual ~ScServerAction() = default;

protected:
  ScServerUserProcessId m_userProcessId;
};
