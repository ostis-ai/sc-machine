/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_logger_impl.hpp"

ScServerLoggerImpl::ScServerLoggerImpl(
    std::shared_ptr<ScServer> const & server,
    std::string const & logType,
    std::string const & logFile,
    std::string const & logLevel)
  : m_server(server)
{
  Initialize(logType, logFile, logLevel);
}

void ScServerLoggerImpl::Initialize(std::string logType, std::string logFile, std::string logLevel)
{
  m_server->LogMessage(ScServerErrorLevel::info, "Build logger");

  logType = DefineType(logType);
  logFile = DefineFile(logType, logFile);
  logLevel = DefineLevel(logLevel);

  {
    m_server->LogMessage(ScServerErrorLevel::info, "Logger:");
    m_server->LogMessage(ScServerErrorLevel::info, "\tLog type: " + logType);
    m_server->LogMessage(
        ScServerErrorLevel::info,
        "\tLog file: " + ((logType != SC_SERVER_FILE_TYPE || logFile.empty()) ? "No" : logFile));
    m_server->LogMessage(ScServerErrorLevel::info, "\tLog level: " + logLevel);
  }

  if (logFile.empty() == SC_FALSE)
  {
    m_instance = new std::ofstream();
    m_instance->open(logFile);
  }
  else
  {
    m_instance = nullptr;
  }

  m_server->ClearChannels();

  ScServerLogLevel level = ScServerErrorLevel::error | ScServerErrorLevel::fatal | ScServerErrorLevel::warning;
  if (logLevel == SC_SERVER_DEBUG_LEVEL)
    level |= (ScServerErrorLevel::debug | ScServerErrorLevel::info);
  else if (logLevel == SC_SERVER_INFO_LEVEL)
    level |= ScServerErrorLevel::info;

  m_server->SetChannels(level);
  m_server->LogMessage(ScServerErrorLevel::info, "Logger built");
}
