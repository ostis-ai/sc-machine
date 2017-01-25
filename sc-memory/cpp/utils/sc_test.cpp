/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_test.hpp"

namespace test
{

std::set<ScTestUnit*, ScTestUnit::TestLess> ScTestUnit::ms_tests;

ScTestUnit::ScTestUnit(char const * name, char const * filename, void(*fn)())
  : m_name(name)
  , m_filename(filename)
  , m_fn(fn)
{
  ms_tests.insert(this);
}

ScTestUnit::~ScTestUnit()
{
  ms_tests.erase(this);
}

void ScTestUnit::Run()
{
  SC_LOG_INFO("Run test " << m_name);

  InitMemory();

  if (m_fn)
    m_fn();

  ShutdownMemory(false);
  SC_LOG_INFO_COLOR("Test " << m_name << " complete", ScConsole::Color::LightGreen);
}

void ScTestUnit::InitMemory()
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

void ScTestUnit::ShutdownMemory(bool save)
{
  ScMemory::logMute();
  ScMemory::shutdown(save);
  ScMemory::logUnmute();
}

void ScTestUnit::RunAll()
{
  for (ScTestUnit * unit : ms_tests)
    unit->Run();
}


} // namespace test
