/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

template <ScType const & elementType>
TScElementaryEvent<elementType>::TScElementaryEvent(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

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
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & edgeType>
ScEventAddEdge<edgeType>::ScEventAddEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<edgeType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

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
ScEventAddOutputArc<arcType>::ScEventAddOutputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddArc<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & arcType>
ScAddr ScEventAddInputArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventAddInputArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
_SC_EXTERN ScEventAddInputArc<arcType>::ScEventAddInputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddArc<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr) {};

template <ScType const & arcType>
ScAddr ScEventRemoveArc<arcType>::GetRemovableArc() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & arcType>
ScType ScEventRemoveArc<arcType>::GetRemovableArcType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventRemoveArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventRemoveArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScEventRemoveArc<arcType>::ScEventRemoveArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventRemoveEdge<edgeType>::GetRemovableEdge() const
{
  return ScElementaryEvent::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventRemoveEdge<edgeType>::GetRemovableEdgeType() const
{
  return ScElementaryEvent::GetConnectorType();
}

template <ScType const & edgeType>
ScAddr ScEventRemoveEdge<edgeType>::GetEdgeSourceElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & edgeType>
ScAddr ScEventRemoveEdge<edgeType>::GetEdgeTargetElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & edgeType>
ScEventRemoveEdge<edgeType>::ScEventRemoveEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent<edgeType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & arcType>
ScEventRemoveOutputArc<arcType>::ScEventRemoveOutputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveArc<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & arcType>
ScAddr ScEventRemoveInputArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

template <ScType const & arcType>
ScAddr ScEventRemoveInputArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent::GetSubscriptionElement();
}

template <ScType const & arcType>
ScEventRemoveInputArc<arcType>::ScEventRemoveInputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveArc<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};
