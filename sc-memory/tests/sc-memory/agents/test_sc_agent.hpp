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

class ATestGenerateConnector : public ScAgent<ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const generate_connector_action{"generate_connector_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestGenerateIncomingArc : public ScAgent<ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const generate_incoming_arc_action{"generate_incoming_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestGenerateOutgoingArc : public ScAgent<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const generate_outgoing_arc_action{"generate_outgoing_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestGenerateEdge : public ScAgent<ScEventAfterGenerateEdge<ScType::EdgeUCommonConst>>
{
public:
  static inline ScKeynode const add_edge_action{"add_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateEdge<ScType::EdgeUCommonConst> const & event, ScAction & action) override;
};

class ATestEraseConnector : public ScAgent<ScEventBeforeEraseConnector<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const erase_connector_action{"erase_connector_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseConnector<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestEraseIncomingArc : public ScAgent<ScEventBeforeEraseIncomingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const erase_incoming_arc_action{"erase_incoming_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseIncomingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestEraseOutgoingArc : public ScAgent<ScEventBeforeEraseOutgoingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline ScKeynode const erase_outgoing_arc_action{"erase_outgoing_arc_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestEraseEdge : public ScAgent<ScEventBeforeEraseEdge<ScType::EdgeUCommonConst>>
{
public:
  static inline ScKeynode const erase_edge_action{"erase_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseEdge<ScType::EdgeUCommonConst> const & event, ScAction & action) override;
};

class ATestEraseElement : public ScAgent<ScEventBeforeEraseElement>
{
public:
  static inline ScKeynode const erase_element_action{"erase_element_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseElement const & event, ScAction & action) override;
};

class ATestChangeLinkContent : public ScAgent<ScEventBeforeChangeLinkContent>
{
public:
  static inline ScKeynode const content_change_action{"content_change_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeChangeLinkContent const & event, ScAction & action) override;
};

class ATestGenerateMultipleOutputArc : public ScAgent<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event, ScAction & action)
      override;
};

class ATestDoProgramOne : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestDoProgramTwo : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;
};

class ATestDoProgram : public ScBaseAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetAbstractAgent() const override;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;
};

class ATestException : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  static inline TestWaiter msWaiter;

  ScResult DoProgram(ScAction & action) override;
};

class ATestCheckResult : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplate : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScTemplate GetInitiationConditionTemplate(ScActionEvent const & event) const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  static inline ScTemplateKeynode const & initiationCondition =
      ScTemplateKeynode("initiation_condition_without_event_triple")
          .Triple(
              ATestGenerateOutgoingArc::generate_outgoing_arc_action,
              ScType::EdgeAccessVarPosPerm,
              ScType::NodeVar >> "_action")
          .Triple("_action", ScType::EdgeAccessVarPosPerm, ScType::NodeVar);

  ScAddr GetActionClass() const override;

  ScAddr GetInitiationCondition() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  static inline ScTemplateKeynode const & initiationCondition =
      ScTemplateKeynode("initiation_condition_with_invalid_connector_type_in_event_type")
          .Triple(ScKeynodes::action_initiated, ScType::EdgeDCommonVar, ScType::NodeVar >> "_action")
          .Triple(ATestGenerateOutgoingArc::generate_outgoing_arc_action, ScType::EdgeAccessVarPosPerm, "_action")
          .Triple("_action", ScType::EdgeAccessVarPosPerm, ScType::NodeVar);

  ScAddr GetActionClass() const override;

  ScAddr GetInitiationCondition() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplateHasEventTripleTwice : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  static inline ScTemplateKeynode const & initiationCondition =
      ScTemplateKeynode("initiation_condition_has_event_triple_twice")
          .Triple(ScKeynodes::action_initiated, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action")
          .Triple(ScKeynodes::action_initiated, ScType::EdgeAccessVarPosPerm, "_action")
          .Triple(ATestGenerateOutgoingArc::generate_outgoing_arc_action, ScType::EdgeAccessVarPosPerm, "_action")
          .Triple("_action", ScType::EdgeAccessVarPosPerm, ScType::NodeVar);

  ScAddr GetActionClass() const override;

  ScAddr GetInitiationCondition() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestCheckInitiationCondition : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  bool CheckInitiationCondition(ScActionEvent const & event) override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

class ATestGetResultConditionTemplate : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;

  ScTemplate GetResultConditionTemplate(ScActionEvent const & event, ScAction & action) const override;
};

class ATestCheckResultCondition : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;

  bool CheckResultCondition(ScActionEvent const & event, ScAction & action) override;
};

class ATestActionDeactivated : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};
