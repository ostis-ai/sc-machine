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
  PyThreadState * m_oldThreadState;
  PyThreadState * m_newThreadState;
  PyThreadState * m_subThreadState;
  PyGILState_STATE m_subGILState;
};

class WithoutGIL
{
public:
  WithoutGIL()
  {
    m_state = PyEval_SaveThread();
  }
  ~WithoutGIL()
  {
    PyEval_RestoreThread(m_state);
  }

  WithoutGIL(WithoutGIL const &) = delete;
  WithoutGIL & operator=(WithoutGIL const &) = delete;

private:
  PyThreadState * m_state;
};

class WithGIL
{
public:
  WithGIL()
  {
    m_state = PyGILState_Ensure();
  }
  ~WithGIL()
  {
    PyGILState_Release(m_state);
  }

  WithGIL(WithoutGIL const &) = delete;
  WithGIL & operator=(WithoutGIL const &) = delete;

private:
  PyGILState_STATE m_state;
};

}  // namespace py