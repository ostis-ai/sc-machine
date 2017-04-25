/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/python/sc_python_interp.hpp"

#include "test_defines.hpp"

UNIT_TEST(Python_dummy)
{
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);
  py::ScPythonInterpreter::RunScript("dummy.py");
  py::ScPythonInterpreter::RunScript("test.py");
}
