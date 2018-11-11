/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "../sc_types.hpp"
#include "../sc_memory.hpp"
#include "../sc_debug.hpp"
#include "../sc_utils.hpp"

#include "../utils/sc_console.hpp"

#include <cstdlib>
#include <iostream>

namespace test
{

class ScTestUnit final
{
public:
  _SC_EXTERN ScTestUnit(char const * name, char const * filename, void(*fn)());
  _SC_EXTERN ~ScTestUnit();

  void Run(std::string const & configPath, std::string const & extPath);

  static _SC_EXTERN void RunAll(std::string const & configPath = "sc-memory.ini", std::string const & extPath = "");

  static _SC_EXTERN void NotifySubTest();

protected:
  void ShutdownMemory(bool save);
  void InitMemory(std::string const & configPath, std::string const & extPath);

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
};

#define UNIT_TEST(__name) \
  void Test_##__name(); \
  ::test::ScTestUnit g_test_unit_##__name(#__name, __FILE__, &Test_##__name); \
  void Test_##__name()

#define _STATUS_COLOR(_expr) ((_expr) ? ScConsole::Color::Green : ScConsole::Color::Red)
#define _STATUS(_expr)  ((_expr) ? "ok" : "fail")

#define _TEST_IMPL(_check, _expr, _msg) \
{ \
  bool const _v = _expr; \
  std::cout << #_expr << "... "; \
  _check(_expr, _msg); \
  ScConsole::SetColor(STATUS_COLOR(_v)); \
  std::cout << _STATUS(_v) << std::endl; \
  ScConsole::ResetColor(); \
}

#define TEST(_expr, _msg) _TEST_IMPL(CHECK, _expr, _msg)
#define TEST_NOT(_expr, _msg) _TEST_IMPL(CHECK, !_expr, _msg)

#define SUBTEST_START(_name) { SC_LOG_INFO("Test "#_name" ..."); test::ScTestUnit::NotifySubTest(); }
#define SUBTEST_END() SC_LOG_INFO_COLOR(" ok", _STATUS_COLOR(true))

#ifdef SC_BUILD_AUTO_TESTS
# define _WAIT_KEY_IMPL()
#else
# define _WAIT_KEY_IMPL() ScConsole::WaitAnyKey("Press a key to continue...");
#endif

#define SC_AUTOMATION_TESTS(__name) \
int main(int argc, char ** argv) try \
{ \
  utils::ScLog::GetInstance()->Initialize(__name".log"); \
  test::ScTestUnit::RunAll(); \
  utils::ScLog::GetInstance()->Shutdown(); \
  _WAIT_KEY_IMPL() \
  return 0; \
} \
catch (utils::ScException const & ex) \
{ \
  SC_LOG_ERROR(ex.Description()); \
  _WAIT_KEY_IMPL() \
}

} // namespace test
