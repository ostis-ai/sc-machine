#include "sc_python_service.hpp"
#include "sc_python_interp.hpp"

namespace py
{

ScPythonService::ScPythonService(std::string const & scriptName)
  : m_scriptName(scriptName)
{
  m_bridge.reset(new ScPythonBridge());
}

void ScPythonService::Run(std::string const & params)
{
  SC_ASSERT(m_workThread.get() == nullptr, ("Can't run service twicely"));
  m_bridge->SetInitParams(params);

  // Run script in a separate thread
  m_workThread.reset(new std::thread([&]
  {
    py::ScPythonInterpreter::RunScript(m_scriptName, m_bridge);
  }));
  
  // wait until bridge starts
  m_bridge->WaitReady();
}

void ScPythonService::Stop()
{
  m_bridge->Close();
  m_workThread->join();
  m_bridge.reset();
}

bool ScPythonService::IsRun() const
{
  return !m_bridge->IsFinished();
}


}
