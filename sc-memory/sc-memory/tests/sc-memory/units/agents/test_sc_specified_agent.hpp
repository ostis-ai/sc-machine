/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <thread>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_agent.hpp>

#include <sc-memory/sc_timer.hpp>
#include <sc-memory/utils/sc_lock.hpp>

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
      ScKeynode("test_specified_agent_implementation", ScType::ConstNode);
  static inline ScKeynode const test_specified_agent_action =
      ScKeynode("test_specified_agent_action", ScType::ConstNodeClass);

  static inline ScTemplateKeynode const & test_specified_agent_initiation_condition =
      ScTemplateKeynode("test_specified_agent_initiation_condition_programably_specified")
          .Triple(ScKeynodes::action_initiated, ScType::VarPermPosArc, ScType::VarNode >> "_action")
          .Triple(test_specified_agent_action, ScType::VarPermPosArc, "_action");

  static inline ScTemplateKeynode const & test_specified_agent_initiation_condition_in_kb =
      ScTemplateKeynode("test_specified_agent_initiation_condition_programably_specified");

  static inline ScTemplateKeynode const & test_specified_agent_result_condition =
      ScTemplateKeynode("test_specified_agent_result_condition_programably_specified")
          .Triple(ScKeynodes::action_initiated, ScType::VarPermPosArc, ScType::VarNode >> "_action")
          .Triple(test_specified_agent_action, ScType::VarPermPosArc, "_action")
          .Quintuple("_action", ScType::VarCommonArc, ScType::VarNode, ScType::VarPermPosArc, ScKeynodes::nrel_result);

  static inline ScTemplateKeynode const & test_specified_agent_result_condition_in_kb =
      ScTemplateKeynode("test_specified_agent_result_condition_programably_specified");
  static inline ScKeynode const concept_set = ScKeynode("concept_set");

  ScResult DoProgram(ScElementaryEvent const & event, ScAction & action) override;
};
