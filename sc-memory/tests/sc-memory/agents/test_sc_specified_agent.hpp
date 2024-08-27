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
    if (!m_lock.IsLocked())
      m_lock.Lock();
  }

private:
  utils::ScLock m_lock;
};

class ATestSpecifiedAgent : public ScAgent<ScElementaryEvent>
{
public:
  static inline TestWaiter msWaiter;
  static inline ScKeynode const test_specified_agent_implementation =
      ScKeynode("test_specified_agent_implementation", ScType::NodeConst);
  static inline ScKeynode const test_specified_agent_action =
      ScKeynode("test_specified_agent_action", ScType::NodeConstClass);

  static inline ScTemplateKeynode const & test_specified_agent_initiation_condition =
      ScTemplateKeynode("test_specified_agent_initiation_condition")
          .Triple(ScKeynodes::action_initiated, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action")
          .Triple(test_specified_agent_action, ScType::EdgeAccessVarPosPerm, "_action");

  static inline ScTemplateKeynode const & test_specified_agent_initiation_condition_in_kb =
      ScTemplateKeynode("test_specified_agent_initiation_condition");

  static inline ScTemplateKeynode const & test_specified_agent_result_condition =
      ScTemplateKeynode("test_specified_agent_result_condition")
          .Triple(ScKeynodes::action_initiated, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action")
          .Triple(test_specified_agent_action, ScType::EdgeAccessVarPosPerm, "_action")
          .Quintuple(
              "_action",
              ScType::EdgeDCommonVar,
              ScType::NodeVar,
              ScType::EdgeAccessVarPosPerm,
              ScKeynodes::nrel_result);

  static inline ScTemplateKeynode const & test_specified_agent_result_condition_in_kb =
      ScTemplateKeynode("test_specified_agent_result_condition");

  ScResult DoProgram(ScElementaryEvent const & event, ScAction & action) override;
};
