#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"
#include "sc-memory/sc_timer.hpp"
#include "sc-memory/utils/sc_lock.hpp"
#include "sc-memory/sc_agent.hpp"

#include <thread>

class TestWaiter
{
public:
  TestWaiter()
  {
    m_lock.Lock();
  }

  bool Wait()
  {
    ScTimer timer(5.0);
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

class ATestAddInputEdge : public ScAgent<ScEventAddInputEdge>
{
public:
  static inline ScKeynode const add_input_edge_action{"add_input_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventAddInputEdge const & event, ScAction & action) override;
};

class ATestAddOutputEdge : public ScAgent<ScEventAddOutputEdge>
{
public:
  static inline ScKeynode const add_output_edge_action{"add_output_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventAddOutputEdge const & event, ScAction & action) override;
};

class ATestRemoveInputEdge : public ScAgent<ScEventRemoveInputEdge>
{
public:
  static inline ScKeynode const remove_input_edge_action{"remove_input_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventRemoveInputEdge const & event, ScAction & action) override;
};

class ATestRemoveOutputEdge : public ScAgent<ScEventRemoveOutputEdge>
{
public:
  static inline ScKeynode const remove_output_edge_action{"remove_output_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventRemoveOutputEdge const & event, ScAction & action) override;
};

class ATestRemoveElement : public ScAgent<ScEventRemoveElement>
{
public:
  static inline ScKeynode const remove_element_action{"remove_element_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventRemoveElement const & event, ScAction & action) override;
};

class ATestContentChanged : public ScAgent<ScEventChangeContent>
{
public:
  static inline ScKeynode const content_change_action{"content_change_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventChangeContent const & event, ScAction & action) override;
};

class ATestAddMultipleOutputEdge : public ScAgent<ScEventAddOutputEdge>
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventAddOutputEdge const & event, ScAction & action) override;
};

class ATestCheckResult : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScActionEvent const & event, ScAction & action) override;
};
