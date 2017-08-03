#include "sc_python_service.hpp"
#include "sc_python_interp.hpp"

namespace py
{

ScPythonService::ScPythonService(std::string const & scriptName)
  : m_scriptName(scriptName)
{
  m_bridge.reset(new ScPythonBridge());
}

void ScPythonService::Run()
{
  SC_ASSERT(m_workThread.get() == nullptr, ("Can't run service twicely"));

  // Run script in a separate thread
  m_workThread.reset(new std::thread([&]
  {
    py::ScPythonInterpreter::RunScript(m_scriptName, m_bridge);
  }));
  m_workThread->detach();

  // wait until bridge starts
  m_bridge->WaitInitialize();
}

void ScPythonService::Stop()
{
  m_bridge->Close();
  m_workThread->join();
}


}