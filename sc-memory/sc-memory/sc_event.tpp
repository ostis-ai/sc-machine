/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

// TScElementaryEvent
//-------------------------------------------------------------------------

template <ScType const & elementType>
TScElementaryEvent<elementType>::TScElementaryEvent(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(eventClassAddr, userAddr, subscriptionElementAddr, connectorAddr, connectorType, otherAddr){};

// ScEventAfterGenerateConnector
//-------------------------------------------------------------------------

template <ScType const & connectorType>
std::tuple<ScAddr, ScAddr> ScEventAfterGenerateConnector<connectorType>::GetConnectorIncidentElements() const
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
    ScAddr const & otherAddr)
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
    ScAddr const & otherAddr)
  : ScEventAfterGenerateConnector<tConnectorType>(
        ScEventAfterGenerateConnector::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

// Ignore these methods, because they can't be used.
// LCOV_EXCL_START
template <ScType const & connectorType>
ScAddr ScEventAfterGenerateConnector<connectorType>::GetOtherElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

// LCOV_EXCL_END

// ScEventAfterGenerateArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetArc() const
{
  return GetConnector();
}

template <ScType const & arcType>
ScType ScEventAfterGenerateArc<arcType>::GetArcType() const
{
  return GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetArcTargetElement() const
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
    ScAddr const & otherAddr)
  : ScEventAfterGenerateConnector<arcType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScAddr ScEventAfterGenerateArc<arcType>::GetConnector() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventAfterGenerateArc<arcType>::GetConnectorType() const
{
  return ScElementaryEvent::GetConnectorType();
}

// Ignore these methods, because they can't be used.
// LCOV_EXCL_START
template <ScType const & arcType>
std::tuple<ScAddr, ScAddr> ScEventAfterGenerateArc<arcType>::GetConnectorIncidentElements() const
{
  return {ScElementaryEvent::GetSubscriptionElement(), ScElementaryEvent::GetOtherElement()};
}

// LCOV_EXCL_END

// ScEventAfterGenerateEdge
//-------------------------------------------------------------------------

template <ScType const & edgeType>
ScAddr ScEventAfterGenerateEdge<edgeType>::GetEdge() const
{
  return GetConnector();
}

template <ScType const & edgeType>
ScType ScEventAfterGenerateEdge<edgeType>::GetEdgeType() const
{
  return GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventAfterGenerateEdge<edgeType>::GetEdgeIncidentElements() const
{
  return GetConnectorIncidentElements();
}

template <ScType const & edgeType>
ScEventAfterGenerateEdge<edgeType>::ScEventAfterGenerateEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAfterGenerateConnector<edgeType>(
        ScEventAfterGenerateEdge<edgeType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventAfterGenerateEdge<edgeType>::GetConnector() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventAfterGenerateEdge<edgeType>::GetConnectorType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventAfterGenerateEdge<edgeType>::GetConnectorIncidentElements() const
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
    ScAddr const & otherAddr)
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
ScAddr ScEventAfterGenerateIncomingArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventAfterGenerateIncomingArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
_SC_EXTERN ScEventAfterGenerateIncomingArc<arcType>::ScEventAfterGenerateIncomingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
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
ScAddr ScEventBeforeEraseConnector<connectorType>::GetConnector() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & connectorType>
ScType ScEventBeforeEraseConnector<connectorType>::GetConnectorType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & connectorType>
std::tuple<ScAddr, ScAddr> ScEventBeforeEraseConnector<connectorType>::GetConnectorIncidentElements() const
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
    ScAddr const & otherAddr)
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
    ScAddr const & otherAddr)
  : ScEventBeforeEraseConnector<tConnectorType>(
        ScEventBeforeEraseConnector::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

// Ignore these methods, because they can't be used.
// LCOV_EXCL_START
template <ScType const & connectorType>
ScAddr ScEventBeforeEraseConnector<connectorType>::GetOtherElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

// LCOV_EXCL_END

// ScEventBeforeEraseArc
//-------------------------------------------------------------------------

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetArc() const
{
  return GetConnector();
}

template <ScType const & arcType>
ScType ScEventBeforeEraseArc<arcType>::GetArcType() const
{
  return GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetArcTargetElement() const
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
    ScAddr const & otherAddr)
  : ScEventBeforeEraseConnector<arcType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScAddr ScEventBeforeEraseArc<arcType>::GetConnector() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventBeforeEraseArc<arcType>::GetConnectorType() const
{
  return ScElementaryEvent::GetConnectorType();
}

// Ignore these methods, because they can't be used.
// LCOV_EXCL_START
template <ScType const & arcType>
std::tuple<ScAddr, ScAddr> ScEventBeforeEraseArc<arcType>::GetConnectorIncidentElements() const
{
  return {ScElementaryEvent::GetSubscriptionElement(), ScElementaryEvent::GetOtherElement()};
}

// LCOV_EXCL_END

// ScEventBeforeEraseEdge
//-------------------------------------------------------------------------

template <ScType const & edgeType>
ScAddr ScEventBeforeEraseEdge<edgeType>::GetEdge() const
{
  return GetConnector();
}

template <ScType const & edgeType>
ScType ScEventBeforeEraseEdge<edgeType>::GetEdgeType() const
{
  return GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventBeforeEraseEdge<edgeType>::GetEdgeIncidentElements() const
{
  return GetConnectorIncidentElements();
}

template <ScType const & edgeType>
ScEventBeforeEraseEdge<edgeType>::ScEventBeforeEraseEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventBeforeEraseConnector<edgeType>(
        ScEventBeforeEraseEdge<edgeType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventBeforeEraseEdge<edgeType>::GetConnector() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventBeforeEraseEdge<edgeType>::GetConnectorType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
std::tuple<ScAddr, ScAddr> ScEventBeforeEraseEdge<edgeType>::GetConnectorIncidentElements() const
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
    ScAddr const & otherAddr)
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
ScAddr ScEventBeforeEraseIncomingArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventBeforeEraseIncomingArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScEventBeforeEraseIncomingArc<arcType>::ScEventBeforeEraseIncomingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventBeforeEraseArc<arcType>(
        ScEventBeforeEraseIncomingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};
