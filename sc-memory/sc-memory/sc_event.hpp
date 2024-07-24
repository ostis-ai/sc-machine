/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_type.hpp"

extern "C"
{
#include "sc-core/sc-store/sc-event/sc_event_types.h"
}

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

  _SC_EXTERN virtual ScAddr GetUser() const = 0;

  _SC_EXTERN virtual ScAddr GetSubscriptionElement() const = 0;

protected:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  friend class ScEventSubscriptionFactory;

  /*!
   * @class Type
   * @brief Represents a type of an sc-event.
   */
  class Type
  {
  public:
    sc_event_type m_realType;

    explicit constexpr Type(sc_event_type type)
      : m_realType(type)
    {
    }

    ~Type() = default;

    constexpr operator sc_event_type() const
    {
      return m_realType;
    }

    static Type const Unknown;
    static Type const AddInputArc;
    static Type const AddOutputArc;
    static Type const AddEdge;
    static Type const RemoveInputArc;
    static Type const RemoveOutputArc;
    static Type const RemoveEdge;
    static Type const EraseElement;
    static Type const ChangeContent;
  };

protected:
  static inline Type const type = Type::Unknown;
};

/*!
 * @class ScElementaryEvent
 * @brief Represents a basic sc-event.
 *
 * ScElementaryEvent is a subclass of ScEvent that represents basic events in the sc-memory.
 */
template <ScType const & _elementType>
class _SC_EXTERN ScElementaryEvent : public ScEvent
{
public:
  _SC_EXTERN ScAddr GetUser() const override;

  _SC_EXTERN ScAddr GetSubscriptionElement() const override;

  /*!
   * @brief Gets a triple associated with the event.
   * @return An array representing the triple.
   */
  _SC_EXTERN std::tuple<ScAddr, ScAddr, ScAddr> GetTriple() const;

protected:
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
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

  ScAddr m_userAddr;
  ScAddr m_subscriptionAddr;
  ScAddr m_connectorAddr;
  ScType m_connectorType;
  ScAddr m_otherAddr;

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline ScType const & elementType = _elementType;
  static inline Type const type = Type::Unknown;
};

/*!
 * @class ScEventAddArc
 * @brief Represents an event where an arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAddArc : public ScElementaryEvent<arcType>
{
public:
  _SC_EXTERN virtual ScAddr GetAddedArc() const;

  _SC_EXTERN virtual ScType GetAddedArcType() const;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const;

protected:
  _SC_EXTERN ScEventAddArc(
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
class _SC_EXTERN ScEventAddEdge : public ScElementaryEvent<edgeType>
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
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::AddEdge;
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
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::AddOutputArc;
};

/*!
 * @class ScEventAddInputArc
 * @brief Represents an event where an input arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventAddInputArc final : public ScEventAddArc<arcType>
{
protected:
  _SC_EXTERN ScEventAddInputArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::AddInputArc;
};

/*!
 * @class ScEventRemoveArc
 * @brief Represents an event where an arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventRemoveArc : public ScElementaryEvent<arcType>
{
public:
  _SC_EXTERN virtual ScType GetRemovedArcType() const;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const;

protected:
  _SC_EXTERN ScEventRemoveArc(
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
class _SC_EXTERN ScEventRemoveEdge : public ScElementaryEvent<edgeType>
{
public:
  _SC_EXTERN virtual ScType GetRemovedEdgeType() const;

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
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::RemoveEdge;
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
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::RemoveOutputArc;
};

/*!
 * @class ScEventRemoveInputArc
 * @brief Represents an event where an input arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventRemoveInputArc final : public ScEventRemoveArc<arcType>
{
protected:
  _SC_EXTERN ScEventRemoveInputArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::RemoveInputArc;
};

#include "sc_event.tpp"

/*!
 * @class ScEventEraseElement
 * @brief Represents an event where an element is erased.
 */
class _SC_EXTERN ScEventEraseElement final : public ScElementaryEvent<ScType::Unknown>
{
protected:
  _SC_EXTERN ScEventEraseElement(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  _SC_EXTERN ScAddr GetSubscriptionElement() const final;

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::EraseElement;
};

/*!
 * @class ScEventChangeContent
 * @brief Represents an event where the content of an element is changed.
 */
class _SC_EXTERN ScEventChangeContent final : public ScElementaryEvent<ScType::Unknown>
{
protected:
  _SC_EXTERN ScEventChangeContent(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline ScEvent::Type const type = ScEvent::Type::ChangeContent;
};

using ScActionEvent = ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>;
