/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/python/sc_python_interp.hpp"

#include "test_defines.hpp"

UNIT_TEST(Python_interp)
{
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);
  
  SUBTEST_START("common")
  {
    py::ScPythonBridgePtr bridge(new py::ScPythonBridge());
    py::ScPythonInterpreter::RunScript("sc_test.py", bridge);
  }
  SUBTEST_END()
}
