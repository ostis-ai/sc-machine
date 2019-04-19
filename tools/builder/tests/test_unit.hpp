#pragma once

#include "sc-memory/sc_memory.hpp"

#include "sc-memory/utils/sc_test.hpp"

class BuilderTestUnit : public test::ScTestUnit
{
public:
  BuilderTestUnit(char const * name, char const * filename, void(*fn)())
    : test::ScTestUnit(name, filename, fn)
  {
  }

private:
  
  void InitMemory(std::string const & configPath, std::string const& extPath) override
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_FALSE;
    params.repo_path = "sc-builder-test-repo";
    params.config_file = configPath.c_str();
    params.ext_path = extPath.empty() ? nullptr : extPath.c_str();

    ScMemory::LogMute();
    ScMemory::Initialize(params);
    ScMemory::LogUnmute();
  }

  void ShutdownMemory(bool save) override
  {
    ScMemory::LogMute();
    ScMemory::Shutdown(false);
    ScMemory::LogUnmute();
  }
};