/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_test.hpp"

namespace test
{

std::set<ScTestUnit*, ScTestUnit::TestLess> ScTestUnit::ms_tests;
uint32_t ScTestUnit::ms_subtestsNum = 0;

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

void ScTestUnit::Run(std::string const & configPath, std::string const & extPath)
{
  SC_LOG_INFO("Run test " << m_name);

  InitMemory(configPath, extPath);

  if (m_fn)
    m_fn();

  ShutdownMemory(false);
  SC_LOG_INFO_COLOR("Test " << m_name << " complete", ScConsole::Color::LightGreen);
}

void ScTestUnit::InitMemory(std::string const & configPath, std::string const & extPath)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.config_file = configPath.c_str();
  params.ext_path = extPath.empty() ? nullptr : extPath.c_str();

  ScMemory::LogMute();
  ScMemory::Initialize(params);
  ScMemory::LogUnmute();
}

void ScTestUnit::ShutdownMemory(bool save)
{
  ScMemory::LogMute();
  ScMemory::Shutdown(save);
  ScMemory::LogUnmute();
}

void ScTestUnit::RunAll(std::string const & configPath, std::string const & extPath)
{
  SC_LOG_INFO("Run " << ms_tests.size() << " tests");
  for (ScTestUnit * unit : ms_tests)
    unit->Run(configPath, extPath);

  SC_LOG_INFO_COLOR("Passed " << ms_subtestsNum << " subtests in " << ms_tests.size() << " tests", ScConsole::Color::LightGreen);
}

void ScTestUnit::NotifySubTest()
{
  ++ms_subtestsNum;
}


} // namespace test
