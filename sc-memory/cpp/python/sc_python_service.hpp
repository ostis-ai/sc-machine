#pragma once

#include "sc_python_bridge.hpp"

#include <string>
#include <thread>

namespace py
{

class ScPythonService
{

protected:
  _SC_EXTERN explicit ScPythonService(std::string const & scriptName);

public:
  /** Run specified service. `params` string will be passed into python module initialization function.
    * If you want to implement your own logic on start, then
    * override RunImpl function.
    */
  _SC_EXTERN void Run(std::string const & params = "");

  /** Stops run of this service. This function doesn't returns until service thread stoped
    */
  _SC_EXTERN void Stop();

  /** Check if it still runs
    */
  _SC_EXTERN bool IsRun() const;

protected:
  /// Calls after python script run.
  virtual void RunImpl() = 0;
  /// Calls after service thread stopped.
  virtual void StopImpl() = 0;

  _SC_EXTERN ScPythonBridge & GetBridge() const
  {
    return *m_bridge;
  }

private:
  std::string m_scriptName; // name of script to run
  ScPythonBridgePtr m_bridge; // special bridge to communicate with python script
  std::unique_ptr<std::thread> m_workThread; // thread where script runs
};

// Implementation of dummy python service. Used commonly for test issues
class DummyService : public ScPythonService
{
public:
  explicit DummyService(std::string const & scriptName)
    : py::ScPythonService(scriptName)
  {
  }

private:
  void RunImpl() override {}
  void StopImpl() override {}
};


}