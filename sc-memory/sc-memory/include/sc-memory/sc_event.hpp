/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_type.hpp"

#include "sc_keynodes.hpp"

/*!
 * @class ScEvent
 * @brief Base class for all sc-events.
 *
 * ScEvent is an abstract class that represents a generic event in the sc-memory.
 * It provides interfaces to get a user and a subscription element associated with the event.
 */
class _SC_EXTERN ScEvent : public ScObject
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

public:
  _SC_EXTERN virtual ~ScEvent() noexcept;

  _SC_EXTERN virtual ScAddr GetEventClass() const noexcept = 0;

  _SC_EXTERN virtual ScAddr GetUser() const noexcept = 0;

  _SC_EXTERN virtual ScAddr GetSubscriptionElement() const noexcept = 0;
};

/*!
 * @class ScElementaryEvent
 * @brief Represents a elementary (basic) sc-event.
 *
 * ScElementaryEvent is a subclass of ScEvent that represents basic events in the sc-memory.
 */
class _SC_EXTERN ScElementaryEvent : public ScEvent
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

public:
  _SC_EXTERN ScAddr GetEventClass() const noexcept override;

  _SC_EXTERN ScAddr GetUser() const noexcept override;

  _SC_EXTERN ScAddr GetSubscriptionElement() const noexcept override;

  /*!
   * @brief Gets a triple associated with the event.
   * @return An array representing the triple.
   */
  _SC_EXTERN std::tuple<ScAddr, ScAddr, ScAddr> GetTriple() const noexcept;

  /*!
   * @brief Gets a connector element outgoing from or incoming to a subscription element.
   * @return ScAddr representing a connector element from/to a subscription element.
   */
  _SC_EXTERN virtual ScAddr GetConnector() const noexcept;

  /*!
   * @brief Gets a type of connector element outgoing from or incoming to a subscription element.
   * @return ScType representing a type of connector element outgoing from or incoming to a subscription element.
   */
  _SC_EXTERN virtual ScType GetConnectorType() const noexcept;

  /*!
   * @brief Gets other element of connector outgoing from or incoming to subscription element.
   * @return ScAddr representing other element of connector outgoing from or incoming to subscription element.
   */
  _SC_EXTERN virtual ScAddr GetOtherElement() const noexcept;

protected:
  _SC_EXTERN ScElementaryEvent(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  ScAddr m_eventClassAddr;
  ScAddr m_userAddr;
  ScAddr m_subscriptionAddr;
  ScAddr m_connectorAddr;
  ScType m_connectorType;
  ScAddr m_otherAddr;
};

/*!
 * @class TScElementaryEvent
 * @brief Represents a elementary (basic) sc-event.
 *
 * TScElementaryEvent is a subclass of ScElementaryEvent that represents elementary events in the sc-memory. With this
 * class, sc-event types can be defined statically.
 */
template <ScType const & _elementType>
class _SC_EXTERN TScElementaryEvent : public ScElementaryEvent
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

protected:
  _SC_EXTERN TScElementaryEvent(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  static inline ScType const & elementType = _elementType;
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_unknown;
};

/*!
 * @class ScEventAfterGenerateConnector
 * @brief Represents an event where a sc-connector is generated.
 */
template <ScType const & tConnectorType>
class _SC_EXTERN ScEventAfterGenerateConnector : public TScElementaryEvent<tConnectorType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

public:
  _SC_EXTERN virtual std::tuple<ScAddr, ScAddr> GetConnectorIncidentElements() const noexcept;

protected:
  _SC_EXTERN ScEventAfterGenerateConnector(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  _SC_EXTERN ScEventAfterGenerateConnector(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_after_generate_connector;
};

/*!
 * @class ScEventAfterGenerateArc
 * @brief Represents an event where a sc-arc is generated.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAfterGenerateArc : public ScEventAfterGenerateConnector<arcType>
{
public:
  _SC_EXTERN virtual ScAddr GetArc() const noexcept;

  _SC_EXTERN virtual ScType GetArcType() const noexcept;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const noexcept;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const noexcept;

protected:
  _SC_EXTERN ScEventAfterGenerateArc(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  _SC_EXTERN ScAddr GetConnector() const noexcept override;

  _SC_EXTERN ScType GetConnectorType() const noexcept override;

  using ScEventAfterGenerateConnector<arcType>::GetConnectorIncidentElements;
};

/*!
 * @class ScEventAfterGenerateEdge
 * @brief Represents an event where a sc-edge is generated.
 */
template <ScType const & edgeType>
class _SC_EXTERN ScEventAfterGenerateEdge final : public ScEventAfterGenerateConnector<edgeType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

public:
  _SC_EXTERN virtual ScAddr GetEdge() const noexcept;

  _SC_EXTERN virtual ScType GetEdgeType() const noexcept;

  _SC_EXTERN virtual std::tuple<ScAddr, ScAddr> GetEdgeIncidentElements() const noexcept;

protected:
  _SC_EXTERN ScEventAfterGenerateEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  _SC_EXTERN ScAddr GetConnector() const noexcept override;

  _SC_EXTERN ScType GetConnectorType() const noexcept override;

  _SC_EXTERN std::tuple<ScAddr, ScAddr> GetConnectorIncidentElements() const noexcept override;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_after_generate_edge;
};

