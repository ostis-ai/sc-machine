/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

// Should be equal to C values

ScEvent::Type const ScEvent::Type::Unknown(sc_event_unknown);
ScEvent::Type const ScEvent::Type::AddInputArc(sc_event_add_input_arc);
ScEvent::Type const ScEvent::Type::AddOutputArc(sc_event_add_output_arc);
ScEvent::Type const ScEvent::Type::AddEdge(sc_event_add_edge);
ScEvent::Type const ScEvent::Type::RemoveInputArc(sc_event_remove_input_arc);
ScEvent::Type const ScEvent::Type::RemoveOutputArc(sc_event_remove_output_arc);
ScEvent::Type const ScEvent::Type::RemoveEdge(sc_event_remove_edge);
ScEvent::Type const ScEvent::Type::EraseElement(sc_event_erase_element);
ScEvent::Type const ScEvent::Type::ChangeContent(sc_event_change_content);

ScEvent::~ScEvent() = default;

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
