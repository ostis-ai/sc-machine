#include <gtest/gtest.h>

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_timer.hpp"

#include "event_test_utils.hpp"

#include <atomic>
#include <thread>
#include <mutex>

TEST(ScEventQueueTest, EventsQueueDestroy)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);
  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  ScMemory::Initialize(params);

  ScMemoryContext ctx;

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  ScAddr const node2 = ctx.CreateNode(ScType::NodeConst);
  ScAddr const node3 = ctx.CreateNode(ScType::NodeConst);

  size_t count = 10;

  ScEventAddOutputEdge evt(
      ctx,
      node,
      [node, node2, count](ScAddr const & addr, ScAddr const &, ScAddr const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);

        for (size_t i = 0; i < count; ++i)
          localCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, node2, node);
        return result;
      });

  ScEventAddOutputEdge evt2(
      ctx,
      node2,
      [node3, node2, count](ScAddr const & addr, ScAddr const &, ScAddr const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);
        for (size_t i = 0; i < count; ++i)
          localCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, node3, node2);

        return result;
      });

  ScEventAddOutputEdge evt3(
      ctx,
      node3,
      [](ScAddr const & addr, ScAddr const &, ScAddr const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);

        return result;
      });

  for (size_t i = 0; i < count; ++i)
    ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);

  sleep(5);

  ctx.Destroy();
  ScMemory::Shutdown();
}

namespace
{
double const kTestTimeout = 5.0;

template <typename EventClassT, typename PrepareF, typename EmitF>
void testEventsFuncT(ScMemoryContext & ctx, ScAddr const & addr, PrepareF prepare, EmitF emit)
{
  prepare();

  bool volatile isDone = false;
  auto const callback = [&isDone](ScAddr const &, ScAddr const &, ScAddr const &)
  {
    isDone = true;
    return true;
  };

  EventClassT evt(ctx, addr, callback);
  ScTimer timer(kTestTimeout);

  emit();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

}  // namespace

TEST_F(ScEventTest, AddInputEdge)
{
  ScAddr addr;
  auto const CreateNode = [this, &addr]()
  {
    addr = m_ctx->CreateNode(ScType::Unknown);
    EXPECT_TRUE(addr.IsValid());
  };

  auto const emitEvent = [this, &addr]()
  {
    ScAddr const addr2 = m_ctx->CreateNode(ScType::Unknown);
    EXPECT_TRUE(addr2.IsValid());

    ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccess, addr2, addr);
    EXPECT_TRUE(edge.IsValid());
  };

  testEventsFuncT<ScEventAddInputEdge>(*m_ctx, addr, CreateNode, emitEvent);
}

TEST_F(ScEventTest, AddOutputEdge)
{
  ScAddr addr;
  auto const CreateNode = [this, &addr]()
  {
    addr = m_ctx->CreateNode(ScType::Unknown);
    EXPECT_TRUE(addr.IsValid());
  };

  auto const emitEvent = [this, &addr]()
  {
    ScAddr const addr2 = m_ctx->CreateNode(ScType::Unknown);
    EXPECT_TRUE(addr2.IsValid());

    ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccess, addr, addr2);
    EXPECT_TRUE(edge.IsValid());
  };

  testEventsFuncT<ScEventAddOutputEdge>(*m_ctx, addr, CreateNode, emitEvent);
}

TEST_F(ScEventTest, RemoveInputEdge)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::Unknown);
  EXPECT_TRUE(addr.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::Unknown);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccess, addr, addr2);
  EXPECT_TRUE(edge.IsValid());

  auto const prepare = []() {};
  auto const emitEvent = [this, &edge]()
  {
    EXPECT_TRUE(m_ctx->EraseElement(edge));
  };

  testEventsFuncT<ScEventRemoveInputEdge>(*m_ctx, addr2, prepare, emitEvent);
}

TEST_F(ScEventTest, RemoveOutputEdge)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::Unknown);
  EXPECT_TRUE(addr.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::Unknown);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccess, addr, addr2);
  EXPECT_TRUE(edge.IsValid());

  auto const prepare = []() {};
  auto const emitEvent = [this, &edge]()
  {
    EXPECT_TRUE(m_ctx->EraseElement(edge));
  };

  testEventsFuncT<ScEventRemoveOutputEdge>(*m_ctx, addr, prepare, emitEvent);
}

TEST_F(ScEventTest, ContentChanged)
{
  ScAddr const addr = m_ctx->CreateLink();
  EXPECT_TRUE(addr.IsValid());

  auto const prepare = []() {};
  auto const emitEvent = [this, &addr]()
  {
    std::string const value("test");
    ScStreamPtr stream = ScStreamMakeRead(value);
    EXPECT_TRUE(m_ctx->SetLinkContent(addr, stream));
  };

  testEventsFuncT<ScEventContentChanged>(*m_ctx, addr, prepare, emitEvent);
}

