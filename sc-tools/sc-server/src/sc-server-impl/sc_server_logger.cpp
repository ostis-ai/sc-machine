/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_logger.hpp"

std::string ScServerLogger::DefineType(std::string const & logType)
{
  std::string defined;

  auto const & it = m_types.find(logType);
  if (it != m_types.end())
    return logType;

  return SC_SERVER_CONSOLE_TYPE;
}

std::string ScServerLogger::DefineFile(std::string const & logType, std::string const & logFile)
{
  std::string defined;

  if (logType == SC_SERVER_FILE_TYPE)
    return logFile;

  static std::string empty;
  return empty;
}

std::string ScServerLogger::DefineLevel(std::string const & logLevel)
{
  std::string defined;

  auto const & it = m_levels.find(logLevel);
  if (it != m_types.end())
    return logLevel;

  return SC_SERVER_INFO_LEVEL;
}

std::ostream * ScServerLogger::GetStream()
{
  return m_instance ? reinterpret_cast<std::ostream *>(m_instance) : &std::cout;
}
