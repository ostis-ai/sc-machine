/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include "sc_iterator.hpp"

#include "sc_utils.hpp"

/*!
 * @class ScSet
 * @brief Represents a set of sc-elements in the sc-memory.
 *
 * The ScSet class provides an abstraction for working with sets of sc-elements in the sc-memory.
 * It allows appending and removing sc-elements, checking their presence, and performing set operations.
 *
 * @note All methods in this class assume that the set is valid and contains only sc-elements with the same sc-arc type.
 */
class _SC_EXTERN ScSet : public ScAddr
{
  friend class ScAgentContext;

public:
  /*!
   * @brief Destructor for ScSet.
   */
  _SC_EXTERN ~ScSet();

  /*!
   * @brief Appends a sc-element to the set with a specified sc-arc type.
   * @param elementAddr The sc-address of the element to append.
   * @param arcType The sc-arc type to use for the connection (default: ConstPermPosArc).
   * @return true if the element was successfully appended, false otherwise.
   */
  _SC_EXTERN virtual bool Append(ScAddr const & elementAddr, ScType const & arcType = ScType::ConstPermPosArc);

  /*!
   * @brief Appends a sc-element to the set with a specified role relation and arc type.
   * @param elementAddr The sc-address of the element to append.
   * @param roleAddr The sc-address of the role relation.
   * @param arcType The sc-arc type to use for the connection (default: ConstPermPosArc).
   * @return true if the element was successfully appended, false otherwise.
   */
  _SC_EXTERN virtual bool Append(
      ScAddr const & elementAddr,
      ScAddr const & roleAddr,
      ScType const & arcType = ScType::ConstPermPosArc);

  /*!
   * @brief Removes a sc-element from the set with the specified sc-arc type.
   * @param elementAddr The sc-address of the element to remove.
   * @param arcType The sc-arc type to use for the connection (default: ConstPosArc).
   * @return true if the element was successfully removed, false otherwise.
   */
  _SC_EXTERN virtual bool Remove(ScAddr const & elementAddr, ScType const & arcType = ScType::ConstPosArc);

  /*!
   * @brief Adds a sc-element to the set using the << operator.
   * @param elementAddr The sc-address of the element to add.
   * @return Reference to the modified set.
   */
  _SC_EXTERN ScSet & operator<<(ScAddr const & elementAddr);

  /*!
   * @brief Adds multiple sc-elements from a template search result using the << operator.
   * @param searchResultItem The template search result item containing elements to add.
   * @return Reference to the modified set.
   */
  _SC_EXTERN ScSet & operator<<(class ScTemplateResultItem const & searchResultItem);

  /*!
   * @brief Removes a sc-element from the set using the >> operator.
   * @param elementAddr The sc-address of the element to remove.
   * @return Reference to the modified set.
   */
  _SC_EXTERN ScSet & operator>>(ScAddr const & elementAddr);

  /*!
   * @brief Checks if a sc-element is present in the set with the specified arc type.
   * @param elementAddr The sc-address of the element to check.
   * @param arcType The sc-arc type to use for the check (default: ConstPosArc).
   * @return true if the element is present, false otherwise.
   */
  _SC_EXTERN bool Has(ScAddr const & elementAddr, ScType const & arcType = ScType::ConstPosArc) const;

  /*!
   * @brief Checks if the set is empty.
   * @return true if the set is empty, false otherwise.
   */
  _SC_EXTERN bool IsEmpty() const;

  /*!
   * @brief Returns the number of sc-elements in the set.
   * @return The size of the set.
   */
  _SC_EXTERN size_t GetPower() const;

  /*!
   * @brief Returns the next element in the set and fills the roles set with associated roles.
   * @param roles Set to be filled with roles associated with the element.
   * @return The sc-address of the next element, or ScAddr::Empty if no more elements.
   */
  _SC_EXTERN virtual ScAddr Next(ScAddrUnorderedSet & roles) const;

  /*!
   * @brief Returns the next element in the set.
   * @return The sc-address of the next element, or ScAddr::Empty if no more elements.
   */
  _SC_EXTERN virtual ScAddr Next() const;

  /*!
   * @brief Resets the internal iterator for traversing the set.
   */
  _SC_EXTERN virtual void Reset();

  /*!
   * @brief Applies a function to each element-role quadruple in the set.
   *
   * Iterates over all elements in the set and, for each, calls the provided function with four arguments:
   * - arcToElementAddr: address of the arc connecting the set to the element
   * - elementAddr: address of the element in the set
   * - arcFromRoleAddr: address of the arc connecting the role node to the arc (if any, otherwise ScAddr::Empty)
   * - roleAddr: address of the role node (if any, otherwise ScAddr::Empty)
   *
   * @tparam Func Type of the function or lambda to apply.
   * @param func Function to call for each quadruple (arcToElementAddr, elementAddr, arcFromRoleAddr, roleAddr).
   */
  template <typename Func>
  _SC_EXTERN void ForEach(Func func) const;

  /*!
   * @brief Retrieves elements from the set that are associated with the specified roles.
   *
   * Iterates over the elements in the set and, for each element whose role address is present
   * in the provided `roles` set, adds an entry to the `elements` map. The map is populated with
   * the role address as the key and the corresponding element address as the value.
   *
   * @param roles A set of role addresses to filter elements by. Only elements associated with these roles will be
   * included in the result.
   *
   * @param elements An output map that will be populated with role addresses as keys and their corresponding element
   * addresses as values. Only elements whose roles are found in the `roles` set are included.
   *
   * @return Returns `true` if all roles in the set are found among the elements; returns `false` if there are any roles
   * in the input set that are not associated with any element in the set.
   */
  _SC_EXTERN bool GetElementsByRoles(ScAddrUnorderedSet const & roles, ScAddrToValueUnorderedMap<ScAddr> & elements)
      const;

  /*!
   * @brief Retrieves all element addresses from the set.
   *
   * Clears the provided `elements` set and then populates it with the addresses of all elements contained
   * in this set. After this method is called, `elements` will contain only the element addresses from this set.
   *
   * @param elements
   *   An output set that will be cleared and then filled with the addresses of all elements in the set.
   *
   * @note
   *   Any previous contents of `elements` will be removed.
   */
  _SC_EXTERN void GetElements(ScAddrUnorderedSet & elements) const;

protected:
  /*!
   * @brief Constructs a new ScSet instance.
   * @param context The sc-memory context in which the set resides.
   * @param setAddr The sc-address of the set in sc-memory.
   */
  _SC_EXTERN ScSet(class ScMemoryContext * context, ScAddr const & setAddr);

  ScMemoryContext * m_context;                          ///< Pointer to the sc-memory context.
  mutable ScIterator3Ptr m_elementsIterator = nullptr;  ///< Iterator for traversing set elements.
};

#include "_template/sc_set.tpp"
