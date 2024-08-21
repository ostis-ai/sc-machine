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

  void Reset()
  {
    m_lock.Lock();
  }

private:
  utils::ScLock m_lock;
};

class ATestSpecificatedAgent : public ScAgent<ScElementaryEvent>
{
public:
  static inline TestWaiter msWaiter;
  static inline ScKeynode const test_specificated_agent_implementation =
      ScKeynode("test_specificated_agent_implementation", ScType::NodeConst);
  static inline ScKeynode const test_specificated_agent_action =
      ScKeynode("test_specificated_agent_action", ScType::NodeConstClass);

  static inline ScTemplateKeynode const & test_specificated_agent_initiation_condition =
      ScTemplateKeynode("test_specificated_agent_initiation_condition")
          .Triple(ScKeynodes::action_initiated, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action")
          .Triple(test_specificated_agent_action, ScType::EdgeAccessVarPosPerm, "_action");

  static inline ScTemplateKeynode const & test_specificated_agent_result_condition =
      ScTemplateKeynode("test_specificated_agent_result_condition")
          .Triple(ScKeynodes::action_initiated, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action")
          .Triple(test_specificated_agent_action, ScType::EdgeAccessVarPosPerm, "_action")
          .Quintuple(
              "_action",
              ScType::EdgeDCommonVar,
              ScType::NodeVar,
              ScType::EdgeAccessVarPosPerm,
              ScKeynodes::nrel_result);

  ScResult DoProgram(ScElementaryEvent const & event, ScAction & action) override;
};
