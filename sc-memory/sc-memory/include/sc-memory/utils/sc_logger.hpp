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

/*!
 * @brief Represents different logging levels for messages.
 */
class ScLogLevel
{
public:
  //! Enum representing various log levels.
  enum Level
  {
    Error,    //< Indicates an error message.
    Warning,  //< Indicates a warning message.
    Info,     //< Indicates an informational message.
    Debug,    //< Indicates a debug message.
    Unknown   //< Indicates an unknown log level.
  };

  /*!
   * @brief Constructs a ScLogLevel object with a specified log level.
   *
   * @param level A log level to be set.
   */
  ScLogLevel(Level level);

  /*!
   * @brief Default constructor that initializes the log level to Unknown.
   */
  ScLogLevel();

  /*!
   * @brief Converts the current log level to its string representation.
   *
   * @return A string representing the log level.
   */
  std::string ToString() const;

  /*!
   * @brief Sets the log level based on a string input.
   *
   * @param levelStr A string representing the desired log level.
   * @return A reference to the updated ScLogLevel object.
   */
  ScLogLevel & FromString(std::string const & levelStr);

  /*!
   * @brief Compares two ScLogLevel objects to determine if the current level is less than or equal to another.
   *
   * @param other The other ScLogLevel object to compare against.
   * @return True if the current log level is less than or equal to the other; otherwise, false.
   */
  bool operator<=(ScLogLevel const & other) const;

protected:
  Level m_level;  //< The current log level.
};

/*!
 * @brief Manages logging functionality, allowing messages to be logged to console or file.
 */
class ScLogger
{
public:
  //! Enum class representing the type of logging.
  enum class ScLogType : uint8_t
  {
    Console = 0,  //< Log messages to the console.
    File          //< Log messages to a file.
  };

  /*!
   * @brief Default constructor for ScLogger that initializes with default parameters.
   */
  explicit ScLogger();

  /*!
   * @brief Constructs a ScLogger object with specified parameters for log type, file, and level.
   *
   * @param logType A type of logging (Console or File).
   * @param logFile A filename to log messages if logType is File.
   * @param logLevel An initial logging level.
   */
  explicit ScLogger(ScLogType const & logType, std::string const & logFile, ScLogLevel const & logLevel);

  /*!
   * @brief Copy constructor for the ScLogger class.
   *
   * @param other Another instance of ScLogger to copy from.
   */
  explicit ScLogger(ScLogger const & other);

  /*!
   * @brief Assignment operator for the ScLogger class.
   *
   * @param other The source object to copy from.
   * @return A reference to this object after assignment.
   */
  ScLogger & operator=(ScLogger const & other);

  /*!
   * @brief Destructor for the ScLogger class that ensures resources are cleaned up properly.
   */
  ~ScLogger();

  /*!
   * @brief Clears any open file streams and flushes logs if necessary.
   */
  void Clear();

  //! Mutes logging output.
  void Mute();

  //! Unmute logging output.
  void Unmute();

  /*!
   * @brief Sets a prefix for all logged messages.
   *
   * @param prefix A string that will be prepended to each log message.
   */
  void SetPrefix(std::string const & prefix);

  /*!
   * @brief Opens a specified file for logging and sets it as the current log file.
   *
   * @param logFile A name of the file where logs should be written.
   */
  void SetLogFile(std::string const & logFile);

  /*!
   * @brief Changes severity level of the message being logged.
   * @param level New severity level of the message being logged (e.g., Error, Warning).
   */
  void SetLogLevel(ScLogLevel const & logLevel);

  /*!
   * @brief Defines the type of logging based on a string input ("Console" or "File").
   *
   * @param logType A string indicating the desired logging type.
   * @return A corresponding enum value for the logging type.
   */
  static ScLogType DefineLogType(std::string const & logType);

  /*!
   * @brief Logs a message with a specific severity level and optional color formatting for console output.
   *
   * This function checks whether logging is muted and whether the message's severity
   * meets or exceeds the current logging threshold before logging it to either console
   * or file. It also formats the message with a timestamp and severity label.
   *
   * @param level A severity level of the message being logged (e.g., Error, Warning).
   * @param message A message content to be logged.
   * @param color Optional parameter specifying console color for output; defaults to white.
   */
  void Message(ScLogLevel level, std::string const & message, ScConsole::Color color = ScConsole::Color::White);

