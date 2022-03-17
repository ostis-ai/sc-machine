#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include "sc_test.hpp"

#include <gtest/gtest.h>

#include <algorithm>

using ScTemplateTest = ScMemoryTest;

namespace
{

struct TestTemplParams
{
  explicit TestTemplParams(ScMemoryContext & ctx)
    : m_ctx(ctx)
  {
  }

  bool operator () (ScTemplateItemValue param1, ScTemplateItemValue param2, ScTemplateItemValue param3)
  {
    bool catched = false;
    try
    {
      ScTemplate testTempl;
      testTempl(param1, param2, param3);

      ScTemplateGenResult res;
      EXPECT_TRUE(m_ctx.HelperGenTemplate(testTempl, res));
    }
    catch (utils::ExceptionInvalidParams & e)
    {
      (void)e;
      catched = true;
    }

    return !catched;
  }

private:
  ScMemoryContext & m_ctx;
};


inline bool HasAddr(ScAddrVector const & v, ScAddr const & addr)
{
  return std::find(v.begin(), v.end(), addr) != v.end();
}

inline ScAddr ResolveKeynode(ScMemoryContext & ctx, std::string const & idtf)
{
  ScAddr const addr = ctx.HelperResolveSystemIdtf(idtf, ScType::NodeConst);
  EXPECT_TRUE(addr.IsValid());
  return addr;
}

class DummyFileInterface : public SCsFileInterface
{
protected:
  ScStreamPtr GetFileContent(std::string const & fileURL) override
  {
    return {};
  }
};

} // namespace
