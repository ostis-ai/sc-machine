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
  static _SC_EXTERN int RunAll(
        int argc,
        char * argv[],
        std::string const & configPath = "sc-memory.ini",
        std::string const & extPath = "");

protected:
  void static ShutdownMemory(bool save);

  void static InitMemory(std::string const & configPath, std::string const & extPath);

protected:
  char const * m_name;
  char const * m_filename;

  void (* m_fn)();

private:

  struct TestLess
  {
    bool operator()(ScTestUnit const * a, ScTestUnit const * b) const
    {
      return strcmp(a->m_name, b->m_name) < 0;
    }
  };

  static _SC_EXTERN std::set<ScTestUnit *, TestLess> ms_tests;
};
#define _STATUS_COLOR(_expr) ((_expr) ? ScConsole::Color::Green : ScConsole::Color::Red)

#define SUBTEST_START(_name) SC_LOG_INFO("Test "#_name" ...")
#define SUBTEST_END() SC_LOG_INFO_COLOR(" ok", _STATUS_COLOR(true))

#ifdef SC_BUILD_AUTO_TESTS
# define _WAIT_KEY_IMPL()
#else
# define _WAIT_KEY_IMPL() ScConsole::WaitAnyKey("Press a key to continue...");
#endif

#define SC_AUTOMATION_TESTS(__name) \
int main(int argc, char ** argv) try \
{ \
  utils::ScLog::GetInstance()->SetFileName(__name".log"); \
  int result=test::ScTestUnit::RunAll(argc, argv); \
  utils::ScLog::GetInstance()->Shutdown(); \
  _WAIT_KEY_IMPL() \
  return result; \
} \
catch (utils::ScException const & ex) \
{ \
  SC_LOG_ERROR(ex.Message()); \
  _WAIT_KEY_IMPL() \
}

} // namespace test
