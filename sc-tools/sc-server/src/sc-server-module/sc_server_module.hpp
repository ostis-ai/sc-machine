/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_module.hpp>

#include "../sc-server-impl/sc_server.hpp"

#include "sc_server_module.generated.hpp"

class ScServerModule : public ScModule
{
  SC_CLASS(LoadOrder(1))
  SC_GENERATED_BODY()

  virtual sc_result InitializeImpl() override;

  virtual sc_result ShutdownImpl() override;

private:
  std::shared_ptr<ScServer> m_server;
};
