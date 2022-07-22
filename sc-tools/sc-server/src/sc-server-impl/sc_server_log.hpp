/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

#include "sc_server_defines.hpp"

class ScServerLog
{
public:
  ScServerLog(
      ScServerCore * server, std::string const & logType, std::string const & logFile, std::string const & logLevel)
    : m_server(server)
  {
    Initialize(logType, logFile, logLevel);
  }

  void Initialize(std::string logType, std::string logFile, std::string logLevel)
  {
    logType = DefineType(logType);
    logFile = DefineFile(logType, logFile);
    logLevel = DefineLevel(logLevel);

    if (logFile.empty() == SC_FALSE)
    {
      m_instance = new std::ofstream();
      m_instance->open(logFile);

      m_server->get_alog().set_ostream(m_instance);
      m_server->get_elog().set_ostream(m_instance);
    }
    else
    {
      m_instance = nullptr;
      m_server->get_alog().set_ostream();
      m_server->get_elog().set_ostream();
    }

    m_server->clear_access_channels(ScServerLogMessages::all);
    m_server->clear_error_channels(ScServerLogErrors::all);

    if (logLevel == SC_SERVER_DEBUG_LEVEL)
      m_server->set_access_channels(ScServerLogMessages::all);
    else if (logLevel == SC_SERVER_INFO_LEVEL)
      m_server->set_access_channels(ScServerLogMessages::app);
    else if (logLevel == SC_SERVER_ERROR_LEVEL)
      m_server->set_access_channels(ScServerLogMessages::fail);
    m_server->set_error_channels(ScServerLogErrors::all);
  }

  std::string DefineType(std::string const & logType)
  {
    std::string defined;

    auto const & it = m_types.find(logType);
    if (it != m_types.end())
      return logType;

    return SC_SERVER_CONSOLE_TYPE;
  }

  std::string DefineFile(std::string const & logType, std::string const & logFile)
  {
    std::string defined;

    if (logType == SC_SERVER_FILE_TYPE)
      return logFile;

    static std::string empty;
    return empty;
  }

  std::string DefineLevel(std::string const & logLevel)
  {
    std::string defined;

    auto const & it = m_levels.find(logLevel);
    if (it != m_types.end())
      return logLevel;

    return SC_SERVER_INFO_LEVEL;
  }

  ~ScServerLog()
  {
    delete m_instance;
    m_instance = nullptr;
  }

private:
  std::ofstream * m_instance{};
  ScServerCore * m_server;

  std::unordered_set<std::string> m_types = {SC_SERVER_CONSOLE_TYPE, SC_SERVER_FILE_TYPE};
  std::unordered_set<std::string> m_levels = {SC_SERVER_DEBUG_LEVEL, SC_SERVER_INFO_LEVEL, SC_SERVER_ERROR_LEVEL};
};
