#include "sc_python_service.hpp"
#include "sc_python_interp.hpp"

#include "../sc_memory.hpp"

namespace py
{
ScPythonService::ScPythonService(std::string const & scriptName)
  : m_scriptName(scriptName)
{
  m_bridge.reset(new ScPythonBridge());
}

ScPythonService::~ScPythonService()
{
  // wait until thread will finished, to destroy it
  // if (m_bridge && m_bridge->TryClose())
  {
    if (m_workThread->joinable())
      m_workThread->join();
  }
}

void ScPythonService::Run(std::string const & params)
{
  SC_LOG_LOAD("Python service " + m_scriptName);
  SC_ASSERT(m_workThread.get() == nullptr, ("Can't run service twicely"));
  m_bridge->SetInitParams(params);

  // Run script in a separate thread
  m_workThread.reset(new std::thread([&] {
    py::ScPythonInterpreter::RunScript(
        m_scriptName, ScMemoryContext(sc_access_lvl_make_min, m_scriptName.c_str()), m_bridge);

    SC_LOG_UNLOAD("Python service " + m_scriptName);
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

}  // namespace py