TEST_F(ScEventTest, EraseElement)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::Unknown);
  EXPECT_TRUE(addr.IsValid());

  auto const prepare = []() {};
  auto const emitEvent = [this, &addr]()
  {
    EXPECT_TRUE(m_ctx->EraseElement(addr));
  };

  testEventsFuncT<ScEventEraseElement>(*m_ctx, addr, prepare, emitEvent);
}

TEST_F(ScEventTest, destroy_order)
{
  ScAddr const node = m_ctx->CreateNode(ScType::Unknown);
  EXPECT_TRUE(node.IsValid());

  auto * evt = new ScEventAddOutputEdge(
      *m_ctx,
      node,
      [](ScAddr const &, ScAddr const &, ScAddr const &)
      {
        return true;
      });

  m_ctx.reset();

  // delete event after context
  delete evt;
}

TEST_F(ScEventTest, events_lock)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventAddOutputEdge evt(
      *m_ctx,
      node,
      [](ScAddr const & addr, ScAddr const &, ScAddr const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);

        return result;
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, parallel_create_edges)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventAddOutputEdge evt(
      *m_ctx,
      node,
      [](ScAddr const & addr, ScAddr const &, ScAddr const & target)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        for (size_t i = 0; i < 10000; i++)
          localCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr, target);

        EXPECT_TRUE(result);

        return result;
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, parallel_create_remove_edges)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventAddOutputEdge evt(
      *m_ctx,
      node,
      [](ScAddr const & addr, ScAddr const &, ScAddr const & target)
      {
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          localCtx.EraseElement(it->Get(1));

        return SC_RESULT_OK;
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, parallel_create_remove_edges2)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventAddOutputEdge evt(
      *m_ctx,
      node,
      [](ScAddr const & addr, ScAddr const & edge, ScAddr const & target)
      {
        ScMemoryContext localCtx;
        localCtx.EraseElement(edge);

        return SC_RESULT_OK;
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, pend_events)
{
  /* Main idea of test: create two sets with N elements, and add edges to relations.
   * Everytime, when event emits, we should check, that whole construction exist
   */
  ScAddr const set1 = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const rel = m_ctx->CreateNode(ScType::NodeConstNoRole);

  static const size_t el_num = 1 << 10;
  std::vector<ScAddr> elements(el_num);
  for (size_t i = 0; i < el_num; ++i)
  {
    ScAddr const a = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(a.IsValid());
    elements[i] = a;
  }

  // create template for pending events check
  ScTemplate templ;
  for (auto const & a : elements)
  {
    templ.Quintuple(set1, ScType::EdgeDCommonVar, a >> "_el", ScType::EdgeAccessVarPosPerm, rel);
  }

  std::atomic_uint eventsCount(0);
  std::atomic_uint passedCount(0);

  ScEventAddOutputEdge evt(
      *m_ctx,
      set1,
      [&passedCount, &eventsCount, &set1, &elements, &rel](ScAddr const &, ScAddr const &, ScAddr const &)
      {
        std::shared_ptr<ScTemplate> checkTempl(new ScTemplate());
        size_t step = 100;
        size_t testNum = el_num / step - 1;
        for (size_t i = 0; i < testNum; ++i)
        {
          checkTempl->Quintuple(
              set1, ScType::EdgeDCommonVar, elements[i * step] >> "_el", ScType::EdgeAccessVarPosPerm, rel);
        }

        ScMemoryContext localCtx;
        EXPECT_TRUE(localCtx.IsValid());

        ScTemplateSearchResult res;
        EXPECT_TRUE(localCtx.HelperSearchTemplate(*checkTempl, res));

        if (res.Size() == 1)
          passedCount.fetch_add(1);

        eventsCount.fetch_add(1);

        return true;
      });

  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));

  // wait all events
  while (eventsCount.load() < el_num)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  EXPECT_EQ(passedCount, el_num);
}

TEST_F(ScEventTest, BlockEvents)
{
  ScAddr const nodeAddr = m_ctx->CreateNode(ScType::NodeConst);

  std::atomic_bool isCalled = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      nodeAddr,
      [&isCalled](ScAddr const & addr, ScAddr const &, ScAddr const & target)
      {
        return isCalled = true;
      });

  m_ctx->BeingEventsBlocking();

  ScAddr const nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

  m_ctx->EndEventsBlocking();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);
}

TEST_F(ScEventTest, BlockEventsGuard)
{
  ScAddr const nodeAddr = m_ctx->CreateNode(ScType::NodeConst);

  std::atomic_bool isCalled = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      nodeAddr,
      [&isCalled](ScAddr const & addr, ScAddr const &, ScAddr const & target)
      {
        return isCalled = true;
      });

  ScMemoryContextEventsBlockingGuard guard(*m_ctx);

  ScAddr const nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);
}
