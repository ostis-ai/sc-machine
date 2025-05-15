#pragma once

#include "../memory_test.hpp"

class TestGenerateNode : public TestMemory
{
public:
  void Run()
  {
    m_ctx->GenerateNode(ScType::ConstNode);
  }
};