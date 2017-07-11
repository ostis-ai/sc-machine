/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/
#pragma once

#include "../sc_types.hpp"
#include "sc_console.hpp"

#include <sstream>

namespace utils
{

class ScLog final
{
protected:
  _SC_EXTERN ScLog();
  _SC_EXTERN ~ScLog();

public:
  enum class Type : uint8_t
  {
    Debug = 0,
    Info,
    Warning,
    Error
  };

  _SC_EXTERN bool Initialize(std::string const & file_name, Type mode = Type::Info);
  _SC_EXTERN void Shutdown();

  /// TODO: thread safe
  _SC_EXTERN void Message(Type type, std::string const & msg, ScConsole::Color color = ScConsole::Color::White);
  _SC_EXTERN void SetMuted(bool value);

  _SC_EXTERN static ScLog * GetInstance();

private:
  std::ofstream m_fileStream;
  Type m_mode;
  bool m_isMuted;

  static ScLog * ms_instance;
};


#define SC_LOG_COLOR(__type, __msg, __color) \
{ std::stringstream ss; ss << __msg; ::utils::ScLog::GetInstance()->Message(__type, ss.str(), __color); }

#define SC_LOG(__type, __msg) SC_LOG_COLOR(__type, __msg, ScConsole::Color::White)


#define SC_LOG_DEBUG(__msg) { SC_LOG_COLOR(::utils::ScLog::Type::Debug, __msg, ScConsole::Color::LightBlue) }
#define SC_LOG_INFO(__msg) { SC_LOG_COLOR(::utils::ScLog::Type::Info, __msg, ScConsole::Color::Grey) }
#define SC_LOG_WARNING(__msg) { SC_LOG_COLOR(::utils::ScLog::Type::Warning, __msg, ScConsole::Color::Yellow) }
#define SC_LOG_ERROR(__msg) { SC_LOG_COLOR(::utils::ScLog::Type::Error, __msg, ScConsole::Color::Red) }
#define SC_LOG_INFO_COLOR(__msg, __color) { SC_LOG_COLOR(::utils::ScLog::Type::Info, __msg, __color) }

#define SC_LOG_INIT(__msg) { SC_LOG_INFO("[init] " << __msg) }
#define SC_LOG_SHUTDOWN(__msg) { SC_LOG_INFO("[shutdown] " << __msg) }
#define SC_LOG_LOAD(__msg) { SC_LOG_INFO("[load] " << __msg) }
#define SC_LOG_UNLOAD(__msg) { SC_LOG_INFO("[unload] " << __msg) }


} // namesapce utils
