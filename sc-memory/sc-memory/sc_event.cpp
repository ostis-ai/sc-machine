/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

// Should be equal to C values

ScEvent::Type constexpr ScEvent::Type::Unknown(sc_event_unknown);
ScEvent::Type constexpr ScEvent::Type::AddInputEdge(sc_event_add_input_arc);
ScEvent::Type constexpr ScEvent::Type::AddOutputEdge(sc_event_add_output_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveInputEdge(sc_event_remove_input_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveOutputEdge(sc_event_remove_output_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveElement(sc_event_remove_element);
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

ScAddr ScEventAddEdge::GetAddedConnector() const
{
  return m_connectorAddr;
}

ScType ScEventAddEdge::GetAddedConnectorType() const
{
  return m_connectorType;
}

ScAddr ScEventAddEdge::GetOtherElement() const
{
  return m_otherAddr;
}

ScEventAddEdge::ScEventAddEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScEventAddOutputEdge::ScEventAddOutputEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddEdge(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

_SC_EXTERN ScEventAddInputEdge::ScEventAddInputEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddEdge(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScType ScEventRemoveEdge::GetRemovedConnectorType() const
{
  return m_connectorType;
}

ScAddr ScEventRemoveEdge::GetOtherElement() const
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

ScEventRemoveOutputEdge::ScEventRemoveOutputEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveEdge(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScEventRemoveInputEdge::ScEventRemoveInputEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveEdge(userAddr, otherAddr, connectorAddr, connectorType, subscriptionAddr) {};

ScEventRemoveElement::ScEventRemoveElement(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

ScAddr ScEventRemoveElement::GetSubscriptionElement() const
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
