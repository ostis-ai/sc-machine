/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_python_bridge.hpp"

namespace py
{

ScPythonBridge::ScPythonBridge()
  : m_isInitialized(false)
  , m_isFinished(false)
{
}

bool ScPythonBridge::WaitReady(uint32_t timeOutMS/* = 10000*/)
{
  return m_initWait.Wait(timeOutMS);
}

bool ScPythonBridge::IsInitialized() const
{
  std::lock_guard<std::mutex> lock(m_lock);

  return m_isInitialized;
}

bool ScPythonBridge::IsFinished() const
{
  std::lock_guard<std::mutex> lock(m_lock);

  return m_isFinished;
}

bool ScPythonBridge::TryClose()
{
  std::lock_guard<std::mutex> lock(m_lock);

  if (!m_isFinished)
  {
    m_isInitialized = false;
    m_isFinished = true;

    if (m_closeDelegate)
      m_closeDelegate();

    return true;
  }

  return false;
}

void ScPythonBridge::Close()
{
  std::lock_guard<std::mutex> lock(m_lock);
  m_isInitialized = false;
  m_isFinished = true;

  if (m_closeDelegate)
    m_closeDelegate();
}

void ScPythonBridge::PythonReady()
{
  std::lock_guard<std::mutex> lock(m_lock);

  m_initWait.Resolve();
  m_isInitialized = true;
}

void ScPythonBridge::PythonFinish()
{
  std::lock_guard<std::mutex> lock(m_lock);

  m_closeDelegate = CloseFunc();
  m_isFinished = true;
}

void ScPythonBridge::SetInitParams(std::string const & params)
{
  m_initParams = params;
}

std::string const & ScPythonBridge::GetInitParams() const
{
  return m_initParams;
}

}