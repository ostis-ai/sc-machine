/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_template.hpp>

#include <memory>

class TestTemplate
{
public:
  void Initialize(size_t constrCount)
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);
    params.clear = SC_TRUE;
    params.repo_path = "test_repo";

    ScMemory::LogMute();
    ScMemory::Initialize(params);

    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "test");

    Setup(constrCount);
  }

  void Shutdown()
  {
    m_ctx.reset();

    ScMemory::Shutdown(false);
  }

  virtual void Setup(size_t constrCount) = 0;
  bool Run()
  {
    ScTemplateSearchResult result;
    bool status = m_ctx->HelperSearchTemplate(m_templ, result);

    for (size_t i = 0; i < result.Size(); i++);

    return status;
  }

protected:
  std::unique_ptr<ScMemoryContext> m_ctx;
  ScTemplate m_templ;
};
