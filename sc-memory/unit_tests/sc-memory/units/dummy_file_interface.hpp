/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

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
