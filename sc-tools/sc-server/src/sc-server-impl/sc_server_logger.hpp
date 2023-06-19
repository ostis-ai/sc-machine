/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <unordered_set>

#include <fstream>

#include "sc_server_defines.hpp"

class ScServerLogger
{
public:
  virtual void Initialize(std::string logType, std::string logFile, std::string logLevel) = 0;

  std::string DefineType(std::string const & logType);

  std::string DefineFile(std::string const & logType, std::string const & logFile);

  std::string DefineLevel(std::string const & logLevel);

  std::ostream * GetStream();

  virtual ~ScServerLogger()
  {
    delete m_instance;
    m_instance = nullptr;
  }

protected:
  std::ofstream * m_instance;

  std::unordered_set<std::string> m_types = {SC_SERVER_CONSOLE_TYPE, SC_SERVER_FILE_TYPE};
  std::unordered_set<std::string> m_levels = {SC_SERVER_DEBUG_LEVEL, SC_SERVER_INFO_LEVEL, SC_SERVER_ERROR_LEVEL};
};
