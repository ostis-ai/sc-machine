/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#define CATCH_CONFIG_RUNNER

#include "catch2/catch.hpp"

#include "sc_test.hpp"

namespace test
{
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

int ScTestUnit::RunAll(int argc, char * argv[], std::string const & configPath, std::string const & extPath)
{
  Catch::Session session;

  int returnCode = session.applyCommandLine(argc, argv);
  if (returnCode != 0)
    return returnCode;

  int failed = session.run();

  return failed;
}


} // namespace test
