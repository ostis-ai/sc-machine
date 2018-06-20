/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/python/sc_python_interp.hpp"
#include "sc-memory/cpp/python/sc_python_service.hpp"

#include "test_defines.hpp"

#include <thread>

UNIT_TEST(Python_interp)
{
  SUBTEST_START("common")
  {
    py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

    py::DummyService testService("sc_tests/test_main.py");
    testService.Run();

    while (testService.IsRun())
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

    testService.Stop();
  }
  SUBTEST_END()
}


UNIT_TEST(Python_clean)
{
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

  volatile bool passed = true;

  std::vector<std::unique_ptr<std::thread>> threads;

  size_t const numTests = 50;
  threads.resize(numTests);
  for (size_t i = 0; i < numTests; ++i) {
    threads[i].reset(new std::thread([&passed]()
    {
      py::DummyService testService("sc_tests/test_dummy.py");
      try
      {
        testService.Run();
      }
      catch (utils::ScException const & ex)
      {
        SC_LOG_ERROR(ex.Message());
        passed = false;
      }
      catch (...)
      {
        SC_LOG_ERROR("Unknown error");
        passed = false;
      }
    }));
  }

  for (auto const & t : threads)
    t->join();
  //std::this_thread::sleep_for(std::chrono::seconds(10));
  SC_CHECK(passed, ());
}