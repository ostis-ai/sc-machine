#pragma once

#include "sc_python_includes.hpp"

#include "../sc_debug.hpp"

#include "../utils/sc_lock.hpp"

#include <unordered_map>
#include <thread>

namespace py
{

struct ReleaseGILPolicy
{
  ReleaseGILPolicy() {}

  // Ownership of this argument tuple will ultimately be adopted by
  // the caller.
  template <class ArgumentPackage>
  static bool precall(ArgumentPackage const &)
  {
    // Release GIL and save PyThreadState for this thread here
    utils::ScLockScope guard(m_lock);
  
    PyThreadState * state = PyEval_SaveThread();
    if (!m_threadStates.insert(std::make_pair(std::this_thread::get_id(), state)).second)
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                         "Invalid state of python GIL");
    }

    return true;
  }

  // Pass the result through
  template <class ArgumentPackage>
  static PyObject* postcall(ArgumentPackage const &, PyObject * result)
  {
    // Reacquire GIL using PyThreadState for this thread here
    utils::ScLockScope guard(m_lock);
    auto it = m_threadStates.find(std::this_thread::get_id());
    if (it != m_threadStates.end())
    {
      PyEval_RestoreThread(it->second);
      m_threadStates.erase(it);
    }

    return result;
  }

  typedef boost::python::default_result_converter result_converter;
  typedef PyObject* argument_package;

  template <class Sig>
  struct extract_return_type : boost::mpl::front<Sig>
  {
  };

private:
  static utils::ScLock m_lock;
  static std::unordered_map<std::thread::id, PyThreadState*> m_threadStates;
};

}