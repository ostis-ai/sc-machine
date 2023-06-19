/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server_logger.hpp"
#include "sc_server.hpp"

class ScServerLoggerImpl : public ScServerLogger
{
public:
  ScServerLoggerImpl(
      std::shared_ptr<ScServer> const & server,
      std::string const & logType,
      std::string const & logFile,
      std::string const & logLevel);

  virtual void Initialize(std::string logType, std::string logFile, std::string logLevel) override;

private:
  std::shared_ptr<ScServer> m_server;
};
