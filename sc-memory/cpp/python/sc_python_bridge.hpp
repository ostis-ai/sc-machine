/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "../sc_types.hpp"
#include "../sc_utils.hpp"
#include "../sc_wait.hpp"

#include <mutex>
#include <string>

namespace py
{

class ScPythonBridge
{
public:
  using CloseFunc = std::function<void()>;

  ScPythonBridge();
    
  /* Waits until bridge would be initialized
  * Returns true, when bridge initialized correctly; otherwise - false
  */
  bool WaitReady(uint32_t timeOutMS = 10000);
  bool IsInitialized() const;
  bool IsFinished() const;
  void Close();

  void SetInitParams(std::string const & params);
  std::string const & GetInitParams() const;

  // functions that called from python
  void PythonReady();
  void PythonFinish();

private:
  ScWait m_initWait;
  bool m_isInitialized : 1;
  bool m_isFinished : 1;
  std::string m_initParams;

  mutable std::mutex m_lock;

public:
  // TODO: find solution to make them protected
  // delegates
  CloseFunc m_closeDelegate;
};

using ScPythonBridgePtr = std::shared_ptr<ScPythonBridge>;

} // py