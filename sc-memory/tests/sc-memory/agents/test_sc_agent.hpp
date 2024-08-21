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

class ATestAddIncomingArc : public ScAgent<ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const add_incoming_arc_action{"add_incoming_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestAddOutgoingArc : public ScAgent<ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const add_outgoing_arc_action{"add_outgoing_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestAddEdge : public ScAgent<ScEventGenerateEdge<ScType::EdgeUCommonConst>>
{
public:
  static inline ScKeynode const add_edge_action{"add_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventGenerateEdge<ScType::EdgeUCommonConst> const & event, ScAction & action) override;
};

class ATestEraseIncomingArc : public ScAgent<ScEventEraseIncomingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const remove_incoming_arc_action{"remove_incoming_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventEraseIncomingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action) override;
};

class ATestEraseOutgoingArc : public ScAgent<ScEventEraseOutgoingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const remove_outgoing_arc_action{"remove_outgoing_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventEraseOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action) override;
};

class ATestEraseEdge : public ScAgent<ScEventEraseEdge<ScType::EdgeUCommonConst>>
{
public:
  static inline ScKeynode const remove_edge_action{"remove_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventEraseEdge<ScType::EdgeUCommonConst> const & event, ScAction & action) override;
};

class ATestEraseElement : public ScAgent<ScEventEraseElement>
{
public:
  static inline ScKeynode const erase_element_action{"erase_element_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventEraseElement const & event, ScAction & action) override;
};

class ATestChangeLinkContent : public ScAgent<ScEventChangeLinkContent>
{
public:
  static inline ScKeynode const content_change_action{"content_change_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventChangeLinkContent const & event, ScAction & action) override;
};

class ATestAddMultipleOutputArc : public ScAgent<ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestCheckResult : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplate : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScTemplate GetInitiationConditionTemplate() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestCheckInitiationCondition : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  sc_bool CheckInitiationCondition(ScActionEvent const & event) override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestGetResultConditionTemplate : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;

  ScTemplate GetResultConditionTemplate() const override;
};

class ATestCheckResultCondition : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;

  sc_bool CheckResultCondition(ScActionEvent const & event, ScAction & action) override;
};

class ATestActionDeactivated : public ScActionAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};
