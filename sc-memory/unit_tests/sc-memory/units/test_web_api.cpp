#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/python/sc_python_interp.hpp"
#include "sc-memory/python/sc_python_service.hpp"

#include "test_defines.hpp"

UNIT_TEST(WebAPI)
{
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

  SUBTEST_START("ws_json")
  {
    py::DummyService testService("http_api/test/test_web_api.py");
    testService.Run();

    while (testService.IsRun())
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

    testService.Stop();
  }
  SUBTEST_END()
}
