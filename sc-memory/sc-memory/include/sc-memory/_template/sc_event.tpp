/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_event.hpp"

// TScElementaryEvent
//-------------------------------------------------------------------------

template <ScType const & elementType>
TScElementaryEvent<elementType>::TScElementaryEvent(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScElementaryEvent(eventClassAddr, userAddr, subscriptionElementAddr, connectorAddr, connectorType, otherAddr){};

// ScEventAfterGenerateConnector
//-------------------------------------------------------------------------

template <ScType const & connectorType>
std::tuple<ScAddr, ScAddr> ScEventAfterGenerateConnector<connectorType>::GetConnectorIncidentElements() const noexcept
{
  return {ScElementaryEvent::GetSubscriptionElement(), ScElementaryEvent::GetOtherElement()};
}

template <ScType const & tConnectorType>
ScEventAfterGenerateConnector<tConnectorType>::ScEventAfterGenerateConnector(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : TScElementaryEvent<tConnectorType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & tConnectorType>
ScEventAfterGenerateConnector<tConnectorType>::ScEventAfterGenerateConnector(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventAfterGenerateConnector<tConnectorType>(
        ScEventAfterGenerateConnector::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

// ScEventAfterGenerateArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetArc() const noexcept
{
  return GetConnector();
}

template <ScType const & arcType>
ScType ScEventAfterGenerateArc<arcType>::GetArcType() const noexcept
{
  return GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetArcSourceElement() const noexcept
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetArcTargetElement() const noexcept
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScEventAfterGenerateArc<arcType>::ScEventAfterGenerateArc(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventAfterGenerateConnector<arcType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetConnector() const noexcept
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventAfterGenerateArc<arcType>::GetConnectorType() const noexcept
{
  return ScElementaryEvent::GetConnectorType();
}

// ScEventAfterGenerateEdge
//-------------------------------------------------------------------------

template <ScType const & edgeType>
ScAddr ScEventAfterGenerateEdge<edgeType>::GetEdge() const noexcept
{
  return GetConnector();
}

template <ScType const & edgeType>
ScType ScEventAfterGenerateEdge<edgeType>::GetEdgeType() const noexcept
{
  return GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventAfterGenerateEdge<edgeType>::GetEdgeIncidentElements() const noexcept
{
  return GetConnectorIncidentElements();
}

template <ScType const & edgeType>
ScEventAfterGenerateEdge<edgeType>::ScEventAfterGenerateEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventAfterGenerateConnector<edgeType>(
        ScEventAfterGenerateEdge<edgeType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventAfterGenerateEdge<edgeType>::GetConnector() const noexcept
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventAfterGenerateEdge<edgeType>::GetConnectorType() const noexcept
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventAfterGenerateEdge<edgeType>::GetConnectorIncidentElements() const noexcept
{
  return {ScElementaryEvent::GetSubscriptionElement(), ScElementaryEvent::GetOtherElement()};
}

// ScEventAfterGenerateOutgoingArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScEventAfterGenerateOutgoingArc<arcType>::ScEventAfterGenerateOutgoingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventAfterGenerateArc<arcType>(
        ScEventAfterGenerateOutgoingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

// ScEventAfterGenerateIncomingArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScAddr ScEventAfterGenerateIncomingArc<arcType>::GetArcSourceElement() const noexcept
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventAfterGenerateIncomingArc<arcType>::GetArcTargetElement() const noexcept
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
_SC_EXTERN ScEventAfterGenerateIncomingArc<arcType>::ScEventAfterGenerateIncomingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventAfterGenerateArc<arcType>(
        ScEventAfterGenerateIncomingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr) {};

// ScEventBeforeEraseConnector
//-------------------------------------------------------------------------

template <ScType const & connectorType>
ScAddr ScEventBeforeEraseConnector<connectorType>::GetConnector() const noexcept
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & connectorType>
ScType ScEventBeforeEraseConnector<connectorType>::GetConnectorType() const noexcept
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & connectorType>
std::tuple<ScAddr, ScAddr> ScEventBeforeEraseConnector<connectorType>::GetConnectorIncidentElements() const noexcept
{
  return {ScElementaryEvent::GetSubscriptionElement(), ScElementaryEvent::GetOtherElement()};
}

template <ScType const & tConnectorType>
ScEventBeforeEraseConnector<tConnectorType>::ScEventBeforeEraseConnector(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : TScElementaryEvent<tConnectorType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & tConnectorType>
ScEventBeforeEraseConnector<tConnectorType>::ScEventBeforeEraseConnector(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventBeforeEraseConnector<tConnectorType>(
        ScEventBeforeEraseConnector::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

// ScEventBeforeEraseArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetArc() const noexcept
{
  return GetConnector();
}

template <ScType const & arcType>
ScType ScEventBeforeEraseArc<arcType>::GetArcType() const noexcept
{
  return GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetArcSourceElement() const noexcept
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetArcTargetElement() const noexcept
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScEventBeforeEraseArc<arcType>::ScEventBeforeEraseArc(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventBeforeEraseConnector<arcType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetConnector() const noexcept
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventBeforeEraseArc<arcType>::GetConnectorType() const noexcept
{
  return ScElementaryEvent::GetConnectorType();
}

// ScEventBeforeEraseEdge
//-------------------------------------------------------------------------

template <ScType const & edgeType>
ScAddr ScEventBeforeEraseEdge<edgeType>::GetEdge() const noexcept
{
  return GetConnector();
}

template <ScType const & edgeType>
ScType ScEventBeforeEraseEdge<edgeType>::GetEdgeType() const noexcept
{
  return GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventBeforeEraseEdge<edgeType>::GetEdgeIncidentElements() const noexcept
{
  return GetConnectorIncidentElements();
}

template <ScType const & edgeType>
ScEventBeforeEraseEdge<edgeType>::ScEventBeforeEraseEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventBeforeEraseConnector<edgeType>(
        ScEventBeforeEraseEdge<edgeType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventBeforeEraseEdge<edgeType>::GetConnector() const noexcept
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventBeforeEraseEdge<edgeType>::GetConnectorType() const noexcept
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventBeforeEraseEdge<edgeType>::GetConnectorIncidentElements() const noexcept
{
  return {ScElementaryEvent::GetSubscriptionElement(), ScElementaryEvent::GetOtherElement()};
}

// ScEventBeforeEraseOutgoingArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScEventBeforeEraseOutgoingArc<arcType>::ScEventBeforeEraseOutgoingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventBeforeEraseArc<arcType>(
        ScEventBeforeEraseOutgoingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

// ScEventBeforeEraseIncomingArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScAddr ScEventBeforeEraseIncomingArc<arcType>::GetArcSourceElement() const noexcept
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventBeforeEraseIncomingArc<arcType>::GetArcTargetElement() const noexcept
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScEventBeforeEraseIncomingArc<arcType>::ScEventBeforeEraseIncomingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : ScEventBeforeEraseArc<arcType>(
        ScEventBeforeEraseIncomingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};
