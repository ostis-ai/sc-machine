/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_event.hpp"
#include "sc-memory/cpp/sc_timer.hpp"
#include "sc-memory/cpp/sc_stream.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/utils/sc_progress.hpp"

#include <thread>
#include <mutex>

extern "C"
{
#include <glib.h>
}

namespace
{
const double kTestTimeout = 5.0;

template<typename EventClassT, typename PrepareF, typename EmitF>
void testEventsFuncT(ScMemoryContext & ctx, ScAddr const & addr, PrepareF prepare, EmitF emit)
{
  prepare();

  volatile bool isDone = false;
  auto const callback = [&isDone](ScAddr const & listenAddr,
      ScAddr const & edgeAddr,
      ScAddr const & otherAddr)
  {
    isDone = true;
    return true;
  };

  EventClassT evt(ctx, addr, callback);
  ScTimer timer(kTestTimeout);

  emit();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  SC_CHECK(isDone, ());
}
}



UNIT_TEST(events_common)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "events_common");

  SUBTEST_START(ScEventAddInputEdge)
  {
    ScAddr addr;
    auto const CreateNode = [&ctx, &addr]()
    {
      addr = ctx.CreateNode(ScType::Unknown);
      SC_CHECK(addr.IsValid(), ());
    };

    auto const emitEvent = [&ctx, &addr]()
    {
      ScAddr const addr2 = ctx.CreateNode(ScType::Unknown);
      SC_CHECK(addr2.IsValid(), ());

      ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccess, addr2, addr);
      SC_CHECK(edge.IsValid(), ());
    };

    testEventsFuncT<ScEventAddInputEdge>(ctx, addr, CreateNode, emitEvent);
  }
  SUBTEST_END()

  SUBTEST_START(ScEventAddOutputEdge)
  {
    ScAddr addr;
    auto const CreateNode = [&ctx, &addr]()
    {
      addr = ctx.CreateNode(ScType::Unknown);
      SC_CHECK(addr.IsValid(), ());
    };

    auto const emitEvent = [&ctx, &addr]()
    {
      ScAddr const addr2 = ctx.CreateNode(ScType::Unknown);
      SC_CHECK(addr2.IsValid(), ());

      ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccess, addr, addr2);
      SC_CHECK(edge.IsValid(), ());
    };

    testEventsFuncT<ScEventAddOutputEdge>(ctx, addr, CreateNode, emitEvent);
  }
  SUBTEST_END();

  SUBTEST_START(ScEventRemoveInputEdge)
  {
    ScAddr const addr = ctx.CreateNode(ScType::Unknown);
    SC_CHECK(addr.IsValid(), ());

    ScAddr const addr2 = ctx.CreateNode(ScType::Unknown);
    SC_CHECK(addr2.IsValid(), ());

    ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccess, addr, addr2);
    SC_CHECK(edge.IsValid(), ());

    auto const prepare = []() {};
    auto const emitEvent = [&ctx, &edge]()
    {
      SC_CHECK(ctx.EraseElement(edge), ());
    };

    testEventsFuncT<ScEventRemoveInputEdge>(ctx, addr2, prepare, emitEvent);
  }
  SUBTEST_END()

  SUBTEST_START(ScEventRemoveOutputEdge)
  {
    ScAddr const addr = ctx.CreateNode(ScType::Unknown);
    SC_CHECK(addr.IsValid(), ());

    ScAddr const addr2 = ctx.CreateNode(ScType::Unknown);
    SC_CHECK(addr2.IsValid(), ());

    ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccess, addr, addr2);
    SC_CHECK(edge.IsValid(), ());

    auto const prepare = []() {};
    auto const emitEvent = [&ctx, &edge]()
    {
      SC_CHECK(ctx.EraseElement(edge), ());
    };

    testEventsFuncT<ScEventRemoveOutputEdge>(ctx, addr, prepare, emitEvent);
  }
  SUBTEST_END()

  SUBTEST_START(ScEventContentChanged)
  {
    ScAddr const addr = ctx.CreateLink();
    SC_CHECK(addr.IsValid(), ());

    auto const prepare = []() {};
    auto const emitEvent = [&ctx, &addr]()
    {
      std::string const value("test");
      ScStream stream((sc_char*)value.data(), static_cast<sc_uint32>(value.size()), SC_STREAM_FLAG_READ);
      SC_CHECK(ctx.SetLinkContent(addr, stream), ());
    };

    testEventsFuncT<ScEventContentChanged>(ctx, addr, prepare, emitEvent);
  }
  SUBTEST_END()

  SUBTEST_START(ScEventEraseElement)
  {
    ScAddr const addr = ctx.CreateNode(ScType::Unknown);
    SC_CHECK(addr.IsValid(), ());

    auto const prepare = []() {};
    auto const emitEvent = [&ctx, &addr]()
    {
      SC_CHECK(ctx.EraseElement(addr), ());
    };

    testEventsFuncT<ScEventEraseElement>(ctx, addr, prepare, emitEvent);
  }
  SUBTEST_END()
}


