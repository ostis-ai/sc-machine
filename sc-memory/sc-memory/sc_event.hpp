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
public:
  _SC_EXTERN virtual ~ScEvent();

  _SC_EXTERN virtual ScAddr GetEventClass() const = 0;

  _SC_EXTERN virtual ScAddr GetUser() const = 0;

  _SC_EXTERN virtual ScAddr GetSubscriptionElement() const = 0;

protected:
  friend class ScElementaryEventSubscription;
  friend class ScEventSubscriptionFactory;
};

/*!
 * @class ScEvent
 * @brief Represents a basic sc-event.
 *
 * ScEvent is a subclass of ScEvent that represents basic events in the sc-memory.
 */
class _SC_EXTERN ScElementaryEvent : public ScEvent
{
public:
  _SC_EXTERN ScAddr GetEventClass() const override;

  _SC_EXTERN ScAddr GetUser() const override;

  _SC_EXTERN ScAddr GetSubscriptionElement() const override;

  /*!
   * @brief Gets a triple associated with the event.
   * @return An array representing the triple.
   */
  _SC_EXTERN std::tuple<ScAddr, ScAddr, ScAddr> GetTriple() const;

  /*!
   * @brief Gets a connector element outgoing from or ingoing to a subscription element.
   * @return ScAddr representing a connector element from/to a subscription element.
   */
  _SC_EXTERN ScAddr GetConnector() const;

  /*!
   * @brief Gets a type of connector element outgoing from or ingoing to a subscription element.
   * @return ScType representing a type of connector element outgoing from or ingoing to a subscription element.
   */
  _SC_EXTERN ScType GetConnectorType() const;

  /*!
   * @brief Gets other element of connector outgoing from or ingoing to subscription element.
   * @return ScAddr representing other element of connector outgoing from or ingoing to subscription element.
   */
  _SC_EXTERN ScAddr GetOtherElement() const;

protected:
  _SC_EXTERN ScElementaryEvent(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

  ScAddr m_eventClassAddr;
  ScAddr m_userAddr;
  ScAddr m_subscriptionAddr;
  ScAddr m_connectorAddr;
  ScType m_connectorType;
  ScAddr m_otherAddr;

private:
  friend class ScElementaryEventSubscription;
};

/*!
 * @class ScElementaryEvent
 * @brief Represents a basic sc-event.
 *
 * ScElementaryEvent is a subclass of ScEvent that represents basic events in the sc-memory.
 */
template <ScType const & _elementType>
class _SC_EXTERN TScElementaryEvent : public ScElementaryEvent
{
protected:
  _SC_EXTERN TScElementaryEvent(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScType const & elementType = _elementType;
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_unknown;
};

/*!
 * @class ScEventAddArc
 * @brief Represents an event where an arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAddArc : public TScElementaryEvent<arcType>
{
public:
  _SC_EXTERN virtual ScAddr GetAddedArc() const;

  _SC_EXTERN virtual ScType GetAddedArcType() const;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const;

protected:
  _SC_EXTERN ScEventAddArc(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);
};

/*!
 * @class ScEventAddEdge
 * @brief Represents an event where an edge is added.
 */
template <ScType const & edgeType>
class _SC_EXTERN ScEventAddEdge : public TScElementaryEvent<edgeType>
{
public:
  _SC_EXTERN virtual ScAddr GetAddedEdge() const;

  _SC_EXTERN virtual ScType GetAddedEdgeType() const;

  _SC_EXTERN virtual ScAddr GetEdgeSourceElement() const;

  _SC_EXTERN virtual ScAddr GetEdgeTargetElement() const;

protected:
  _SC_EXTERN ScEventAddEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_add_edge;
};

/*!
 * @class ScEventAddOutputArc
 * @brief Represents an event where an output arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAddOutputArc : public ScEventAddArc<arcType>
{
protected:
  _SC_EXTERN ScEventAddOutputArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_add_output_arc;
};

/*!
 * @class ScEventAddInputArc
 * @brief Represents an event where an input arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAddInputArc final : public ScEventAddArc<arcType>
{
public:
  _SC_EXTERN ScAddr GetArcSourceElement() const override;

  _SC_EXTERN ScAddr GetArcTargetElement() const override;

protected:
  _SC_EXTERN ScEventAddInputArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_add_input_arc;
};

/*!
 * @class ScEventRemoveArc
 * @brief Represents an event where an arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventRemoveArc : public TScElementaryEvent<arcType>
{
public:
  _SC_EXTERN virtual ScAddr GetRemovableArc() const;

  _SC_EXTERN virtual ScType GetRemovableArcType() const;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const;

protected:
  _SC_EXTERN ScEventRemoveArc(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);
};

/*!
 * @class ScEventRemoveEdge
 * @brief Represents an event where an edge is removed.
 */
template <ScType const & edgeType>
class _SC_EXTERN ScEventRemoveEdge : public TScElementaryEvent<edgeType>
{
public:
  _SC_EXTERN virtual ScAddr GetRemovableEdge() const;

  _SC_EXTERN virtual ScType GetRemovableEdgeType() const;

  _SC_EXTERN virtual ScAddr GetEdgeSourceElement() const;

  _SC_EXTERN virtual ScAddr GetEdgeTargetElement() const;

protected:
  _SC_EXTERN ScEventRemoveEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_remove_edge;
};

/*!
 * @class ScEventRemoveOutputArc
 * @brief Represents an event where an output arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventRemoveOutputArc final : public ScEventRemoveArc<arcType>
{
protected:
  _SC_EXTERN ScEventRemoveOutputArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_remove_output_arc;
};

/*!
 * @class ScEventRemoveInputArc
 * @brief Represents an event where an input arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventRemoveInputArc final : public ScEventRemoveArc<arcType>
{
public:
  _SC_EXTERN ScAddr GetArcSourceElement() const override;

  _SC_EXTERN ScAddr GetArcTargetElement() const override;

protected:
  _SC_EXTERN ScEventRemoveInputArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_remove_input_arc;
};

#include "sc_event.tpp"

/*!
 * @class ScEventRemoveElement
 * @brief Represents an event where an element is erased.
 */
class _SC_EXTERN ScEventRemoveElement final : public TScElementaryEvent<ScType::Unknown>
{
protected:
  _SC_EXTERN ScEventRemoveElement(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_remove_element;
};

/*!
 * @class ScEventChangeLinkContent
 * @brief Represents an event where the content of an element is changed.
 */
class _SC_EXTERN ScEventChangeLinkContent final : public TScElementaryEvent<ScType::Unknown>
{
protected:
  _SC_EXTERN ScEventChangeLinkContent(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class TScElementaryEventSubscription;
  template <class TScEvent>
  friend class ScAgentAbstract;

  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_change_content;
};

using ScActionEvent = ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>;
