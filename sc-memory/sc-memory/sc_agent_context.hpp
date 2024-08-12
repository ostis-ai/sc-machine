/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory.hpp"

template <class TScEvent>
class ScWaitCondition;
class ScAction;
class ScSet;
class ScStructure;
class ScKeynode;

/*!
 * @class ScAgentContext
 * @brief Represents context for an sc-agent. ScAgentContext is derived from ScMemoryContext.
 */
class ScAgentContext : public ScMemoryContext
{
  template <class TScEvent>
  friend class ScAgentAbstract;
  friend class ScAction;

  SC_DISALLOW_COPY(ScAgentContext);

public:
  /*!
   * @brief Default constructor for ScAgentContext.
   */
  _SC_EXTERN explicit ScAgentContext() noexcept;

  /*!
   * @brief Constructor for ScAgentContext with a given memory context.
   * @param context Pointer to sc-memory context.
   */
  _SC_EXTERN explicit ScAgentContext(sc_memory_context * context) noexcept;

  /*!
   * @brief Move constructor for ScAgentContext.
   * @param other Other ScAgentContext to move from.
   */
  _SC_EXTERN ScAgentContext(ScAgentContext && other) noexcept;

  /*!
   * @brief Move assignment operator for ScAgentContext.
   * @param other Other ScAgentContext to move from.
   * @return Reference to the current ScAgentContext object.
   */
  _SC_EXTERN ScAgentContext & operator=(ScAgentContext && other) noexcept;

  /*!
   * @brief Initializes an event with a given description address, cause, and check function.
   * @tparam TScEvent Type of event.
   * @param subscriptionAddr An address of sc-element describing an event.
   * @param cause Function to call to cause an event.
   * @param check Function to check an event.
   * @return Shared pointer to ScWaitCondition for an event.
   */
  template <class TScEvent>
  _SC_EXTERN std::shared_ptr<ScWaitCondition<TScEvent>> InitializeEvent(
      ScAddr const & subscriptionAddr,
      std::function<void(void)> const & cause,
      std::function<sc_result(TScEvent const &)> check) const;

  /*!
   * @brief Creates an action with a given action class.
   * @param actionClassAddr Address of the action class.
   * @return ScAction object.
   */
  _SC_EXTERN ScAction CreateAction(ScAddr const & actionClassAddr);

  /*!
   * @brief Create ScAction object for provided action.
   * @param actionAddr Address of the action.
   * @return ScAction object.
   */
  _SC_EXTERN ScAction UseAction(ScAddr const & actionAddr);

  /*!
   * @brief Creates a set.
   * @return ScSet object.
   */
  _SC_EXTERN ScSet CreateSet();

  /*!
   * @brief Create ScSet object for provided set.
   * @param setAddr Address of the set.
   * @return ScSet object.
   */
  _SC_EXTERN ScSet UseSet(ScAddr const & setAddr);

  /*!
   * @brief Creates a structure.
   * @return ScStructure object.
   */
  _SC_EXTERN ScStructure CreateStructure();

  /*!
   * @brief Create ScStructure object for provided set.
   * @param structureAddr Address of the set.
   * @return ScStructure object.
   */
  _SC_EXTERN ScStructure UseStructure(ScAddr const & structureAddr);

protected:
  /*!
   * @brief Constructor for ScAgentContext with a given user.
   * @param userAddr Address of the user.
   */
  _SC_EXTERN explicit ScAgentContext(ScAddr const & userAddr) noexcept;
};

#include "sc_agent_context.tpp"
