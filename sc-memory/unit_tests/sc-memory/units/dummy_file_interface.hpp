#pragma once

#include "sc-memory/sc_scs_helper.hpp"

class DummyFileInterface : public SCsFileInterface
{
public:
  ~DummyFileInterface() override {}

  ScStreamPtr GetFileContent(std::string const &) override
  {
    return ScStreamPtr();
  }
};
