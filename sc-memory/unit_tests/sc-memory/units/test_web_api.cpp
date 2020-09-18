/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/python/sc_python_interp.hpp"
#include "sc-memory/python/sc_python_service.hpp"
#include "test_defines.hpp"

TEST_CASE("WebAPI", "[test web api]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

  SECTION("ws_json")
  {
    SUBTEST_START("ws_json")
    {
      try
      {
        py::DummyService testService("http_api/test/test_web_api.py");
        testService.Run();

        while (testService.IsRun())
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        testService.Stop();
      } catch (...)
      {
        SC_LOG_ERROR("Test \"ws_json\" failed")
      }
    }
    SUBTEST_END()
  }

  test::ScTestUnit::ShutdownMemory(false);
}