  /*!
   * @brief Logs an error message with variable arguments using formatted output.
   *
   * This function allows multiple arguments to be passed and formats them into a single error message
   *
   * @tparam T Type of the first argument.
   * @tparam ARGS Types of subsequent arguments (variadic).
   * @param t The first argument to be logged as part of the error message.
   * @param others Additional arguments that will be included in the error message if provided.
   */
  template <typename T, typename... ARGS>
  void Error(T const & t, ARGS const &... others);

  /*!
   * @brief Logs a warning message with variable arguments using formatted output.
   *
   * This function allows multiple arguments to be passed and formats them into a single warning message
   *
   * @tparam T Type of the first argument.
   * @tparam ARGS Types of subsequent arguments (variadic).
   * @param t The first argument to be logged as part of the warning message.
   * @param others Additional arguments that will be included in the warning message if provided.
   */
  template <typename T, typename... ARGS>
  void Warning(T const & t, ARGS const &... others);

  /*!
   * @brief Logs an informational message with variable arguments using formatted output.
   *
   * This function allows multiple arguments to be passed and formats them into a single informational message
   *
   * @tparam T Type of the first argument.
   * @tparam ARGS Types of subsequent arguments (variadic).
   * @param t The first argument to be logged as part of the informational message.
   * @param others Additional arguments that will be included in the informational message if provided.
   */
  template <typename T, typename... ARGS>
  void Info(T const & t, ARGS const &... others);

  /*!
   * @brief Logs a debug message with variable arguments using formatted output.
   *
   * @tparam T Type of the first argument.
   * @tparam ARGS Types of subsequent arguments (variadic).
   * @param t The first argument to be logged as part of the debug message.
   * @param others Additional arguments that will be included in the debug message if provided.
   */
  template <typename T, typename... ARGS>
  void Debug(T const & t, ARGS const &... others);

protected:
  bool m_isMuted;        //< Indicates whether logging is muted.
  std::string m_prefix;  //< Prefix added to each logged message.
  ScLogType m_logType;   //< Current type of logging (Console or File).
  std::string m_logFile;

private:
  std::ofstream m_logFileStream;  //< Stream for writing logs to file if applicable.
  ScLogLevel m_logLevel;          //< Current logging level threshold.
};

}  // namespace utils

#include "sc-memory/_template/utils/sc_log.tpp"

static utils::ScLogger ms_globalLogger;

/*!
 * Macro for logging messages with color support in console output
 *
 * This macro takes in a type (log level), a message, and color,
 * constructs a stringstream for formatting, and sends it through
 * ms_globalLogger's Message method with appropriate parameters
 *
 * Usage: SC_LOG_COLOR(logLevel, "Your Message", Color);
 */
#define SC_LOG_COLOR(__type, __message, __color) \
  { \
    std::stringstream ss; \
    ss << __message; \
    ms_globalLogger.Message(__type, ss.str(), __color); \
  }

/*!
 * Macro for default white console logging without color specification
 *
 * Usage: SC_LOG(logLevel, "Your Message");
 */
#define SC_LOG(__type, __message) SC_LOG_COLOR(__type, __message, ScConsole::Color::White)

/*!
 * Macro specifically for error messages formatted in red color in console output
 *
 * Usage: SC_LOG_ERROR("An error occurred");
 */
#define SC_LOG_ERROR(__message) ({SC_LOG_COLOR(utils::ScLogLevel::Level::Error, __message, ScConsole::Color::Red)})

/*!
 * Macro specifically for warning messages formatted in yellow color in console output
 *
 * Usage: SC_LOG_WARNING("This is a warning");
 */
#define SC_LOG_WARNING(__message) \
  ({SC_LOG_COLOR(utils::ScLogLevel::Level::Warning, __message, ScConsole::Color::Yellow)})

/*!
 * Macro specifically for informational messages formatted in grey color in console output
 *
 * Usage: SC_LOG_INFO("This is some information");
 */
#define SC_LOG_INFO(__message) ({SC_LOG_COLOR(utils::ScLogLevel::Level::Info, __message, ScConsole::Color::Grey)})

/*!
 * Macro specifically for debug messages formatted in light blue color in console output
 *
 * Usage: SC_LOG_DEBUG("Debugging information");
 */
#define SC_LOG_DEBUG(__message) \
  ({SC_LOG_COLOR(utils::ScLogLevel::Level::Debug, __message, ScConsole::Color::LightBlue)})

/*!
 * Macro specifically for informational messages with custom color formatting in console output
 *
 * Usage: SC_LOG_INFO_COLOR("Custom info", customColor);
 */
#define SC_LOG_INFO_COLOR(__message, __color) ({SC_LOG_COLOR(utils::ScLogLevel::Level::Info, __message, __color)})
