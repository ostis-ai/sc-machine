/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_event.hpp"
#include "sc_event_subscription.hpp"

#include "sc_wait.hpp"

#include "sc_struct.hpp"

template <class TScAddr>
concept ScAddrClass = std::derived_from<TScAddr, ScAddr>;

class _SC_EXTERN ScAction final : public ScAddr
{
public:
  _SC_EXTERN ScAddr GetClass() const;

  _SC_EXTERN ScAddr GetArgument(size_t idx, ScAddr const & defaultArgumentAddr = ScAddr::Empty) const;

  _SC_EXTERN ScAddr
  GetArgument(ScAddr const & orderRelationAddr, ScAddr const & defaultArgumentAddr = ScAddr::Empty) const;

  template <std::size_t N>
  _SC_EXTERN auto GetArguments()
  {
    return GetArgumentsImpl(std::make_index_sequence<N>{});
  }

  _SC_EXTERN ScAction & SetArgument(size_t idx, ScAddr const & argumentAddr);

  _SC_EXTERN ScAction & SetArgument(ScAddr const & orderRelationAddr, ScAddr const & argumentAddr);

  template <ScAddrClass... TScAddr>
  _SC_EXTERN ScAction & SetArguments(TScAddr const &... arguments)
  {
    size_t i = 0;
    for (ScAddr const & argumentAddr : ScAddrVector{arguments...})
      SetArgument(++i, argumentAddr);

    return *this;
  }

  _SC_EXTERN ScStruct GetAnswer() const;

  template <ScAddrClass... TScAddr>
  _SC_EXTERN ScAction & FormAnswer(TScAddr const &... addrs)
  {
    for (ScAddr const & addr : ScAddrVector(addrs...))
      m_answer << addr;

    return *this;
  }

  template <ScAddrClass... TScAddr>
  _SC_EXTERN ScAction & UpdateAnswer(TScAddr const &... addrs)
  {
    return FormAnswer(addrs...);
  }

  _SC_EXTERN sc_bool IsInitiated();

  _SC_EXTERN std::shared_ptr<ScWaitActionFinished> Initiate() noexcept(false);

  _SC_EXTERN sc_bool IsFinished() const;

  _SC_EXTERN sc_bool IsFinishedSuccessfully() const;

  _SC_EXTERN sc_result FinishSuccessfully() noexcept(false);

  _SC_EXTERN sc_bool IsFinishedUnsuccessfully() const;

  _SC_EXTERN sc_result FinishUnsuccessfully() noexcept(false);

  _SC_EXTERN sc_bool IsFinishedWithError() const;

  _SC_EXTERN sc_result FinishWithError() noexcept(false);

protected:
  class ScAgentContext * m_ctx;

  ScAddr m_userAddr;
  ScAddr m_actionClassAddr;

  ScStruct m_answer;

  _SC_EXTERN ScAction(ScAddr const & userAddr, ScAddr const & actionAddr, ScAddr const & actionClassAddr);

  _SC_EXTERN ScAction(ScAgentContext * ctx, ScAddr const & actionClassAddr);

  template <std::size_t... Is>
  _SC_EXTERN auto GetArgumentsImpl(std::index_sequence<Is...>)
  {
    return std::make_tuple(GetArgument(Is + 1)...);
  }

private:
  friend class ScAgentContext;
  friend class ScActionAgent;
  friend class ScActionEvent;
};

class _SC_EXTERN ScActionEvent : public ScEventAddOutputEdge
{
public:
  _SC_EXTERN ScAction GetAction() const;

protected:
  _SC_EXTERN ScActionEvent(
      ScAddr const & userAddr,
      ScAddr const & subscriptionAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  template <ScEventClass TScEvent>
  friend class ScElementaryEventSubscription;

  friend class ScActionAgent;

  static inline Type const type = Type::AddOutputEdge;
};

class _SC_EXTERN ScActionEventSubscription final : public ScElementaryEventSubscription<ScActionEvent>
{
public:
  _SC_EXTERN ScActionEventSubscription(
      ScMemoryContext const & ctx,
      ScAddr const & addr,
      std::function<sc_result(ScActionEvent const &)> const & func);
};
