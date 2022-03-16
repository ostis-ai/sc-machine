#include <gtest/gtest.h>

#include "sc-memory/python/sc_python_interp.hpp"
#include "sc-memory/python/sc_python_service.hpp"

#include "test_defines.hpp"
#include "sc_test.hpp"

#include <thread>

using ScPythonTest = ScMemoryTest;

TEST_F(ScPythonTest, python_unittest)
{
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

  py::DummyService testService("sc_tests/test_main.py");
  testService.Run();

  while (testService.IsRun())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  testService.Stop();
}

TEST_F(ScPythonTest, smoke)
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
                                         SC_LOG_ERROR(ex.Description());
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

  EXPECT_TRUE(passed);
}

TEST_F(ScPythonTest, web_api)
{
  py::ScPythonInterpreter::AddModulesPath(SC_TEST_KPM_PYTHON_PATH);

  py::DummyService testService("http_api/test/test_web_api.py");
  testService.Run();

  while (testService.IsRun())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  testService.Stop();
}