/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/python/sc_python_interp.hpp"
#include "sc-memory/cpp/python/sc_python_service.hpp"

#include "test_defines.hpp"

UNIT_TEST(Python_interp)
{
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);
  
  SUBTEST_START("common")
  {
    py::DummyService testService("sc_test.py");
    testService.Run();

    while (testService.IsRun())
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

    testService.Stop();
  }
  SUBTEST_END()
}
