/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_scs_helper.hpp>

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

  bool operator()(ScTemplateItem param1, ScTemplateItem param2, ScTemplateItem param3)
  {
    bool catched = false;
    try
    {
      ScTemplate testTempl;
      testTempl(param1, param2, param3);

      ScTemplateGenResult res;
      m_ctx.GenerateByTemplate(testTempl, res);
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
  ScAddr const addr = ctx.ResolveElementSystemIdentifier(idtf, ScType::ConstNode);
  EXPECT_TRUE(addr.IsValid());
  return addr;
}

class DummyFileInterface : public SCsFileInterface
{
protected:
  ScStreamPtr GetFileContent(std::string const &) override
  {
    return {};
  }
};

}  // namespace
