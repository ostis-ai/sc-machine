#pragma once

#include "sc_python_gil.hpp"

#include "sc-memory/cpp/sc_utils.hpp"

namespace py
{

namespace bp = boost::python;

class ScPythonBridgeImpl
{
public:
 
  ScPythonBridgeImpl(ScPythonBridgeImpl const & other) = delete;
  ScPythonBridgeImpl & operator = (ScPythonBridgeImpl const & other) = delete;

  ScPythonBridgeImpl() {}
  ~ScPythonBridgeImpl() {}

  virtual MemoryBufferSafePtr SendEventToPython(std::string const & eventName, MemoryBufferSafePtr const & data)
  {
    bp::object pyData(bp::handle<>(PyMemoryView_FromMemory((char*)data->Data(), data->Size(), PyBUF_READ)));
    bp::object result = bp::call<bp::object>(m_pythonFunc.ptr(), "onEvent", bp::object(eventName), pyData);

    MemoryBufferSafePtr resultBuffer;

    PyObject * pyResult = result.ptr();
    if (pyResult && PyMemoryView_Check(pyResult))
    {
      Py_buffer * buffer = PyMemoryView_GET_BUFFER(pyResult);
      resultBuffer = MemoryBufferSafePtr(new MemoryBufferSafe((char*)buffer->buf, buffer->len));
    }

    return resultBuffer;
  }

  void SetOnEvent(bp::object & func)
  {
    m_pythonFunc = func;
  }

protected:
  bp::object m_pythonFunc;
};

class ScPythonMemoryModule
{
public:
  static void Initialize();
};

} // py
