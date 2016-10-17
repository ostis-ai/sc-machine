/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "test.hpp"

#include "../wrap/sc_utils.hpp"
#include "../wrap/sc_debug.hpp"

#include <conio.h>
#include <iostream>

#define WAIT_KEY() \
{ \
	std::cout << "Press any key to exit..." << std::endl; \
	getch(); \
}

int main(int argc, char ** argv) try
{
	utils::ScLog::GetInstance()->Initialize("test_wrap.log");
	test::TestUnit::RunAll();
	utils::ScLog::GetInstance()->Shutdown();
	
	WAIT_KEY();

	return 0;
}
catch (ScException const & ex)
{
	SC_LOG_ERROR(ex.what());
}