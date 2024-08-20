/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

template <ScType const & elementType>
TScElementaryEvent<elementType>::TScElementaryEvent(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(eventClassAddr, userAddr, subscriptionElementAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & arcType>
ScAddr ScEventAddArc<arcType>::GetAddedArc() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventAddArc<arcType>::GetAddedArcType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventAddArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventAddArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScEventAddArc<arcType>::ScEventAddArc(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<arcType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScEventAddEdge<edgeType>::ScEventAddEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<edgeType>(
        ScEventAddEdge<edgeType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventAddEdge<edgeType>::GetAddedEdge() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventAddEdge<edgeType>::GetAddedEdgeType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
ScAddr ScEventAddEdge<edgeType>::GetEdgeSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & edgeType>
ScAddr ScEventAddEdge<edgeType>::GetEdgeTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScEventAddOutgoingArc<arcType>::ScEventAddOutgoingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddArc<arcType>(
        ScEventAddOutgoingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScAddr ScEventAddIncomingArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventAddIncomingArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
_SC_EXTERN ScEventAddIncomingArc<arcType>::ScEventAddIncomingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddArc<arcType>(
        ScEventAddIncomingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr) {};

template <ScType const & arcType>
ScAddr ScEventEraseArc<arcType>::GetErasableArc() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventEraseArc<arcType>::GetErasableArcType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventEraseArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventEraseArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScEventEraseArc<arcType>::ScEventEraseArc(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<arcType>(
        eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventEraseEdge<edgeType>::GetErasableEdge() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventEraseEdge<edgeType>::GetErasableEdgeType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
ScAddr ScEventEraseEdge<edgeType>::GetEdgeSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & edgeType>
ScAddr ScEventEraseEdge<edgeType>::GetEdgeTargetElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & edgeType>
ScEventEraseEdge<edgeType>::ScEventEraseEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<edgeType>(
        ScEventEraseEdge<edgeType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScEventEraseOutgoingArc<arcType>::ScEventEraseOutgoingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventEraseArc<arcType>(
        ScEventEraseOutgoingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScAddr ScEventEraseIncomingArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventEraseIncomingArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScEventEraseIncomingArc<arcType>::ScEventEraseIncomingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventEraseArc<arcType>(
        ScEventEraseIncomingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};
