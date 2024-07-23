/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc_addr.hpp"

class ScStruct;
class ScResult;

/*!
 * @class ScAction
 * @brief Represents an sc-action in the sc-memory.
 */
class _SC_EXTERN ScAction final : public ScAddr
{
public:
  /*!
   * @brief Gets class of the action.
   * @return ScAddr representing class of the action.
   */
  _SC_EXTERN ScAddr GetClass() const;

  /*!
   * @brief Gets argument of the action by index.
   * @param idx An index of argument of the action.
   * @param defaultArgumentAddr Default address to return if argument is not found.
   * @return ScAddr of argument or defaultArgumentAddr if argument is not found.
   */
  _SC_EXTERN ScAddr GetArgument(size_t idx, ScAddr const & defaultArgumentAddr = ScAddr::Empty) const;

  /*!
   * @brief Gets argument of the action by order relation.
   * @param orderRelationAddr An address of order relation.
   * @param defaultArgumentAddr Default address to return if argument is not found.
   * @return ScAddr of argument or defaultArgumentAddr if argument is not found.
   */
  _SC_EXTERN ScAddr
  GetArgument(ScAddr const & orderRelationAddr, ScAddr const & defaultArgumentAddr = ScAddr::Empty) const;

  /*!
   * @brief Gets multiple arguments of the action.
   * @tparam N Number of arguments to get.
   * @return Tuple of ScAddr representing action arguments.
   */
  template <std::size_t N>
  _SC_EXTERN auto GetArguments();

  /*!
   * @brief Sets argument of the action by index.
   * @param idx Index of argument of the action.
   * @param argumentAddr An address of the argument to set.
   * @return Reference to the current ScAction object.
   */
  _SC_EXTERN ScAction & SetArgument(size_t idx, ScAddr const & argumentAddr) noexcept(false);

  /*!
   * @brief Sets argument of the action by order relation.
   * @param orderRelationAddr An address of order relation.
   * @param argumentAddr An address of argument to set.
   * @return Reference to the current ScAction object.
   */
  _SC_EXTERN ScAction & SetArgument(ScAddr const & orderRelationAddr, ScAddr const & argumentAddr) noexcept(false);

  /*!
   * @brief Sets multiple arguments of the action.
   * @param arguments Arguments to set.
   * @return Reference to the current ScAction object.
   */
  template <class... TScAddr>
  _SC_EXTERN ScAction & SetArguments(TScAddr const &... arguments) noexcept(false);

  /*!
   * @brief Gets answer structure of the action.
   * @return ScStruct representing answer.
   */
  _SC_EXTERN ScStruct GetAnswer() noexcept(false);

  /*!
   * @brief Sets answer structure for the action.
   * @param structureAddr ScAddr representing answer.
   * @return Reference to the current ScAction object.
   */
  _SC_EXTERN ScAction & SetAnswer(ScAddr const & structureAddr) noexcept(false);

  /*!
   * @brief Forms answer structure of the action.
   * @param addrs Addresses of sc-elements to form the answer with.
   * @return Reference to the current ScAction object.
   */
  template <class... TScAddr>
  _SC_EXTERN ScAction & FormAnswer(TScAddr const &... addrs) noexcept(false);

  /*!
   * @brief Updates answer structure of the action.
   * @param addrs Addresses of sc-elements to update the answer with.
   * @return Reference to the current ScAction object.
   */
  template <class... TScAddr>
  _SC_EXTERN ScAction & UpdateAnswer(TScAddr const &... addrs) noexcept(false);

  /*!
   * @brief Checks if the action is initiated.
   * @return True if the action is initiated, false otherwise.
   */
  _SC_EXTERN sc_bool IsInitiated() const;

  /*!
   * @brief Initiates and waits the action.
   * @param waitTime_ms Wait time (in milliseconds) of action being finished. Be default, it equals to 5000
   * milliseconds.
   * @return SC_TRUE if the action has been waited, otherwise SC_FALSE.
   * @throws utils::ExceptionInvalidState if the action is already initiated or finished.
   */
  _SC_EXTERN sc_bool InitiateAndWait(sc_uint32 waitTime_ms = 5000u) noexcept(false);

  /*!
   * @brief Initiates the action.
   * @throws utils::ExceptionInvalidState if the action is already initiated or finished.
   * @return Reference to the current ScAction object.
   */
  _SC_EXTERN ScAction & Initiate() noexcept(false);

  /*!
   * @brief Checks if the action is finished.
   * @return True if the action is finished, false otherwise.
   */
  _SC_EXTERN sc_bool IsFinished() const;

  /*!
   * @brief Checks if the action is finished successfully.
   * @return True if the action is finished successfully, false otherwise.
   */
  _SC_EXTERN sc_bool IsFinishedSuccessfully() const;

  /*!
   * @brief Marks the action as finished successfully.
   * @return Result of the operation.
   * @throws utils::ExceptionInvalidState if the action is not initiated or already finished.
   */
  _SC_EXTERN ScResult FinishSuccessfully() noexcept(false);

  /*!
   * @brief Checks if the action is finished unsuccessfully.
   * @return True if the action is finished unsuccessfully, false otherwise.
   */
  _SC_EXTERN sc_bool IsFinishedUnsuccessfully() const;

  /*!
   * @brief Marks the action as finished unsuccessfully.
   * @return Result of the operation.
   * @throws utils::ExceptionInvalidState if the action is not initiated or already finished.
   */
  _SC_EXTERN ScResult FinishUnsuccessfully() noexcept(false);

  /*!
   * @brief Checks if the action is finished with an error.
   * @return True if the action is finished with an error, false otherwise.
   */
  _SC_EXTERN sc_bool IsFinishedWithError() const;

  /*!
   * @brief Marks the action as finished with an error.
   * @return Result of the operation.
   * @throws utils::ExceptionInvalidState if the action is not initiated or already finished.
   */
  _SC_EXTERN ScResult FinishWithError() noexcept(false);

protected:
  class ScAgentContext * m_ctx;  ///< Context of the agent.
  ScAddr m_actionClassAddr;      ///< Class of the action.
  ScAddr m_answerAddr;           ///< Answer structure of the action.

  /*!
   * @brief Constructor for ScAction.
   * @param ctx Context of the agent.
   * @param actionAddr An address of the action.
   * @param actionClassAddr An address of the action class.
   */
  _SC_EXTERN ScAction(ScAgentContext * ctx, ScAddr const & actionAddr, ScAddr const & actionClassAddr);

  /*!
   * @brief Constructor for ScAction.
   * @param ctx Context of the agent.
   * @param actionClassAddr An address of the action class.
   */
  _SC_EXTERN ScAction(ScAgentContext * ctx, ScAddr const & actionClassAddr);

  /*!
   * @brief Gets multiple arguments of the action.
   * @tparam Is Index sequence.
   * @return Tuple of ScAddr representing arguments.
   */
  template <std::size_t... Is>
  _SC_EXTERN auto GetArgumentsImpl(std::index_sequence<Is...>);

  /*!
   * @brief Marks the action as finished and add action state class to the action.
   * @param actionStateAddr An address of action state class.
   * @return Result of the operation.
   * @throws utils::ExceptionInvalidState if the action is not initiated or already finished.
   */
  void Finish(ScAddr const & actionStateAddr);

private:
  friend class ScAgentContext;
  template <class TScEvent>
  friend class ScAgent;
};

#include "sc_action.tpp"
