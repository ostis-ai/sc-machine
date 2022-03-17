/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

class TestCreateNode : public TestMemory
{
public:
  void Run()
  {
    m_ctx->CreateNode(ScType::NodeConstAbstract);
  }
};
