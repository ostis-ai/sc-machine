/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

template <ScType const & elementType>
ScAddr ScElementaryEvent<elementType>::GetUser() const
{
  return m_userAddr;
}

template <ScType const & elementType>
ScAddr ScElementaryEvent<elementType>::GetSubscriptionElement() const
{
  return m_subscriptionAddr;
}

template <ScType const & elementType>
std::tuple<ScAddr, ScAddr, ScAddr> ScElementaryEvent<elementType>::GetTriple() const
{
  return {m_subscriptionAddr, m_connectorAddr, m_otherAddr};
}

template <ScType const & elementType>
ScAddr ScElementaryEvent<elementType>::GetConnector() const
{
  return m_connectorAddr;
}

template <ScType const & elementType>
ScType ScElementaryEvent<elementType>::GetConnectorType() const
{
  return m_connectorType;
}

template <ScType const & elementType>
ScAddr ScElementaryEvent<elementType>::GetOtherElement() const
{
  return m_otherAddr;
}

template <ScType const & elementType>
ScElementaryEvent<elementType>::ScElementaryEvent(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : m_userAddr(userAddr)
  , m_subscriptionAddr(subscriptionAddr)
  , m_connectorAddr(connectorAddr)
  , m_connectorType(connectorType)
  , m_otherAddr(otherAddr){};

template <ScType const & arcType>
ScAddr ScEventAddArc<arcType>::GetAddedArc() const
{
  return ScElementaryEvent<arcType>::GetConnector();
}

template <ScType const & arcType>
ScType ScEventAddArc<arcType>::GetAddedArcType() const
{
  return ScElementaryEvent<arcType>::GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventAddArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent<arcType>::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventAddArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent<arcType>::GetOtherElement();
}

template <ScType const & arcType>
ScEventAddArc<arcType>::ScEventAddArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & edgeType>
ScEventAddEdge<edgeType>::ScEventAddEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent<edgeType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & edgeType>
ScAddr ScEventAddEdge<edgeType>::GetAddedEdge() const
{
  return ScElementaryEvent<edgeType>::GetConnector();
}

template <ScType const & edgeType>
ScType ScEventAddEdge<edgeType>::GetAddedEdgeType() const
{
  return ScElementaryEvent<edgeType>::GetConnectorType();
}

template <ScType const & edgeType>
ScAddr ScEventAddEdge<edgeType>::GetEdgeSourceElement() const
{
  return ScElementaryEvent<edgeType>::GetOtherElement();
}

template <ScType const & edgeType>
ScAddr ScEventAddEdge<edgeType>::GetEdgeTargetElement() const
{
  return ScElementaryEvent<edgeType>::GetSubscriptionElement();
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
_SC_EXTERN ScEventAddInputArc<arcType>::ScEventAddInputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddArc<arcType>(userAddr, otherAddr, connectorAddr, connectorType, subscriptionAddr) {};

template <ScType const & arcType>
ScType ScEventRemoveArc<arcType>::GetRemovedArcType() const
{
  return ScElementaryEvent<arcType>::GetConnectorType();
}

template <ScType const & arcType>
ScAddr ScEventRemoveArc<arcType>::GetArcSourceElement() const
{
  return ScElementaryEvent<arcType>::GetSubscriptionElement();
}

template <ScType const & arcType>
ScAddr ScEventRemoveArc<arcType>::GetArcTargetElement() const
{
  return ScElementaryEvent<arcType>::GetOtherElement();
}

template <ScType const & arcType>
ScEventRemoveArc<arcType>::ScEventRemoveArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & edgeType>
ScType ScEventRemoveEdge<edgeType>::GetRemovedEdgeType() const
{
  return ScElementaryEvent<edgeType>::GetConnectorType();
}

template <ScType const & edgeType>
ScAddr ScEventRemoveEdge<edgeType>::GetEdgeSourceElement() const
{
  return ScElementaryEvent<edgeType>::GetSubscriptionElement();
}

template <ScType const & edgeType>
ScAddr ScEventRemoveEdge<edgeType>::GetEdgeTargetElement() const
{
  return ScElementaryEvent<edgeType>::GetOtherElement();
}

template <ScType const & edgeType>
ScEventRemoveEdge<edgeType>::ScEventRemoveEdge(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScElementaryEvent<edgeType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & arcType>
ScEventRemoveOutputArc<arcType>::ScEventRemoveOutputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveArc<arcType>(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

template <ScType const & arcType>
ScEventRemoveInputArc<arcType>::ScEventRemoveInputArc(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventRemoveArc<arcType>(userAddr, otherAddr, connectorAddr, connectorType, subscriptionAddr){};
