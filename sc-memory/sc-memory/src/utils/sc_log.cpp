/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/utils/sc_log.hpp"
#include "sc-memory/utils/sc_lock.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

namespace utils
{
ScLock gLock;

ScLogLevel::ScLogLevel(ScLogLevel::Level level)
  : m_level(level)
{
}

ScLogLevel::ScLogLevel()
  : ScLogLevel(ScLogLevel::Unknown)
{
}

std::string ScLogLevel::ToString() const
{
  switch (m_level)
  {
  case ScLogLevel::Error:
    return "Error";
  case ScLogLevel::Warning:
    return "Warning";
  case ScLogLevel::Info:
    return "Info";
  case ScLogLevel::Debug:
    return "Debug";
  default:
    return "Unknown";
  }
}

ScLogLevel & ScLogLevel::FromString(std::string const & levelStr)
{
  if (levelStr == "Error")
    m_level = ScLogLevel::Error;
  else if (levelStr == "Warning")
    m_level = ScLogLevel::Warning;
  else if (levelStr == "Info")
    m_level = ScLogLevel::Info;
  else if (levelStr == "Debug")
    m_level = ScLogLevel::Debug;
  else
    m_level = ScLogLevel::Unknown;

  return *this;
}

bool ScLogLevel::operator<=(ScLogLevel const & other) const
{
  return static_cast<int>(m_level) <= static_cast<int>(other.m_level);
}

ScLog::ScLog()
  : ScLog(ScLogType::Console, "", ScLogLevel::Level::Info)
{
}

ScLog::ScLog(ScLogType const & logType, std::string const & logFile, ScLogLevel const & logLevel)
  : m_isMuted(false)
  , m_logType(logType)
  , m_logFile(logFile)
  , m_logLevel(logLevel)
{
  if (m_logType == ScLogType::File)
    SetLogFile(m_logFile);
}

ScLog::~ScLog()
{
  Clear();
}

ScLog::ScLog(ScLog const & other)
{
  *this = other;
}

ScLog & ScLog::operator=(ScLog const & other)
{
  if (this != &other)
  {
    m_isMuted = other.m_isMuted;
    m_logType = other.m_logType;
    if (m_logType == ScLogType::File)
      SetLogFile(other.m_logFile);
    m_logLevel = other.m_logLevel;
  }
  return *this;
}

void ScLog::Clear()
{
  if (m_logFileStream.is_open())
  {
    m_logFileStream.flush();
    m_logFileStream.close();
  }
}

void ScLog::SetMuted(bool value)
{
  m_isMuted = value;
}

void ScLog::SetLogFile(std::string const & logFile)
{
  Clear();
  m_logType = ScLog::ScLogType::File;
  m_logFileStream.open(logFile, std::ofstream::out | std::ofstream::trunc);
}

ScLog::ScLogType ScLog::DefineLogType(std::string const & logType)
{
  return logType == "Console" ? ScLog::ScLogType::Console : ScLog::ScLogType::File;
}

void ScLog::Message(
    ScLogLevel logLevel,
    std::string const & message,
    ScConsole::Color color /*= ScConsole::Color::White*/)
{
  // Do nothing on mute
  if (m_isMuted)
    return;

  if (!(m_logLevel <= logLevel))
    return;

  utils::ScLockScope lock(gLock);
  // Get time
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);

  std::stringstream ss;
  ss << "[" << std::setw(2) << std::setfill('0') << tm.tm_hour << ":" << std::setw(2) << std::setfill('0') << tm.tm_min
     << ":" << std::setw(2) << std::setfill('0') << tm.tm_sec << "][" << logLevel.ToString() << "]: ";

  if (m_logType == ScLog::ScLogType::Console)
  {
    ScConsole::SetColor(ScConsole::Color::White);
    std::cout << ss.str();
    ScConsole::SetColor(color);
    std::cout << message << std::endl;

    ScConsole::ResetColor();
  }
  else if (m_logFileStream.is_open())
  {
    m_logFileStream << ss.str() << message << std::endl;
    m_logFileStream.flush();
  }
}

}  // namespace utils