UNIT_TEST(events_destroy_order)
{
  ScMemoryContext * ctx = new ScMemoryContext(sc_access_lvl_make_min, "events_destroy_order");

  ScAddr const node = ctx->CreateNode(ScType::Unknown);
  SC_CHECK(node.IsValid(), ());

  ScEventAddOutputEdge * evt = new ScEventAddOutputEdge(*ctx, node,
                                                        [](ScAddr const &, ScAddr const &, ScAddr const &)
  {
      return true;
  });

  delete ctx;

  // delete event after context
  delete evt;
}

UNIT_TEST(events_lock)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "events_lock");

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  ScAddr const node2 = ctx.CreateNode(ScType::NodeConst);

  ScEventAddOutputEdge evt(ctx, node,
                           [](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
    bool result = false;
    ScMemoryContext localCtx(sc_access_lvl_make_min);
    ScIterator3Ptr it = localCtx.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
    while (it->Next())
    {
      result = true;
    }

    return true;
  });

  for (size_t i = 0; i < 10000; i++)
  {
    ctx.CreateEdge(ScType::EdgeAccessConstPosPerm,
                   node, node2);
  }
}

UNIT_TEST(pend_events)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "pend_events");

  /* Main idea of test: create two sets with N elements, and add edges to relations.
   * Everytime, when event emits, we should check, that whole construction exist
   */
  ScAddr const set1 = ctx.CreateNode(ScType::NodeConstClass);
  ScAddr const rel = ctx.CreateNode(ScType::NodeConstNoRole);

  static const size_t el_num = 1 << 10;
  std::vector<ScAddr> elements(el_num);
  for (size_t i = 0; i < el_num; ++i)
  {
    ScAddr const a = ctx.CreateNode(ScType::NodeConst);
    SC_CHECK(a.IsValid(), ());
    elements[i] = a;
  }

  // create template for pending events check
  ScTemplate templ;
  for (auto const & a : elements)
  {
    templ.TripleWithRelation(
      set1,
      ScType::EdgeDCommonVar,
      a >> "_el",
      ScType::EdgeAccessVarPosPerm,
      rel);
  }

  volatile int32_t eventsCount = 0;
  volatile int32_t passedCount = 0;

  ScEventAddOutputEdge evt(ctx, set1,
    [&passedCount, &eventsCount, &set1, &elements, &rel](ScAddr const &, ScAddr const &, ScAddr const &)
  {
    std::shared_ptr<ScTemplate> checkTempl(new ScTemplate());
    size_t step = 100;
    size_t testNum = el_num / step - 1;
    for (size_t i = 0; i < testNum; ++i)
    {
      checkTempl->TripleWithRelation(
        set1,
        ScType::EdgeDCommonVar,
        elements[i * step] >> "_el",
        ScType::EdgeAccessVarPosPerm,
        rel);
    }

    ScMemoryContext localCtx(sc_access_lvl_make_min);

    ScTemplateSearchResult res;
    SC_CHECK(localCtx.HelperSearchTemplate(*checkTempl, res), ());

    if (res.Size() == 1)
      g_atomic_int_add(&passedCount, 1);

    g_atomic_int_add(&eventsCount, 1);

    return true;
  });

  ScTemplateGenResult genResult;
  SC_CHECK(ctx.HelperGenTemplate(templ, genResult), ());

  // wait all events
  while (g_atomic_int_get(&eventsCount) < el_num)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  SC_CHECK_EQUAL(passedCount, el_num, ());
}
