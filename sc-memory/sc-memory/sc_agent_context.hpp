/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory.hpp"

#include "sc_wait.hpp"

#include "sc_action.hpp"

#include "utils/sc_keynode_cache.hpp"

class ScAgentContext : public ScMemoryContext
{
public:
  _SC_EXTERN explicit ScAgentContext() noexcept;

  _SC_EXTERN explicit ScAgentContext(sc_memory_context * context) noexcept;

  _SC_EXTERN ScAgentContext(ScAgentContext && other) noexcept;

  _SC_EXTERN ScAgentContext & operator=(ScAgentContext && other) noexcept;

  template <class TScEvent>
  _SC_EXTERN std::shared_ptr<ScWaitCondition<TScEvent>> InitializeEvent(
      ScAddr const & descriptionAddr,
      std::function<void(void)> const & cause,
      std::function<sc_result(TScEvent const &)> check)
  {
    static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

    cause();
    return std::make_shared<ScWaitCondition<TScEvent>>(*this, descriptionAddr, check);
  }

  _SC_EXTERN ScAction CreateAction(ScAddr const & actionAddrClass)
  {
    ScAction action{this, actionAddrClass};
    return action;
  }

protected:
  template <class TScEvent>
  friend class ScAgentAbstract;

  friend class ScAction;

  _SC_EXTERN explicit ScAgentContext(ScAddr const & userAddr) noexcept;
};
