/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <utility>

#include "sc_addr.hpp"

class ScStructure;
class ScResult;
class ScKeynode;

/*!
 * @class ScAction
 * @brief Represents an sc-action in the sc-memory.
 */
class _SC_EXTERN ScAction final : public ScAddr
{
  friend class ScAgentContext;
  template <class TScAgent>
  friend class ScAgentBuilder;

public:
  /*!
   * @brief Gets class of the action.
   *
   * This method searches action class that has a valid type.
   * Valid action class types include:
   * - receptor_action,
   * - effector_action,
   * - behavioral_action,
   * - information_action.
   *
   * @return ScAddr representing class of the action.
   */
  _SC_EXTERN ScAddr GetClass() const noexcept;

  /*!
   * @brief Gets argument of the action by index.
   * @param idx An index of argument of the action.
   * @param defaultArgumentAddr Default address to return if argument is not found.
   * @return ScAddr of argument or defaultArgumentAddr if argument is not found.
   */
  _SC_EXTERN ScAddr GetArgument(size_t idx, ScAddr const & defaultArgumentAddr = ScAddr::Empty) const noexcept;

  /*!
   * @brief Gets argument of the action by order relation.
   * @param orderRelationAddr An address of order relation.
   * @param defaultArgumentAddr Default address to return if argument is not found.
   * @return ScAddr of argument or defaultArgumentAddr if argument is not found.
   */
  _SC_EXTERN ScAddr
  GetArgument(ScAddr const & orderRelationAddr, ScAddr const & defaultArgumentAddr = ScAddr::Empty) const noexcept;

  /*!
   * @brief Gets multiple arguments of the action.
   * @tparam N Number of arguments to get.
   * @return Tuple of ScAddr representing action arguments.
   */
  template <std::size_t N>
  _SC_EXTERN auto GetArguments() noexcept;

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
   * @brief Gets result structure of the action.
   * @return ScStructure representing result.
   */
  _SC_EXTERN ScStructure GetResult() noexcept(false);

  /*!
   * @brief Sets result structure for the action.
   * @param structureAddr ScAddr representing result.
   * @return Reference to the current ScAction object.
   */
  _SC_EXTERN ScAction & SetResult(ScAddr const & structureAddr) noexcept(false);

  /*!
   * @brief Forms result structure of the action.
   * @param addrs Addresses of sc-elements to form the result with.
   * @return Reference to the current ScAction object.
   */
  template <class... TScAddr>
  _SC_EXTERN ScAction & FormResult(TScAddr const &... addrs) noexcept(false);

  /*!
   * @brief Updates result structure of the action.
   * @param addrs Addresses of sc-elements to update the result with.
   * @return Reference to the current ScAction object.
   */
  template <class... TScAddr>
  _SC_EXTERN ScAction & UpdateResult(TScAddr const &... addrs) noexcept(false);

  /*!
   * @brief Checks if the action is initiated.
   * @return True if the action is initiated, false otherwise.
   */
  _SC_EXTERN bool IsInitiated() const noexcept;

  /*!
   * @brief Initiates and waits the action.
   * @param waitTime_ms Wait time (in milliseconds) of action being finished. Be default, it equals to 5000
   * milliseconds.
   * @return true if the action has been waited, otherwise true.
   * @throws utils::ExceptionInvalidState if the action is already initiated or finished.
   */
  _SC_EXTERN bool InitiateAndWait(sc_uint32 waitTime_ms = 5000u) noexcept(false);

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
  _SC_EXTERN bool IsFinished() const noexcept;

  /*!
   * @brief Checks if the action is finished successfully.
   * @return True if the action is finished successfully, false otherwise.
   */
  _SC_EXTERN bool IsFinishedSuccessfully() const noexcept;

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
  _SC_EXTERN bool IsFinishedUnsuccessfully() const noexcept;

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
  _SC_EXTERN bool IsFinishedWithError() const noexcept;

  /*!
   * @brief Marks the action as finished with an error.
   * @return Result of the operation.
   * @throws utils::ExceptionInvalidState if the action is not initiated or already finished.
   */
  _SC_EXTERN ScResult FinishWithError() noexcept(false);

protected:
  class ScAgentContext * m_context;  ///< Context of the agent.
  ScAddr m_resultAddr;               ///< Result structure of the action.
  mutable ScAddr m_actionClassAddr;  ///< Class of the action.

  /*!
   * @brief Constructor for ScAction.
   * @param context Context of the agent.
   * @param actionAddr An address of the action.
   */
  _SC_EXTERN ScAction(ScAgentContext * context, ScAddr const & actionAddr) noexcept;

  /*!
   * @brief Gets multiple arguments of the action.
   * @tparam Is Index sequence.
   * @return Tuple of ScAddr representing arguments.
   */
  template <std::size_t... Is>
  _SC_EXTERN auto GetArgumentsImpl(std::index_sequence<Is...>) noexcept;

  /*!
   * @brief Marks the action as finished and add action state class to the action.
   * @param actionStateAddr An address of action state class.
   * @return Result of the operation.
   * @throws utils::ExceptionInvalidState if the action is not initiated or already finished.
   */
  void Finish(ScAddr const & actionStateAddr) noexcept(false);

private:
  /*!
   * @brief Checks if the given action class address is valid by verifying its type.
   *
   * This method determines whether the specified action class has a type
   * that is recognized as a valid action class. Valid action class types include:
   * - receptor_action,
   * - effector_action,
   * - behavioral_action,
   * - information_action.
   *
   * @param context A pointer to the ScMemoryContext, which provides access to
   *                the memory structure and allows for querying.
   * @param actionClassAddr A sc-address of action class whose validity is to be checked.
   * @return Returns true if the action class address has a valid type;
   *         otherwise, returns false.
   * @note This method is crucial for ensuring that only valid action classes
   *       are processed within the system, helping to maintain data integrity
   *       and prevent errors during action execution.
   */
  static bool IsActionClassValid(class ScMemoryContext * context, ScAddr const & actionClassAddr);

  /*!
   * @brief Retrieves a user-friendly string representation of the action's name.
   *
   * If the action's system identifier cannot be retrieved or is empty, it falls back to using the hash of the action.
   *
   * @return A string containing the action's name wrapped in backticks, or an empty string if no name is available.
   */
  std::string GetActionPrettyString() const;

  /*!
   * @brief Retrieves a formatted string that describes class of the action.
   *
   * If the action class address is valid, it attempts to retrieve the class's system identifier. If the identifier is
   * empty, it falls back to using the hash of the class address.
   *
   * @return A string containing "with class" followed by the class name wrapped in backticks, or an empty string if no
   * class name is available.
   */
  std::string GetActionClassPrettyString() const;
};

#include "sc_action.tpp"
