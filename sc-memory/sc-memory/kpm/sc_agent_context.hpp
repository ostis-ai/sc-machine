/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"

#include "sc-memory/utils/sc_keynode_cache.hpp"

#include "sc_wait.hpp"

class ScAgentContext : public ScMemoryContext
{
public:
  _SC_EXTERN explicit ScAgentContext(sc_uint8 accessLevels, std::string const & name = "")
    : ScMemoryContext(accessLevels, name)
    , m_cache(*this)
  {
  }

  _SC_EXTERN ScAddr GetActionArgument(ScAddr const & actionAddr, sc_uint16 number);

  _SC_EXTERN ScAddr SetActionArgument(ScAddr const & actionAddr, ScAddr const & argumentAddr, sc_uint16 number);

  template <class ScEventClass>
  _SC_EXTERN std::shared_ptr<ScWaitCondition<ScEventClass>> InitializeEvent(
      ScAddr const & descriptionAddr,
      std::function<void(void)> const & cause,
      std::function<sc_result(const ScAddr &, const ScAddr &, const ScAddr &)> check)
  {
    cause();
    return std::make_shared<ScWaitCondition<ScEventClass>>(*this, descriptionAddr, check);
  }

  _SC_EXTERN ScAddr FormStructure(ScAddrVector const & addrVector, ScAddr answerAddr = ScAddr::Empty);

  _SC_EXTERN void FormActionAnswer(ScAddr const & actionAddr, ScAddr const & answerAddr);

protected:
  utils::ScKeynodeCache m_cache;
};
