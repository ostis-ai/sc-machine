/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"
#include "sc-memory/cpp/sc_wait.hpp"
#include "sc-memory/cpp/sc_timer.hpp"
#include "sc-memory/cpp/utils/sc_progress.hpp"

#include <thread>
#include <cstdlib>
#include <ctime>

UNIT_TEST(events_threading)
{
  std::srand(unsigned(std::time(0)));

  ScMemoryContext ctx(sc_access_lvl_make_min, "events_threading");

  size_t const nodeNum = 1000;
  size_t const eventsNum = 2500;
  size_t const testCount = 500;

  // generate N nodes
  std::vector<ScAddr> nodes;
  nodes.resize(nodeNum);

  for (size_t i = 0; i < nodeNum; ++i)
  {
    ScAddr const addr = ctx.createNode(*ScType::NODE_CONST);
    SC_CHECK(addr.isValid(), ());
    nodes[i] = addr;
  }

  // create random events for each node
  std::vector<ScEvent*> events;
  events.resize(eventsNum);

  std::vector<ScEvent::Type> eventTypes =
  {
    ScEvent::AddOutputEdge,
    ScEvent::AddInputEdge,
    ScEvent::RemoveOutputEdge,
    ScEvent::RemoveInputEdge,
    ScEvent::EraseElement,
    ScEvent::ContentChanged
  };

  auto const randNode = [&nodes]()
  {
    return nodes[std::rand() % nodes.size()];
  };

  size_t evtCount = 0;

  for (size_t i = 0; i < eventsNum; ++i)
  {
    events[i] = new ScEvent(ctx,
                            randNode(),
                            eventTypes[std::rand() % (eventTypes.size() - 1)], // ignore ContentChanged event
        [&](ScAddr const &, ScAddr const &, ScAddr const &)
    {
      evtCount++;
      return true;
    });
  }

  ScTimer timer;

  size_t createEdgeCount = 0;
  size_t eraseNodeCount = 0;

  utils::ScProgress progress("Run tests", testCount);
  std::vector<ScAddr> edges;
  edges.reserve(testCount);
  for (size_t i = 0; i < testCount; ++i)
  {
    uint32_t const v = rand() % 2;
    if (v == 0)
    {
      ScAddr const e = ctx.createEdge(*ScType::EDGE_ACCESS, randNode(), randNode());
      SC_CHECK(e.isValid(), ());
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

        SC_CHECK(ctx.eraseElement(addr), ());
        eraseNodeCount++;
      }
    }

    progress.PrintStatus(i);
  }

  for (auto e : events)
    delete e;

  events.clear();

  SC_LOG_INFO("Event counter: " << evtCount);
  SC_LOG_INFO("Events per second: " << ((float)evtCount / timer.Seconds()));
  SC_LOG_INFO("Created edges: " << createEdgeCount);
  SC_LOG_INFO("Erased nodes: " << eraseNodeCount);
}
