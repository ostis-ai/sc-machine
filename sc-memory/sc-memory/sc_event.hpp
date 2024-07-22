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

public:
  class Type
  {
  public:
    sc_event_type m_realType;

    explicit constexpr Type(sc_event_type type)
      : m_realType(type)
    {
    }

    constexpr Type(Type const & otherType)
      : Type(otherType.m_realType)
    {
    }

    Type & operator()(Type const & other)
    {
      m_realType |= other.m_realType;
      return *this;
    }

    ~Type() = default;

    constexpr operator sc_event_type() const
    {
      return m_realType;
    }

    inline constexpr sc_event_type operator*() const
    {
      return m_realType;
    }

    inline sc_bool operator==(Type const & other) const
    {
      return (m_realType == other.m_realType);
    }

    inline sc_bool operator!=(Type const & other) const
    {
      return (m_realType != other.m_realType);
    }

    inline Type operator|(Type const & other) const
    {
      return Type(m_realType | other.m_realType);
    }

    inline Type operator&(Type const & other) const
    {
      return Type(m_realType & other.m_realType);
    }

    inline Type & operator|=(Type const & other)
    {
      m_realType |= other.m_realType;
      return *this;
    }

    inline Type & operator&=(Type const & other)
    {
      m_realType &= other.m_realType;
      return *this;
    }

    static Type const Unknown;
    static Type const AddInputEdge;
    static Type const AddOutputEdge;
    static Type const RemoveInputEdge;
    static Type const RemoveOutputEdge;
    static Type const RemoveElement;
    static Type const ChangeContent;
  };

protected:
  static inline Type const type = Type::Unknown;

  _SC_EXTERN sc_result Initialize(ScMemoryContext *, ScAddr const &) final;

  _SC_EXTERN sc_result Shutdown(ScMemoryContext *) final;
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

class _SC_EXTERN ScEventAddEdge : public ScElementaryEvent
{
public:
  _SC_EXTERN virtual ScAddr GetAddedConnector() const;

  _SC_EXTERN virtual ScType GetAddedConnectorType() const;

  _SC_EXTERN virtual ScAddr GetOtherElement() const;

protected:
  _SC_EXTERN ScEventAddEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);
};

class _SC_EXTERN ScEventAddOutputEdge : public ScEventAddEdge
{
protected:
  _SC_EXTERN ScEventAddOutputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::AddOutputEdge;
};

class _SC_EXTERN ScEventAddInputEdge final : public ScEventAddEdge
{
protected:
  _SC_EXTERN ScEventAddInputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::AddInputEdge;
};

class _SC_EXTERN ScEventRemoveEdge : public ScElementaryEvent
{
public:
  _SC_EXTERN virtual ScType GetRemovedConnectorType() const;

  _SC_EXTERN virtual ScAddr GetOtherElement() const;

protected:
  _SC_EXTERN ScEventRemoveEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);
};

class _SC_EXTERN ScEventRemoveOutputEdge final : public ScEventRemoveEdge
{
protected:
  _SC_EXTERN ScEventRemoveOutputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::RemoveOutputEdge;
};

class _SC_EXTERN ScEventRemoveInputEdge final : public ScEventRemoveEdge
{
protected:
  _SC_EXTERN ScEventRemoveInputEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

  static inline Type const type = Type::RemoveInputEdge;
};

class _SC_EXTERN ScEventRemoveElement final : public ScElementaryEvent
{
protected:
  _SC_EXTERN ScEventRemoveElement(
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

  static inline Type const type = Type::RemoveElement;
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

using ScActionEvent = ScEventAddOutputEdge;
