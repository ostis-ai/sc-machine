#include <gtest/gtest.h>

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_timer.hpp"

#include "event_test_utils.hpp"

#include <atomic>
#include <cstdlib>
#include <ctime>
#include <thread>

TEST_F(ScEventTest, threading_smoke)
{
  std::srand(unsigned(std::time(0)));

  size_t const nodeNum = 1000000;
  size_t const eventsNum = 250000;
  size_t const testCount = 50000;

  // generate N nodes
  std::vector<ScAddr> nodes;
  nodes.resize(nodeNum);

  for (size_t i = 0; i < nodeNum; ++i)
  {
    ScAddr const addr = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(addr.IsValid());
    nodes[i] = addr;
  }

  // create random events for each node
  std::vector<ScEvent*> events;
  events.resize(eventsNum);

  std::vector<ScEvent::Type> eventTypes =
        {
              ScEvent::Type::AddOutputEdge,
              ScEvent::Type::AddInputEdge,
              ScEvent::Type::RemoveOutputEdge,
              ScEvent::Type::RemoveInputEdge,
              ScEvent::Type::EraseElement,
              ScEvent::Type::ContentChanged
        };

  auto const randNode = [&nodes]()
  {
    return nodes[std::rand() % nodes.size()];
  };

  std::atomic_int evtCount = { 0 };

  for (size_t i = 0; i < eventsNum; ++i)
  {
    events[i] = new ScEvent(*m_ctx,
                            randNode(),
                            eventTypes[std::rand() % (eventTypes.size() - 1)], // ignore ContentChanged event
                            [&](ScAddr const &, ScAddr const &, ScAddr const &)
                            {
                              evtCount++;
                              return true;
                            });
  }

  ScTimer timer;

  std::atomic_int createEdgeCount = { 0 };
  std::atomic_int eraseNodeCount = { 0 };

  std::vector<ScAddr> edges;
  edges.reserve(testCount);
  for (size_t i = 0; i < testCount; ++i)
  {
    uint32_t const v = rand() % 2;
    if (v == 0)
    {
      ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, randNode(), randNode());
      EXPECT_TRUE(e.IsValid());
      edges.push_back(e);

      createEdgeCount++;
    }
    else if (v == 1) // will also erase edges
    {
      if (nodes.size() > 2)
      {
        size_t const idx = std::rand() % nodes.size();
        ScAddr const addr = nodes[idx];
        nodes.erase(nodes.begin() + idx);

        EXPECT_TRUE(m_ctx->EraseElement(addr));
        eraseNodeCount++;
      }
    }
  }

  for (auto e : events)
    delete e;

  events.clear();
}
