/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "../sc_debug.hpp"
#include "../sc_memory.hpp"
#include "../sc_utils.hpp"

#include "../utils/sc_console.hpp"

#include <atomic>
#include <cstdlib>
#include <iostream>

namespace test
{

class ScTestUnit
{
public:
  _SC_EXTERN ScTestUnit(char const * name, char const * filename, void(*fn)());
  _SC_EXTERN virtual ~ScTestUnit();

  void Run(std::string const & configPath, std::string const & extPath);

  static _SC_EXTERN void RunAll(std::string const & configPath = "", std::string const & extPath = "");

  static _SC_EXTERN void NotifySubTest();

protected:
  virtual void ShutdownMemory(bool save);
  virtual void InitMemory(std::string const & configPath, std::string const & extPath);

protected:
  char const * m_name;
  char const * m_filename;
  void(*m_fn)();

private:

  struct TestLess
  {
    bool operator() (ScTestUnit const * a, ScTestUnit const * b) const
    {
      return strcmp(a->m_name, b->m_name) < 0;
    }
  };

  static _SC_EXTERN std::set<ScTestUnit*, TestLess> ms_tests;
  static _SC_EXTERN uint32_t ms_subtestsNum;
  static std::atomic_bool ms_isRun;
};

#define UNIT_TEST_CUSTOM(__name, _class) \
  void Test_##__name(); \
  _class g_test_unit_##__name(#__name, __FILE__, &Test_##__name); \
  void Test_##__name()

#define UNIT_TEST(__name) UNIT_TEST_CUSTOM(__name, ::test::ScTestUnit)
 
#define SC_TEST_STATUS_COLOR(_expr) ((_expr) ? ScConsole::Color::Green : ScConsole::Color::Red)
#if SC_PLATFORM_WIN32
# define SC_TEST_STATUS(_expr)  ((_expr) ? "ok" : "fail")
#else
# define SC_TEST_STATUS(_expr)  ((_expr) ? "\u2713" : "\u274C")
#endif

#define SC_TEST_IMPL(_check, _expr, _msg) \
{ \
  bool const _v = _expr; \
  std::cout << #_expr << "... "; \
  _check(_expr, _msg); \
  ScConsole::SetColor(SC_TEST_STATUS_COLOR(_v)); \
  std::cout << SC_TEST_STATUS(_v) << std::endl; \
  ScConsole::ResetColor(); \
}

#define TEST(_expr, _msg) SC_TEST_IMPL(CHECK, _expr, _msg)
#define TEST_NOT(_expr, _msg) SC_TEST_IMPL(CHECK, !_expr, _msg)

#define SUBTEST_START(_name) \
{ \
  ScConsole::SetColor(ScConsole::Color::Grey); \
  std::cout << std::endl << "\t" << #_name; \
  ScConsole::ResetColor(); \
  std::cout << " ... " << std::flush; \
  test::ScTestUnit::NotifySubTest(); \
};
#define SUBTEST_END() \
{ \
  ScConsole::SetColor(SC_TEST_STATUS_COLOR(true)); \
  std::cout << SC_TEST_STATUS(true) << std::flush; \
};

#ifdef SC_BUILD_AUTO_TESTS
# define SC_WAIT_KEY_IMPL()
#else
# define SC_WAIT_KEY_IMPL() ScConsole::WaitAnyKey("Press a key to continue...");
#endif

#define SC_AUTOMATION_TESTS(__name) \
int main(int argc, char ** argv) try \
{ \
  utils::ScLog::GetInstance()->Initialize(__name".log"); \
  test::ScTestUnit::RunAll(); \
  utils::ScLog::GetInstance()->Shutdown(); \
  SC_WAIT_KEY_IMPL() \
  return 0; \
} \
catch (utils::ScException const & ex) \
{ \
  ScMemory::LogUnmute(); \
  SC_LOG_ERROR(ex.Description()); \
  SC_WAIT_KEY_IMPL() \
}

} // namespace test
