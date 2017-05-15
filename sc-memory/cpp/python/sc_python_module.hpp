#pragma once

#include "sc_python_gil.hpp"

#include "../sc_utils.hpp"
#include "../sc_wait.hpp"

namespace py
{

namespace bp = boost::python;

class ScPythonMemoryModule
{
public:
  static void Initialize();
};

} // py
