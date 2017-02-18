/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_log.hpp"
#include "../sc_debug.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

namespace utils
{

ScLog * ScLog::ms_instance = nullptr;

ScLog * ScLog::GetInstance()
{
  if (!ms_instance)
    return new ScLog();

  return ms_instance;
}

ScLog::ScLog()
  : m_mode(Debug)
{
  ASSERT(!ms_instance, ());
  ms_instance = this;
}

ScLog::~ScLog()
{
  ms_instance = nullptr;
}

bool ScLog::Initialize(std::string const & file_name, eType mode /*= Info*/)
{
  m_mode = mode;
  m_fileStream.open(file_name, std::ofstream::out | std::ofstream::trunc);
  return m_fileStream.is_open();
}

void ScLog::Shutdown()
{
  m_fileStream.flush();
  m_fileStream.close();
}

void ScLog::Message(ScLog::eType type, std::string const & msg, ScConsole::Color color /*= ScConsole::Color::White*/)
{
  if (m_mode <= type)
  {
    // get time
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::stringstream ss;
    ss << "[" << std::setw(2) << std::setfill('0') << tm.tm_hour
       << ":" << std::setw(2) << std::setfill('0') << tm.tm_min
       << ":" << std::setw(2) << std::setfill('0') << tm.tm_sec << "]: ";

    ScConsole::SetColor(ScConsole::Color::White);
    std::cout << ss.str();
    ScConsole::SetColor(color);
    std::cout << msg << std::endl;;
    ScConsole::ResetColor();

    m_fileStream << ss.str() << msg << std::endl;
    m_fileStream.flush();
  }
}

} // namespace utils
