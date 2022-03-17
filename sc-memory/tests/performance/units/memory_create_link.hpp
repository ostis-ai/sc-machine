/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

#include "sc-memory/sc_link.hpp"

class TestCreateLink : public TestMemory
{
public:
  void Run()
  {
    ScAddr const addr = m_ctx->CreateLink();

    ScLink link(*m_ctx, addr);
    link.Set(addr.Hash());
  }
};
