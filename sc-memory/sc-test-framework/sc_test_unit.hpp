/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <atomic>
#include <cstdlib>
#include <iostream>

#include "sc-memory/sc_debug.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_utils.hpp"
#include "sc-memory/utils/sc_console.hpp"

namespace test
{

class ScTestUnit
{
public:
  _SC_EXTERN ScTestUnit(char const * name, char const * filename, void(*fn)());
  _SC_EXTERN virtual ~ScTestUnit();

  void Run(std::string const & configPath, std::string const & extPath);

  static _SC_EXTERN int RunAll(
        int argc,
        char * argv[],
        std::string const & configPath = "sc-memory.ini",
        std::string const & extPath = "");

  static void ShutdownMemory(bool save);

  static void InitMemory(std::string const & configPath, std::string const & extPath);

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

#define UNIT_TEST_CUSTOM(__name, _class) \
  void Test_##__name(); \
  _class g_test_unit_##__name(#__name, __FILE__, &Test_##__name); \
  void Test_##__name()

#define SC_TEST_STATUS_COLOR(_expr) ((_expr) ? ScConsole::Color::Green : ScConsole::Color::Red)
#if SC_PLATFORM_WIN32
# define SC_TEST_STATUS(_expr)  ((_expr) ? "ok" : "fail")
#else
# define SC_TEST_STATUS(_expr)  ((_expr) ? "\u2713" : "\u274C")
#endif

#define SUBTEST_START(_name) SC_LOG_INFO("Subtest "#_name" started")
#define SUBTEST_END() SC_LOG_INFO("Subtest finished")

#ifdef SC_BUILD_AUTO_TESTS
# define SC_WAIT_KEY_IMPL()
#else
# define SC_WAIT_KEY_IMPL() ScConsole::WaitAnyKey("Press a key to continue...");
#endif

#define SC_AUTOMATION_TESTS(__name) \
int main(int argc, char ** argv) try \
{ \
  utils::ScLog::GetInstance()->SetFileName(__name".log"); \
  int result=test::ScTestUnit::RunAll(argc, argv); \
  utils::ScLog::GetInstance()->Shutdown(); \
  SC_WAIT_KEY_IMPL() \
  return result; \
} \
catch (utils::ScException const & ex) \
{ \
  ScMemory::LogUnmute(); \
  SC_LOG_ERROR(ex.Description()); \
  SC_WAIT_KEY_IMPL() \
}

} // namespace test
