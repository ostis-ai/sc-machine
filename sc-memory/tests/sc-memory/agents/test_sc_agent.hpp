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

class ATestAddInputArc : public ScAgent<ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const add_input_arc_action{"add_input_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAddInputArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action) override;
};

class ATestAddOutputArc : public ScAgent<ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const add_output_arc_action{"add_output_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action) override;
};

class ATestAddEdge : public ScAgent<ScEventAddEdge<ScType::EdgeUCommonConst>>
{
public:
  static inline ScKeynode const add_edge_action{"add_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAddEdge<ScType::EdgeUCommonConst> const & event, ScAction & action) override;
};

class ATestRemoveInputArc : public ScAgent<ScEventRemoveInputArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const remove_input_arc_action{"remove_input_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventRemoveInputArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action) override;
};

class ATestRemoveOutputArc : public ScAgent<ScEventRemoveOutputArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const remove_output_arc_action{"remove_output_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventRemoveOutputArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action) override;
};

class ATestRemoveEdge : public ScAgent<ScEventRemoveEdge<ScType::EdgeUCommonConst>>
{
public:
  static inline ScKeynode const remove_edge_action{"remove_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventRemoveEdge<ScType::EdgeUCommonConst> const & event, ScAction & action) override;
};

class ATestEraseElement : public ScAgent<ScEventRemoveElement>
{
public:
  static inline ScKeynode const erase_element_action{"erase_element_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventRemoveElement const & event, ScAction & action) override;
};

class ATestChangeContent : public ScAgent<ScEventChangeLinkContent>
{
public:
  static inline ScKeynode const content_change_action{"content_change_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventChangeLinkContent const & event, ScAction & action) override;
};

class ATestAddMultipleOutputArc : public ScAgent<ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action) override;
};

class ATestCheckResult : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};
