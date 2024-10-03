/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_event.hpp"

// ScEvent
//-------------------------------------------------------------------------

ScEvent::~ScEvent() noexcept = default;

// ScElementaryEvent
//-------------------------------------------------------------------------

ScElementaryEvent::ScElementaryEvent(
    ScAddr const & eventClassAddr,
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : m_eventClassAddr(eventClassAddr)
  , m_userAddr(userAddr)
  , m_subscriptionAddr(subscriptionElementAddr)
  , m_connectorAddr(connectorAddr)
  , m_connectorType(connectorType)
  , m_otherAddr(otherAddr){};

ScAddr ScElementaryEvent::GetEventClass() const noexcept
{
  return m_eventClassAddr;
}

ScAddr ScElementaryEvent::GetUser() const noexcept
{
  return m_userAddr;
}

ScAddr ScElementaryEvent::GetSubscriptionElement() const noexcept
{
  return m_subscriptionAddr;
}

std::tuple<ScAddr, ScAddr, ScAddr> ScElementaryEvent::GetTriple() const noexcept
{
  return {m_subscriptionAddr, m_connectorAddr, m_otherAddr};
}

ScAddr ScElementaryEvent::GetConnector() const noexcept
{
  return m_connectorAddr;
}

ScType ScElementaryEvent::GetConnectorType() const noexcept
{
  return m_connectorType;
}

ScAddr ScElementaryEvent::GetOtherElement() const noexcept
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
    ScAddr const & otherAddr) noexcept
  : TScElementaryEvent(
        ScEventBeforeEraseElement::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};

// ScEventBeforeChangeLinkContent
//-------------------------------------------------------------------------

ScEventBeforeChangeLinkContent::ScEventBeforeChangeLinkContent(
    ScAddr const & userAddr,
    ScAddr const & subscriptionElementAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr) noexcept
  : TScElementaryEvent(
        ScEventBeforeChangeLinkContent::eventClassAddr,
        userAddr,
        subscriptionElementAddr,
        connectorAddr,
        connectorType,
        otherAddr){};
