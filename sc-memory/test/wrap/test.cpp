/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "test.hpp"

namespace test
{

std::set<TestUnit*> TestUnit::ms_tests;

TestUnit::TestUnit(char const * name, char const * filename, void(*fn)())
	: m_name(name)
	, m_filename(filename)
	, m_fn(fn)
{
	ms_tests.insert(this);
}

TestUnit::~TestUnit()
{
	ms_tests.erase(this);
}

void TestUnit::Run()
{
	SC_LOG_INFO("Run test " << m_name);
	
	InitMemory();

	if (m_fn)
		m_fn();

	ShutdownMemory(false);
	SC_LOG_INFO("Test " << m_name << " complete");
}

void TestUnit::InitMemory()
{
	sc_memory_params params;
	sc_memory_params_clear(&params);

	params.clear = SC_TRUE;
	params.repo_path = "repo";
	params.config_file = "sc-memory.ini";
	params.ext_path = 0;

	ScMemory::logMute();
	ScMemory::initialize(params);
	ScMemory::logUnmute();
}

void TestUnit::ShutdownMemory(bool save)
{
	ScMemory::logMute();
	ScMemory::shutdown(save);
	ScMemory::logUnmute();
}

void TestUnit::RunAll()
{
	for (TestUnit * unit : ms_tests)
		unit->Run();
}


} // namespace test
