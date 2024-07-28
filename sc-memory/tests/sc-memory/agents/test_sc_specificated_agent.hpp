#pragma once

#include <thread>

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"
#include "sc-memory/sc_agent.hpp"

#include "sc-memory/sc_timer.hpp"
#include "sc-memory/utils/sc_lock.hpp"

class TestWaiter
{
public:
  TestWaiter()
  {
    m_lock.Lock();
  }

  bool Wait(sc_float time_s = 5.0)
  {
    ScTimer timer(time_s);
    while (!timer.IsTimeOut() && m_lock.IsLocked())
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return !m_lock.IsLocked();
  }

  void Unlock()
  {
    m_lock.Unlock();
  }

private:
  utils::ScLock m_lock;
};

static ScKeynode const test_specificated_agent_implementation =
    ScKeynode("test_specificated_agent_implementation", ScType::NodeConst);

class ATestSpecificatedAgent : public ScSpecificatedAgent<test_specificated_agent_implementation>
{
public:
  static inline TestWaiter msWaiter;

  ScResult DoProgram(ScElementaryEvent const & event, ScAction & action) override;
};
