#include "sc_python_threads.hpp"

namespace py
{

ScPythonMainThread::ScPythonMainThread()
{
  Py_Initialize();
  PyEval_InitThreads();

  m_GILState = PyGILState_Ensure();
  m_threadState = PyEval_SaveThread();
}

ScPythonMainThread::~ScPythonMainThread()
{
  PyEval_RestoreThread(m_threadState);
  PyGILState_Release(m_GILState);

  Py_Finalize();
}


// ------------------------------
ScPythonSubThread::ScPythonSubThread()
{
  m_mainGILState = PyGILState_Ensure();
  m_oldThreadState = PyThreadState_Get();
  m_newThreadState = Py_NewInterpreter();
  PyThreadState_Swap(m_newThreadState);

  m_subThreadState = PyEval_SaveThread();
  m_subGILState = PyGILState_Ensure();
}

ScPythonSubThread::~ScPythonSubThread()
{
  PyGILState_Release(m_subGILState);
  PyEval_RestoreThread(m_subThreadState);
  Py_EndInterpreter(m_newThreadState);
  PyThreadState_Swap(m_oldThreadState);
  PyGILState_Release(m_mainGILState);
}

} // namespace py
