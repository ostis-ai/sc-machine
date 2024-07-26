/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

ScEvent::~ScEvent() = default;

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
  , m_otherAddr(otherAddr){};

ScAddr ScElementaryEvent::GetUser() const
{
  return m_userAddr;
}

ScAddr ScElementaryEvent::GetSubscriptionElement() const
{
  return m_subscriptionAddr;
}

std::tuple<ScAddr, ScAddr, ScAddr> ScElementaryEvent::GetTriple() const
{
  return {m_subscriptionAddr, m_connectorAddr, m_otherAddr};
}

ScAddr ScElementaryEvent::GetConnector() const
{
  return m_connectorAddr;
}

ScType ScElementaryEvent::GetConnectorType() const
{
  return m_connectorType;
}

ScAddr ScElementaryEvent::GetOtherElement() const
{
  return m_otherAddr;
}

ScEventEraseElement::ScEventEraseElement(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};

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
  : TScElementaryEvent(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr){};
