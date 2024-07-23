/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_type.hpp"

#include "sc-core/sc-store/sc-event/sc_event_types.h"

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

class _SC_EXTERN ScElementaryEvent : public ScEvent
{
public:
  _SC_EXTERN ScAddr GetUser() const override;

  _SC_EXTERN ScAddr GetSubscriptionElement() const override;

  _SC_EXTERN std::array<ScAddr, 3> GetTriple() const;

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

  static inline Type const type = Type::Unknown;
};

class _SC_EXTERN ScEventAddArc : public ScElementaryEvent
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

class _SC_EXTERN ScEventAddEdge : public ScElementaryEvent
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

  static inline Type const type = Type::AddEdge;
};

class _SC_EXTERN ScEventAddOutputArc : public ScEventAddArc
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

  static inline Type const type = Type::AddOutputArc;
};

class _SC_EXTERN ScEventAddInputArc final : public ScEventAddArc
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

  static inline Type const type = Type::AddInputArc;
};

class _SC_EXTERN ScEventRemoveArc : public ScElementaryEvent
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

class _SC_EXTERN ScEventRemoveEdge : public ScElementaryEvent
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

  static inline Type const type = Type::RemoveEdge;
};

class _SC_EXTERN ScEventRemoveOutputArc final : public ScEventRemoveArc
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

  static inline Type const type = Type::RemoveOutputArc;
};

class _SC_EXTERN ScEventRemoveInputArc final : public ScEventRemoveArc
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

  static inline Type const type = Type::RemoveInputArc;
};

class _SC_EXTERN ScEventEraseElement final : public ScElementaryEvent
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

  static inline Type const type = Type::EraseElement;
};

class _SC_EXTERN ScEventChangeContent final : public ScElementaryEvent
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

  static inline Type const type = Type::ChangeContent;
};

using ScActionEvent = ScEventAddOutputArc;
