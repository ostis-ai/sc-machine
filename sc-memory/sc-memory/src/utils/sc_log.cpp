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

namespace
{
// should be synced with ScLog::Type
std::string const kTypeToStr[] = {"Debug", "Info", "Warning", "Error"};

// should be synced with ScLog::OutputType
std::string const kOutputTypeToStr[] = {"Console", "File"};

}  // namespace

namespace utils
{
ScLock gLock;
ScLog * ScLog::ms_instance = nullptr;

ScLog * ScLog::SetUp(std::string const & logType, std::string const & logFile, std::string const & logLevel)
{
  delete ms_instance;
  return new ScLog(logType, logFile, logLevel);
}

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

  ms_instance = this;
}

ScLog::ScLog(std::string const & logType, std::string const & logFile, std::string const & logLevel)
{
  m_isMuted = false;

  int modeIndex = FindEnumElement(kTypeToStr, logLevel);
  m_mode = modeIndex != -1 ? Type(modeIndex) : Type::Info;

  int outputTypeIndex = FindEnumElement(kOutputTypeToStr, logType);
  m_output_mode = outputTypeIndex != -1 ? OutputType(outputTypeIndex) : OutputType::Console;

  if (m_output_mode == OutputType::File)
  {
    Initialize(logFile);
  }

  ms_instance = this;
}

ScLog::~ScLog()
{
  Shutdown();
  ms_instance = nullptr;
}

bool ScLog::Initialize(std::string const & logFile)
{
  if (m_output_mode == OutputType::File)
    m_fileStream.open(logFile, std::ofstream::out | std::ofstream::trunc);

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
    return;  // do nothing on mute

  utils::ScLockScope lock(gLock);
  if (m_mode <= type)
  {
    // get time
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::stringstream ss;
    ss << "[" << std::setw(2) << std::setfill('0') << tm.tm_hour << ":" << std::setw(2) << std::setfill('0')
       << tm.tm_min << ":" << std::setw(2) << std::setfill('0') << tm.tm_sec << "][" << kTypeToStr[int(type)] << "]: ";

    if (m_output_mode == OutputType::Console)
    {
      ScConsole::SetColor(ScConsole::Color::White);
      std::cout << ss.str();
      ScConsole::SetColor(color);
      std::cout << msg << std::endl;

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

void ScLog::SetFileName(std::string const & file_name)
{
  Shutdown();
  Initialize(file_name);
}

template <size_t N>
int ScLog::FindEnumElement(std::string const (&elements)[N], std::string const & externalValue)
{
  size_t size = N;
  int index = -1;
  for (size_t i = 0; i < size; i++)
  {
    std::string mode = elements[i];
    if (externalValue == mode)
    {
      index = (int)i;
      break;
    }
  }
  return index;
}

}  // namespace utils
