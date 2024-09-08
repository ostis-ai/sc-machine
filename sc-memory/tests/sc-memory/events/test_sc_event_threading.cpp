#include <gtest/gtest.h>

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_event_subscription.hpp"

#include "sc-memory/sc_timer.hpp"

#include "event_test_utils.hpp"

#include <atomic>
#include <cstdlib>
#include <ctime>
#include <thread>

TEST_F(ScEventTest, ThreadingSmoke)
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
    ScAddr const addr = m_ctx->GenerateNode(ScType::NodeConst);
    EXPECT_TRUE(addr.IsValid());
    nodes[i] = addr;
  }

  // create random events for each node
  std::vector<std::shared_ptr<ScEventSubscription>> eventSubscriptions;
  eventSubscriptions.resize(eventsNum);

  std::vector<ScAddr> eventTypes = {
      ScKeynodes::sc_event_after_generate_connector,
      ScKeynodes::sc_event_after_generate_incoming_arc,
      ScKeynodes::sc_event_after_generate_outgoing_arc,
      ScKeynodes::sc_event_after_generate_edge,
      ScKeynodes::sc_event_before_erase_connector,
      ScKeynodes::sc_event_before_erase_incoming_arc,
      ScKeynodes::sc_event_before_erase_outgoing_arc,
      ScKeynodes::sc_event_before_erase_edge,
      ScKeynodes::sc_event_before_erase_element,
      ScKeynodes::sc_event_before_change_link_content,
  };

  auto const randNode = [&nodes]()
  {
    return nodes[std::rand() % nodes.size()];
  };

  std::atomic_int evtCount = {0};

  for (size_t i = 0; i < eventsNum; ++i)
  {
    eventSubscriptions[i] = m_ctx->GenerateElementaryEventSubscription(
        eventTypes[std::rand() % (eventTypes.size() - 1)],  // ignore ChangeContent event
        randNode(),
        [&](ScElementaryEvent const &)
        {
          evtCount++;
        });
  }

  ScTimer timer;

  std::atomic_int createEdgeCount = {0};
  std::atomic_int eraseNodeCount = {0};

  std::vector<ScAddr> connectors;
  connectors.reserve(testCount);
  for (size_t i = 0; i < testCount; ++i)
  {
    uint32_t const v = rand() % 2;
    if (v == 0)
    {
      ScAddr const arcAddr = m_ctx->GenerateConnector(ScType::EdgeAccess, randNode(), randNode());
      EXPECT_TRUE(arcAddr.IsValid());
      connectors.push_back(arcAddr);

      createEdgeCount++;
    }
    else if (v == 1)  // will also erase connectors
    {
      if (nodes.size() > 2)
      {
        size_t const idx = std::rand() % nodes.size();
        ScAddr const addr = nodes[idx];
        nodes.erase(nodes.begin() + idx);

        m_ctx->EraseElement(addr);
        eraseNodeCount++;
      }
    }
  }

  eventSubscriptions.clear();
}
