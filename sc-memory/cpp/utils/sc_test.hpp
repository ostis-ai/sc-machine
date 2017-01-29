/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "../sc_types.hpp"
#include "../sc_memory.hpp"
#include "../sc_debug.hpp"

#include <iostream>

namespace test
{

class ScTestUnit final
{
public:
  _SC_EXTERN ScTestUnit(char const * name, char const * filename, void(*fn)());
  _SC_EXTERN ~ScTestUnit();

  void Run();

  static _SC_EXTERN void RunAll();

protected:
  void ShutdownMemory(bool save);
  void InitMemory();

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

#define SUBTEST_START(_name) SC_LOG_INFO("Test "#_name" ...")
#define SUBTEST_END() SC_LOG_INFO_COLOR(" ok", _STATUS_COLOR(true))

} // namespace test
