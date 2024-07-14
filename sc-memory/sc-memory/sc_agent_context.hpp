/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory.hpp"

#include "sc_wait.hpp"

#include "utils/sc_keynode_cache.hpp"

class ScAgentContext : public ScMemoryContext
{
public:
  _SC_EXTERN explicit ScAgentContext() noexcept;

  _SC_EXTERN explicit ScAgentContext(sc_memory_context * context) noexcept;

  _SC_EXTERN ScAgentContext(ScAgentContext && other) noexcept;

  _SC_EXTERN ScAgentContext & operator=(ScAgentContext && other) noexcept;

  _SC_EXTERN ScAddr GetActionArgument(ScAddr const & actionAddr, sc_uint16 number);

  _SC_EXTERN ScAddr SetActionArgument(ScAddr const & actionAddr, ScAddr const & argumentAddr, sc_uint16 number);

  template <class ScEventClass>
  _SC_EXTERN std::shared_ptr<ScWaitCondition<ScEventClass>> InitializeEvent(
      ScAddr const & descriptionAddr,
      std::function<void(void)> const & cause,
      std::function<sc_result(ScEventClass const &)> check)
  {
    cause();
    return std::make_shared<ScWaitCondition<ScEventClass>>(*this, descriptionAddr, check);
  }

  template <class... TScAddr>
  _SC_EXTERN ScAddr FormStructure(TScAddr const &... addrs)
  {
    static_assert(
        (std::is_base_of<ScAddr, TScAddr>::value && ...),
        "Each argument in the parameter pack must be of class ScAddr.");

    ScAddr const & structureAddr = CreateNode(ScType::NodeConstStruct);

    ScAddrVector const & addrVector{addrs...};
    std::for_each(
        addrVector.begin(),
        addrVector.end(),
        [this, &structureAddr](ScAddr const & addr)
        {
          CreateEdge(ScType::EdgeAccessConstPosPerm, structureAddr, addr);
        });

    return structureAddr;
  }

  template <class... TScAddr>
  _SC_EXTERN ScAddr UpdateStructure(ScAddr structureAddr, TScAddr const &... addrs)
  {
    static_assert(
        (std::is_base_of<ScAddr, TScAddr>::value && ...),
        "Each argument in the parameter pack must be of class ScAddr.");

    if (!structureAddr.IsValid())
      structureAddr = CreateNode(ScType::NodeConstStruct);

    ScAddrVector const & addrVector{addrs...};
    std::for_each(
        addrVector.begin(),
        addrVector.end(),
        [this, &structureAddr](ScAddr const & addr)
        {
          CreateEdge(ScType::EdgeAccessConstPosPerm, structureAddr, addr);
        });

    return structureAddr;
  }

  _SC_EXTERN void FormActionAnswer(ScAddr const & actionAddr, ScAddr const & answerAddr);

protected:
  utils::ScKeynodeCache m_cache;

  template <ScEventClass TScEvent>
  friend class ScAgentAbstract;

  _SC_EXTERN explicit ScAgentContext(ScAddr const & userAddr) noexcept;
};
