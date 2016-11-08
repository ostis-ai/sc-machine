/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "test.hpp"

#include "sc-memory/cpp/sc_utils.hpp"
#include "sc-memory/cpp/sc_debug.hpp"

#include <iostream>

int main(int argc, char ** argv) try
{
	utils::ScLog::GetInstance()->Initialize("test_wrap.log");
	test::TestUnit::RunAll();
	utils::ScLog::GetInstance()->Shutdown();

	return 0;
}
catch (ScException const & ex)
{
	SC_LOG_ERROR(ex.what());
}