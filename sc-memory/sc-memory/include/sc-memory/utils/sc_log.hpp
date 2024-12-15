/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_console.hpp"

#include <sstream>
#include <fstream>
#include <string>

namespace utils
{
class ScLogLevel
{
public:
  enum Level
  {
    Error,
    Warning,
    Info,
    Debug,
    Unknown
  };

  ScLogLevel(Level level);
  ScLogLevel();
  std::string ToString() const;
  ScLogLevel & FromString(std::string const & levelStr);
  bool operator<=(ScLogLevel const & other) const;

protected:
  Level m_level;
};

class ScLog
{
public:
  enum class ScLogType : uint8_t
  {
    Console = 0,
    File
  };

  explicit ScLog();
  explicit ScLog(ScLogType const & logType, std::string const & logFile, ScLogLevel const & logLevel);
  explicit ScLog(ScLog const & other);

  ScLog & operator=(ScLog const & other);

  ~ScLog();

  void Clear();
  void SetMuted(bool value);
  void SetPrefix(std::string const & prefix);
  void SetLogFile(std::string const & logFile);

  static ScLogType DefineLogType(std::string const & logType);

  void Message(ScLogLevel level, std::string const & message, ScConsole::Color color = ScConsole::Color::White);

  template <typename T, typename... ARGS>
  void Error(T const & t, ARGS const &... others);

  template <typename T, typename... ARGS>
  void Warning(T const & t, ARGS const &... others);

  template <typename T, typename... ARGS>
  void Info(T const & t, ARGS const &... others);

  template <typename T, typename... ARGS>
  void Debug(T const & t, ARGS const &... others);

protected:
  bool m_isMuted;
  std::string m_prefix;
  ScLogType m_logType;
  std::string m_logFile;
  std::ofstream m_logFileStream;
  ScLogLevel m_logLevel;
};

}  // namespace utils

#include "sc-memory/_template/utils/sc_log.tpp"

static utils::ScLog ms_globalLogger;

#define SC_LOG_COLOR(__type, __message, __color) \
  { \
    std::stringstream ss; \
    ss << __message; \
    ms_globalLogger.Message(__type, ss.str(), __color); \
  }

#define SC_LOG(__type, __message) SC_LOG_COLOR(__type, __message, ScConsole::Color::White)

#define SC_LOG_ERROR(__message) ({SC_LOG_COLOR(utils::ScLogLevel::Level::Error, __message, ScConsole::Color::Red)})
#define SC_LOG_WARNING(__message) \
  ({SC_LOG_COLOR(utils::ScLogLevel::Level::Warning, __message, ScConsole::Color::Yellow)})
#define SC_LOG_INFO(__message) ({SC_LOG_COLOR(utils::ScLogLevel::Level::Info, __message, ScConsole::Color::Grey)})
#define SC_LOG_DEBUG(__message) \
  ({SC_LOG_COLOR(utils::ScLogLevel::Level::Debug, __message, ScConsole::Color::LightBlue)})
#define SC_LOG_INFO_COLOR(__message, __color) ({SC_LOG_COLOR(::utils::ScLogType::Info, __message, __color)})
