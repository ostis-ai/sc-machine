/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <memory>
#include "sc-memory/sc_memory.hpp"
#include "sc-core/sc_memory.h"

class TestMemory
{
public:
  void Initialize(size_t objectsNum = 0)
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);
    params.clear = SC_TRUE;
    params.repo_path = "test_repo";

    ScMemory::LogMute();
    ScMemory::Initialize(params);

    InitContext();
    Setup(objectsNum);
  }

  void Shutdown()
  {
    m_ctx.reset();

    ScMemory::Shutdown(false);
  }

  void InitContext()
  {
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "test");
  }

  void DestroyContext()
  {
    m_ctx.reset();
  }

  bool HasContext() const
  {
    return static_cast<bool>(m_ctx);
  }

  virtual void Setup(size_t objectsNum) {}

protected:
  std::unique_ptr<ScMemoryContext> m_ctx {};
};
