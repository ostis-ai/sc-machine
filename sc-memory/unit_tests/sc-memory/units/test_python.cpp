/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <thread>

#include <test_defines.hpp>
#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/python/sc_python_interp.hpp"
#include "sc-memory/python/sc_python_service.hpp"

TEST_CASE("Python_interp", "[test python]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  SECTION("common")
  {
    SUBTEST_START("common")
    {
      try
      {
        py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

        py::DummyService testService("sc_tests/test_main.py");
        testService.Run();

        while (testService.IsRun())
          std::this_thread::sleep_for(std::chrono::milliseconds(10));

        testService.Stop();
      } catch (...)
      {
        SC_LOG_ERROR("Test \"common\" failed")
      }
    }
    SUBTEST_END()
  }

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("Python_clean", "[test python]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  try
  {
    py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

    volatile bool passed = true;

    std::vector<std::unique_ptr<std::thread>> threads;

    size_t const numTests = 50;
    threads.resize(numTests);
    for (size_t i = 0; i < numTests; ++i)
    {
      threads[i].reset(
            new std::thread([&passed]()
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
    REQUIRE(passed);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"Python_clean\" failed")
  }

  test::ScTestUnit::ShutdownMemory(false);
}
