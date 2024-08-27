/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

// ScEvent
//-------------------------------------------------------------------------

ScEvent::~ScEvent() = default;

// ScElementaryEvent
//-------------------------------------------------------------------------

ScElementaryEvent::ScElementaryEvent(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : m_eventClassAddr(eventClassAddr)
  , m_userAddr(userAddr)
  , m_subscriptionAddr(subscriptionElementAddr)
  , m_connectorAddr(connectorAddr)
  , m_connectorType(connectorType)
  , m_otherAddr(otherAddr) {};

ScAddr ScElementaryEvent::GetEventClass() const
{
  return m_eventClassAddr;
}

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

// ScEventBeforeEraseElement
//-------------------------------------------------------------------------

ScEventBeforeEraseElement::ScEventBeforeEraseElement(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent(
        ScEventBeforeEraseElement::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr) {};

// Ignore these methods, because they can't be used.
// LCOV_EXCL_START
ScAddr ScEventBeforeEraseElement::GetConnector() const
{
  return ScElementaryEvent::GetConnector();
}

ScType ScEventBeforeEraseElement::GetConnectorType() const
{
  return ScElementaryEvent::GetConnectorType();
}

ScAddr ScEventBeforeEraseElement::GetOtherElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

// LCOV_EXCL_END

// ScEventBeforeChangeLinkContent
//-------------------------------------------------------------------------

ScEventBeforeChangeLinkContent::ScEventBeforeChangeLinkContent(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : TScElementaryEvent(
        ScEventBeforeChangeLinkContent::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr) {};

// Ignore these methods, because they can't be used.
// LCOV_EXCL_START
ScAddr ScEventBeforeChangeLinkContent::GetConnector() const
{
  return ScElementaryEvent::GetConnector();
}

ScType ScEventBeforeChangeLinkContent::GetConnectorType() const
{
  return ScElementaryEvent::GetConnectorType();
}

ScAddr ScEventBeforeChangeLinkContent::GetOtherElement() const
{
  return ScElementaryEvent::GetOtherElement();
}

// LCOV_EXCL_END
