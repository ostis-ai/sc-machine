/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_log.hpp"
#include "../sc_debug.hpp"
#include "sc_lock.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>
#include "string"

namespace
{

// should be synced with ScLog::Type
const char * kTypeToStr[] = {
      "Debug", "Info", "Warning", "Error", "Python", "PythonError", "Off"
};

const char * kOutputTypeToStr[] = {
      "Console", "File"
};

} // namespace

namespace utils
{

ScLock gLock;
ScLog * ScLog::ms_instance = nullptr;

ScLog * ScLog::GetInstance()
{
  if (!ms_instance)
    return new ScLog();

  return ms_instance;
}

ScLog::ScLog()
{
  m_isMuted = false;
  m_mode = Type::Info;
  m_output_mode = OutputType::Console;

  std::string s_mode = LOG_MODE;
  int typeSize = sizeof(kTypeToStr) / sizeof(char *);
  for (int i = 0; i < typeSize; i++)
  {
    std::string mode = kTypeToStr[i];
    if (s_mode == mode)
    {
      m_mode = Type(i);
    }
  }

  std::string s_output_mode = LOG_OUTPUT_TYPE;
  int outputTypeSize = sizeof(kOutputTypeToStr) / sizeof(char *);
  for (int i = 0; i < outputTypeSize; i++)
  {
    std::string mode = kOutputTypeToStr[i];
    if (s_output_mode == mode)
    {
      m_output_mode = OutputType(i);
    }
  }

  ASSERT(!ms_instance, ());
  ms_instance = this;
}

ScLog::~ScLog()
{
  ms_instance = nullptr;
}

bool ScLog::Initialize(std::string const & file_name)
{
  std::string log_output_type = LOG_OUTPUT_TYPE;

  if (m_output_mode == OutputType::File)
  {
    std::string file_path = directory_log + file_name + extension_log;
    m_fileStream.open(file_path, std::ofstream::out | std::ofstream::app);
  }
  return m_fileStream.is_open();
}

void ScLog::Shutdown()
{
  if (m_fileStream.is_open())
  {
    m_fileStream.flush();
    m_fileStream.close();
  }
}

void ScLog::Message(ScLog::Type type, std::string const & msg, ScConsole::Color color /*= ScConsole::Color::White*/)
{
  if (m_isMuted && type != Type::Error)
    return; // do nothing on mute

  utils::ScLockScope lock(gLock);
  if (m_mode <= type)
  {
    // get time
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::stringstream ss;
    ss << "[" << std::setw(2) << std::setfill('0') << tm.tm_hour
       << ":" << std::setw(2) << std::setfill('0') << tm.tm_min
       << ":" << std::setw(2) << std::setfill('0') << tm.tm_sec << "]["
       << kTypeToStr[int(type)] << "]: ";

    if (m_output_mode == OutputType::Console)
    {
      ScConsole::SetColor(ScConsole::Color::White);
      std::cout << ss.str();
      ScConsole::SetColor(color);
      std::cout << msg << std::endl;;
      ScConsole::ResetColor();
    }
    else
    {
      if (m_fileStream.is_open())
      {
        m_fileStream << ss.str() << msg << std::endl;
        m_fileStream.flush();
      }
    }
  }
}

void ScLog::SetMuted(bool value)
{
  m_isMuted = value;
}

} // namespace utils