/*!
 * @class ScEventAfterGenerateOutgoingArc
 * @brief Represents an event where an outgoing sc-arc is generated.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAfterGenerateOutgoingArc final : public ScEventAfterGenerateArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

protected:
  _SC_EXTERN ScEventAfterGenerateOutgoingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_after_generate_outgoing_arc;
};

/*!
 * @class ScEventAfterGenerateIncomingArc
 * @brief Represents an event where an incoming sc-arc is generated.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAfterGenerateIncomingArc final : public ScEventAfterGenerateArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

public:
  _SC_EXTERN ScAddr GetArcSourceElement() const noexcept;

  _SC_EXTERN ScAddr GetArcTargetElement() const noexcept;

protected:
  _SC_EXTERN ScEventAfterGenerateIncomingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_after_generate_incoming_arc;
};

/*!
 * @class ScEventBeforeEraseConnector
 * @brief Represents an event where a sc-connector is erased.
 */
template <ScType const & tConnectorType>
class _SC_EXTERN ScEventBeforeEraseConnector : public TScElementaryEvent<tConnectorType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

public:
  _SC_EXTERN ScAddr GetConnector() const noexcept override;

  _SC_EXTERN ScType GetConnectorType() const noexcept override;

  _SC_EXTERN virtual std::tuple<ScAddr, ScAddr> GetConnectorIncidentElements() const noexcept;

protected:
  _SC_EXTERN ScEventBeforeEraseConnector(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  _SC_EXTERN ScEventBeforeEraseConnector(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_before_erase_connector;
};

/*!
 * @class ScEventBeforeEraseArc
 * @brief Represents an event where a sc-arc is erased.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventBeforeEraseArc : public ScEventBeforeEraseConnector<arcType>
{
public:
  _SC_EXTERN virtual ScAddr GetArc() const noexcept;

  _SC_EXTERN virtual ScType GetArcType() const noexcept;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const noexcept;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const noexcept;

protected:
  _SC_EXTERN ScEventBeforeEraseArc(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  _SC_EXTERN ScAddr GetConnector() const noexcept;

  _SC_EXTERN ScType GetConnectorType() const noexcept;

  using ScEventBeforeEraseConnector<arcType>::GetConnectorIncidentElements;
};

/*!
 * @class ScEventBeforeEraseEdge
 * @brief Represents an event where a sc-edge is erased.
 */
template <ScType const & edgeType>
class _SC_EXTERN ScEventBeforeEraseEdge final : public ScEventBeforeEraseConnector<edgeType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

public:
  _SC_EXTERN virtual ScAddr GetEdge() const noexcept;

  _SC_EXTERN virtual ScType GetEdgeType() const noexcept;

  _SC_EXTERN virtual std::tuple<ScAddr, ScAddr> GetEdgeIncidentElements() const noexcept;

protected:
  _SC_EXTERN ScEventBeforeEraseEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  _SC_EXTERN ScAddr GetConnector() const noexcept override;

  _SC_EXTERN ScType GetConnectorType() const noexcept override;

  _SC_EXTERN std::tuple<ScAddr, ScAddr> GetConnectorIncidentElements() const noexcept override;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_before_erase_edge;
};

/*!
 * @class ScEventBeforeEraseOutgoingArc
 * @brief Represents an event where an outgoing sc-arc is erased.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventBeforeEraseOutgoingArc final : public ScEventBeforeEraseArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

protected:
  _SC_EXTERN ScEventBeforeEraseOutgoingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_before_erase_outgoing_arc;
};

/*!
 * @class ScEventBeforeEraseIncomingArc
 * @brief Represents an event where an incoming sc-arc is erased.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventBeforeEraseIncomingArc final : public ScEventBeforeEraseArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

public:
  _SC_EXTERN ScAddr GetArcSourceElement() const noexcept;

  _SC_EXTERN ScAddr GetArcTargetElement() const noexcept;

protected:
  _SC_EXTERN ScEventBeforeEraseIncomingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_before_erase_incoming_arc;
};

#include "sc-memory/_template/sc_event.tpp"

/*!
 * @class ScEventBeforeEraseElement
 * @brief Represents an event where an element is erased.
 */
class _SC_EXTERN ScEventBeforeEraseElement final : public TScElementaryEvent<ScType::Unknown>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

protected:
  _SC_EXTERN ScEventBeforeEraseElement(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  using TScElementaryEvent::GetConnector;
  using TScElementaryEvent::GetConnectorType;
  using TScElementaryEvent::GetOtherElement;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_before_erase_element;
};

/*!
 * @class ScEventBeforeChangeLinkContent
 * @brief Represents an event where the content of an element is changed.
 */
class _SC_EXTERN ScEventBeforeChangeLinkContent final : public TScElementaryEvent<ScType::Unknown>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgent;
  template <class TScAgent>
  friend class ScAgentManager;

protected:
  _SC_EXTERN ScEventBeforeChangeLinkContent(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr) noexcept;

  using TScElementaryEvent::GetConnector;
  using TScElementaryEvent::GetConnectorType;
  using TScElementaryEvent::GetOtherElement;

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_before_change_link_content;
};

using ScActionInitiatedEvent = ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>;
