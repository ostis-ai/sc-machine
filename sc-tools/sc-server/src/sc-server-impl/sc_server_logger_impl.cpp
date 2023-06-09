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
  logType = DefineType(logType);
  logFile = DefineFile(logType, logFile);
  logLevel = DefineLevel(logLevel);

  {
    m_server->LogMessage(ScServerLogMessages::app, "Logger:");
    m_server->LogMessage(ScServerLogMessages::app, "\tLog type: " + logType);
    m_server->LogMessage(
        ScServerLogMessages::app,
        "\tLog file: " + ((logType != SC_SERVER_FILE_TYPE || logFile.empty()) ? "No" : logFile));
    m_server->LogMessage(ScServerLogMessages::app, "\tLog level: " + logLevel);
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

  if (logLevel == SC_SERVER_DEBUG_LEVEL)
    m_server->SetMessageChannels(ScServerLogMessages::all);
  else if (logLevel == SC_SERVER_INFO_LEVEL)
    m_server->SetMessageChannels(ScServerLogMessages::app);
  else if (logLevel == SC_SERVER_ERROR_LEVEL)
    m_server->SetMessageChannels(ScServerLogMessages::fail);
  m_server->SetErrorChannels(ScServerLogErrors::info);

  m_server->LogMessage(ScServerLogMessages::app, "Logger applied");
}
