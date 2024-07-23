/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

// Should be equal to C values

ScEvent::Type constexpr ScEvent::Type::Unknown(sc_event_unknown);
ScEvent::Type constexpr ScEvent::Type::AddInputArc(sc_event_add_input_arc);
ScEvent::Type constexpr ScEvent::Type::AddOutputArc(sc_event_add_output_arc);
ScEvent::Type constexpr ScEvent::Type::AddEdge(sc_event_add_edge);
ScEvent::Type constexpr ScEvent::Type::RemoveInputArc(sc_event_remove_input_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveOutputArc(sc_event_remove_output_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveEdge(sc_event_remove_edge);
ScEvent::Type constexpr ScEvent::Type::EraseElement(sc_event_erase_element);
ScEvent::Type constexpr ScEvent::Type::ChangeContent(sc_event_change_content);

ScEvent::~ScEvent() = default;

sc_result ScEvent::Initialize(ScMemoryContext *, ScAddr const &)
{
  return SC_RESULT_OK;
}

sc_result ScEvent::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}

ScAddr ScElementaryEvent::GetUser() const
{
  return m_userAddr;
}

ScAddr ScElementaryEvent::GetSubscriptionElement() const
{
  return m_subscriptionAddr;
}

std::array<ScAddr, 3> ScElementaryEvent::GetTriple() const
{
  return {m_subscriptionAddr, m_connectorAddr, m_otherAddr};
}

ScElementaryEvent::ScElementaryEvent(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : m_userAddr(userAddr)
  , m_subscriptionAddr(subscriptionAddr)
  , m_connectorAddr(connectorAddr)
  , m_connectorType(connectorType)
  , m_otherAddr(otherAddr) {};

ScAddr ScEventAddArc::GetAddedArc() const
{
  return m_connectorAddr;
}

ScType ScEventAddArc::GetAddedArcType() const
{
  return m_connectorType;
}

ScAddr ScEventAddArc::GetArcSourceElement() const
{
  return m_subscriptionAddr;
}

ScAddr ScEventAddArc::GetArcTargetElement() const
{
  return m_otherAddr;
}

ScEventAddArc::ScEventAddArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScEventAddEdge::ScEventAddEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScAddr ScEventAddEdge::GetAddedEdge() const
{
  return m_connectorAddr;
}

ScType ScEventAddEdge::GetAddedEdgeType() const
{
  return m_connectorType;
}

ScAddr ScEventAddEdge::GetEdgeSourceElement() const
{
  return m_otherAddr;
}

ScAddr ScEventAddEdge::GetEdgeTargetElement() const
{
  return m_subscriptionAddr;
}

ScEventAddOutputArc::ScEventAddOutputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddArc(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

_SC_EXTERN ScEventAddInputArc::ScEventAddInputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddArc(userAddr, otherAddr, connectorAddr, connectorType, subscriptionAddr) {};

ScType ScEventRemoveArc::GetRemovedArcType() const
{
  return m_connectorType;
}

ScAddr ScEventRemoveArc::GetArcSourceElement() const
{
  return m_subscriptionAddr;
}

ScAddr ScEventRemoveArc::GetArcTargetElement() const
{
  return m_otherAddr;
}

ScEventRemoveArc::ScEventRemoveArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScType ScEventRemoveEdge::GetRemovedEdgeType() const
{
  return m_connectorType;
}

ScAddr ScEventRemoveEdge::GetEdgeSourceElement() const
{
  return m_subscriptionAddr;
}

ScAddr ScEventRemoveEdge::GetEdgeTargetElement() const
{
  return m_otherAddr;
}

ScEventRemoveEdge::ScEventRemoveEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScEventRemoveOutputArc::ScEventRemoveOutputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveArc(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScEventRemoveInputArc::ScEventRemoveInputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveArc(userAddr, otherAddr, connectorAddr, connectorType, subscriptionAddr) {};

ScEventEraseElement::ScEventEraseElement(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScAddr ScEventEraseElement::GetSubscriptionElement() const
{
  return m_subscriptionAddr;
}

ScEventChangeContent::ScEventChangeContent(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};
