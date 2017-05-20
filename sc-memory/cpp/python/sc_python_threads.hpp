#pragma once

#include "sc_python_includes.hpp"

namespace py
{

class ScPythonMainThread
{
public:
  ScPythonMainThread();
  ~ScPythonMainThread();

private:
  PyGILState_STATE m_GILState;
  PyThreadState * m_threadState;
};

/*! Class to work with interpreter instances in a separate threads
 */
class ScPythonSubThread
{
public:
  ScPythonSubThread();
  ~ScPythonSubThread();

private:
  PyGILState_STATE m_mainGILState;
  PyThreadState* m_oldThreadState;
  PyThreadState* m_newThreadState;
  PyThreadState* m_subThreadState;
  PyGILState_STATE m_subGILState;
};

} // namespace py