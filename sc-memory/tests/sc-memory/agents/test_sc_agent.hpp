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

class ATestAddInputArc : public ScAgent<ScEventAddInputArc>
{
public:
  static inline ScKeynode const add_input_arc_action{"add_input_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventAddInputArc const & event, ScAction & action) override;
};

class ATestAddOutputArc : public ScAgent<ScEventAddOutputArc>
{
public:
  static inline ScKeynode const add_output_arc_action{"add_output_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventAddOutputArc const & event, ScAction & action) override;
};

class ATestRemoveInputArc : public ScAgent<ScEventRemoveInputArc>
{
public:
  static inline ScKeynode const remove_input_arc_action{"remove_input_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventRemoveInputArc const & event, ScAction & action) override;
};

class ATestRemoveOutputArc : public ScAgent<ScEventRemoveOutputArc>
{
public:
  static inline ScKeynode const remove_output_arc_action{"remove_output_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventRemoveOutputArc const & event, ScAction & action) override;
};

class ATestEraseElement : public ScAgent<ScEventEraseElement>
{
public:
  static inline ScKeynode const erase_element_action{"erase_element_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventEraseElement const & event, ScAction & action) override;
};

class ATestChangeContent : public ScAgent<ScEventChangeContent>
{
public:
  static inline ScKeynode const content_change_action{"content_change_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventChangeContent const & event, ScAction & action) override;
};

class ATestAddMultipleOutputArc : public ScAgent<ScEventAddOutputArc>
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScEventAddOutputArc const & event, ScAction & action) override;
};

class ATestCheckResult : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_result DoProgram(ScActionEvent const & event, ScAction & action) override;
};
