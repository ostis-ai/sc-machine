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
ScAddr ScEventGenerateArc<arcType>::GetGeneratedArc() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventGenerateArc<arcType>::GetGeneratedArcType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventGenerateArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventGenerateArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScEventGenerateArc<arcType>::ScEventGenerateArc(
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
ScEventGenerateEdge<edgeType>::ScEventGenerateEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<edgeType>(
        ScEventGenerateEdge<edgeType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventGenerateEdge<edgeType>::GetGeneratedEdge() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventGenerateEdge<edgeType>::GetGeneratedEdgeType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
ScAddr ScEventGenerateEdge<edgeType>::GetEdgeSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & edgeType>
ScAddr ScEventGenerateEdge<edgeType>::GetEdgeTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScEventGenerateOutgoingArc<arcType>::ScEventGenerateOutgoingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventGenerateArc<arcType>(
        ScEventGenerateOutgoingArc<arcType>::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

template <ScType const & arcType>
ScAddr ScEventGenerateIncomingArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventGenerateIncomingArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
_SC_EXTERN ScEventGenerateIncomingArc<arcType>::ScEventGenerateIncomingArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventGenerateArc<arcType>(
        ScEventGenerateIncomingArc<arcType>::eventClassAddr,
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
