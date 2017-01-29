/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"

#include "sc-memory/cpp/sc_utils.hpp"
#include "sc-memory/cpp/sc_debug.hpp"

#include <cstdlib>
#include <iostream>

void _pause()
{
#if !SC_BUILD_AUTO_TESTS
#   if SC_PLATFORM_WIN32
  system("PAUSE");
#   else
  system("read -p \"Press a key to continue...\" -n 1 -s");
#   endif
#endif 
}

int main(int argc, char ** argv) try
{
  utils::ScLog::GetInstance()->Initialize("test_wrap.log");
  test::ScTestUnit::RunAll();
  utils::ScLog::GetInstance()->Shutdown();

  _pause();
  return 0;
}
catch (utils::ScException const & ex)
{
  SC_LOG_ERROR(ex.Message());
  _pause();
}
