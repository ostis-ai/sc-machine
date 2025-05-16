/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_set.hpp"

/*!
 * @class ScOrientedSet
 * @brief Represents an oriented (ordered by nrel_basic_sequence relation) set of sc-elements in sc-memory.
 *
 * ScOrientedSet extends ScSet by supporting ordered insertion and traversal of elements.
 * The order is maintained using special arcs and keynodes for sequence relations.
 */
class _SC_EXTERN ScOrientedSet : public ScSet
{
  friend class ScAgentContext;

public:
  /*!
   * @brief Destructor for ScOrientedSet.
   */
  _SC_EXTERN ~ScOrientedSet();

  /*!
   * @brief Appends an element to the end of the oriented set.
   *
   * The element is inserted as the last element of the sequence, preserving the order.
   *
   * @param elementAddr sc-address of the element to append.
   * @param arcType Arc type to use for the connection (default: ConstPermPosArc).
   * @return true if the element was appended successfully.
   */
  _SC_EXTERN bool Append(ScAddr const & elementAddr, ScType const & arcType = ScType::ConstPermPosArc) override;

  /*!
   * @brief Appends an element to the end of the oriented set with a specified role.
   *
   * The element is inserted as the last element, and the specified role is associated with the connection.
   *
   * @param elementAddr sc-address of the element to append.
   * @param roleAddr sc-address of the role node to associate.
   * @param arcType Arc type to use for the connection (default: ConstPermPosArc).
   * @return true if the element was appended successfully.
   */
  _SC_EXTERN bool Append(
      ScAddr const & elementAddr,
      ScAddr const & roleAddr,
      ScType const & arcType = ScType::ConstPermPosArc) override;

  /*!
   * @brief Returns the next element in the oriented set and fills the roles set with associated roles.
   *
   * Iterates through the set in order, starting from the first element or from the current position if called
   * repeatedly.
   *
   * @param roles Set to be filled with role addresses associated with the current element.
   * @return sc-address of the next element, or ScAddr::Empty if at the end.
   */
  _SC_EXTERN ScAddr Next(ScAddrSet & roles) const override;

  /*!
   * @brief Returns the next element in the oriented set.
   *
   * Iterates through the set in order, starting from the first element or from the current position if called
   * repeatedly.
   *
   * @return sc-address of the next element, or ScAddr::Empty if at the end.
   */
  _SC_EXTERN ScAddr Next() const override;

  /*!
   * @brief Resets the internal iterator to the first element in the oriented set.
   *
   * After calling Reset(), the next call to Next() will return the first element.
   */
  _SC_EXTERN void Reset() override;

  /*!
   * @brief Returns the last element of the oriented set.
   *
   * @return sc-address of the last element in the set, or ScAddr::Empty if the set is empty.
   */
  _SC_EXTERN ScAddr GetLast() const;

  /*!
   * @brief Applies a function to each element-role quadruple in the oriented set, in order.
   *
   * Iterates through all elements of the oriented set in their defined sequence.
   * For each element, the provided function is called with the following arguments:
   *   - arcToElementAddr: sc-address of the arc from the set to the element
   *   - elementAddr: sc-address of the element in the set
   *   - arcFromRoleAddr: sc-address of the arc from a role node to the arc (if any, otherwise ScAddr::Empty)
   *   - roleAddr: sc-address of the role node (if any, otherwise ScAddr::Empty)
   *
   * @tparam Func Type of the function or lambda to apply.
   * @param func Function to call for each quadruple (arcToElementAddr, elementAddr, arcFromRoleAddr, roleAddr).
   */
  template <typename Func>
  _SC_EXTERN void ForEach(Func func) const;

protected:
  /*!
   * @brief Constructs a new ScOrientedSet instance.
   *
   * @param context sc-memory context in which the set resides.
   * @param setAddr sc-address of the set in sc-memory.
   */
  ScOrientedSet(ScMemoryContext * context, ScAddr const & setAddr);

  /*!
   * @brief Appends an element to the end of the oriented set and manages sequence arcs.
   *
   * This method is used internally to maintain the order of elements.
   *
   * @param elementAddr sc-address of the element to append.
   * @param arcType Arc type to use for the connection.
   * @return sc-address of the arc connecting the set to the new element.
   */
  ScAddr AppendElement(ScAddr const & elementAddr, ScType const & arcType);

  //! Internal address of the current arc used for ordered iteration.
  mutable ScAddr m_currentArcAddr;
}
