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
    m_lock.Lock();
  }

private:
  utils::ScLock m_lock;
};

class TestScAgent : public ScAgent<ScElementaryEvent>
{
public:
  void SetImplementation(ScAddr const & implementationAddr)
  {
    ScAgent<ScElementaryEvent>::SetImplementation(implementationAddr);
  }
};

class TestScEvent : public ScElementaryEvent
{
public:
  TestScEvent();
};

class ATestGenerateConnector : public ScAgent<ScEventAfterGenerateConnector<ScType::ConstPermPosArc>>
{
public:
  static inline ScKeynode const generate_connector_action{"generate_connector_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateConnector<ScType::ConstPermPosArc> const & event, ScAction & action) override;
};

class ATestGenerateIncomingArc : public ScAgent<ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc>>
{
public:
  static inline ScKeynode const generate_incoming_arc_action{"generate_incoming_arc_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc> const & event, ScAction & action)
      override;
};

class ATestGenerateOutgoingArc : public ScAgent<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>
{
public:
  static inline ScKeynode const generate_outgoing_arc_action{"generate_outgoing_arc_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event, ScAction & action)
      override;
};

class ATestGenerateEdge : public ScAgent<ScEventAfterGenerateEdge<ScType::ConstCommonEdge>>
{
public:
  static inline ScKeynode const add_edge_action{"add_edge_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateEdge<ScType::ConstCommonEdge> const & event, ScAction & action) override;
};

class ATestGenerateEdgeAsConnector : public ScAgent<ScEventAfterGenerateConnector<ScType::ConstCommonEdge>>
{
public:
  static inline ScKeynode const add_edge_action{"add_edge_as_connector_action", ScType::ConstNodeClass};
  static inline ScKeynode const subscription_element{"subscription_element", ScType::ConstNode};
  static inline TestWaiter msWaiter;

  static inline ScTemplateKeynode const & initiationCondition =
      ScTemplateKeynode("initiation_condition_for_edge_as_connector")
          .Triple(subscription_element, ScType::VarCommonEdge, ScType::VarNode >> "_action");

  ScAddr GetActionClass() const override;

  ScAddr GetInitiationCondition() const override;

  ScResult DoProgram(ScEventAfterGenerateConnector<ScType::ConstCommonEdge> const & event, ScAction & action) override;
};

class ATestEraseConnector : public ScAgent<ScEventBeforeEraseConnector<ScType::ConstPermPosArc>>
{
public:
  static inline ScKeynode const erase_connector_action{"erase_connector_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseConnector<ScType::ConstPermPosArc> const & event, ScAction & action) override;
};

class ATestEraseIncomingArc : public ScAgent<ScEventBeforeEraseIncomingArc<ScType::ConstPermPosArc>>
{
public:
  static inline ScKeynode const erase_incoming_arc_action{"erase_incoming_arc_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseIncomingArc<ScType::ConstPermPosArc> const & event, ScAction & action) override;
};

class ATestEraseOutgoingArc : public ScAgent<ScEventBeforeEraseOutgoingArc<ScType::ConstPermPosArc>>
{
public:
  static inline ScKeynode const erase_outgoing_arc_action{"erase_outgoing_arc_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseOutgoingArc<ScType::ConstPermPosArc> const & event, ScAction & action) override;
};

class ATestEraseEdge : public ScAgent<ScEventBeforeEraseEdge<ScType::ConstCommonEdge>>
{
public:
  static inline ScKeynode const erase_edge_action{"erase_edge_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseEdge<ScType::ConstCommonEdge> const & event, ScAction & action) override;
};

class ATestEraseElement : public ScAgent<ScEventBeforeEraseElement>
{
public:
  static inline ScKeynode const erase_element_action{"erase_element_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeEraseElement const & event, ScAction & action) override;
};

class ATestChangeLinkContent : public ScAgent<ScEventBeforeChangeLinkContent>
{
public:
  static inline ScKeynode const content_change_action{"content_change_action", ScType::ConstNodeClass};
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventBeforeChangeLinkContent const & event, ScAction & action) override;
};

class ATestGenerateMultipleOutputArc : public ScAgent<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event, ScAction & action)
      override;
};

class ATestDoProgramOne : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestDoProgramTwo : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;
};

class ATestDoProgram : public ScElementaryEventAgent
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

class ATestEraseActionWithException : public ScActionInitiatedAgent
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

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplate : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScTemplate GetInitiationConditionTemplate(ScActionInitiatedEvent const & event) const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  static inline ScTemplateKeynode const & initiationCondition =
      ScTemplateKeynode("initiation_condition_without_event_triple")
          .Triple(
              ATestGenerateOutgoingArc::generate_outgoing_arc_action,
              ScType::VarPermPosArc,
              ScType::VarNode >> "_action")
          .Triple("_action", ScType::VarPermPosArc, ScType::VarNode);

  ScAddr GetActionClass() const override;

  ScAddr GetInitiationCondition() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  static inline ScTemplateKeynode const & initiationCondition =
      ScTemplateKeynode("initiation_condition_with_invalid_connector_type_in_event_type")
          .Triple(ScKeynodes::action_initiated, ScType::VarCommonArc, ScType::VarNode >> "_action")
          .Triple(ATestGenerateOutgoingArc::generate_outgoing_arc_action, ScType::VarPermPosArc, "_action")
          .Triple("_action", ScType::VarPermPosArc, ScType::VarNode);

  ScAddr GetActionClass() const override;

  ScAddr GetInitiationCondition() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestGetInitiationConditionTemplateHasEventTripleTwice : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  static inline ScTemplateKeynode const & initiationCondition =
      ScTemplateKeynode("initiation_condition_has_event_triple_twice")
          .Triple(ScKeynodes::action_initiated, ScType::VarPermPosArc, ScType::VarNode >> "_action")
          .Triple(ScKeynodes::action_initiated, ScType::VarPermPosArc, "_action")
          .Triple(ATestGenerateOutgoingArc::generate_outgoing_arc_action, ScType::VarPermPosArc, "_action")
          .Triple("_action", ScType::VarPermPosArc, ScType::VarNode);

  ScAddr GetActionClass() const override;

  ScAddr GetInitiationCondition() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestGetInvalidInitiationConditionTemplate : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScTemplate GetInitiationConditionTemplate(ScActionInitiatedEvent const & event) const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestCheckInitiationCondition : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  bool CheckInitiationCondition(ScActionInitiatedEvent const & event) override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestGetResultConditionTemplate : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;

  ScTemplate GetResultConditionTemplate(ScActionInitiatedEvent const & event, ScAction & action) const override;
};

class ATestGetInvalidResultConditionTemplate : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;

  ScTemplate GetResultConditionTemplate(ScActionInitiatedEvent const & event, ScAction & action) const override;
};

class ATestCheckResultCondition : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;

  bool CheckResultCondition(ScActionInitiatedEvent const & event, ScAction & action) override;
};

class ATestActionDeactivated : public ScActionInitiatedAgent
{
public:
  static inline TestWaiter msWaiter;

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};
