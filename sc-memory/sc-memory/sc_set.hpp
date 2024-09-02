/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include "sc_utils.hpp"

class ScTemplate;

/*!
 * @class ScSet
 * @brief Represents a set of sc-elements in the sc-memory.
 *
 * The `ScSet` class provides an abstraction for working with sets of sc-elements in the sc-memory. It allows appending and
 * removing sc-elements, checking their presence, and performing set operations such as union, intersection, and
 * subtraction.
 *
 * @note All methods in this class assume that the set is valid and contains only sc-elements with the same sc-arc type
 * (either all permanent or all temporary).
 *
 * @note If a set contains elements with both permanent and temporary sc-arcs, most operations will fail with an exception.
 */
class _SC_EXTERN ScSet : public ScAddr
{
  friend class ScAgentContext;

  struct ScTypeHashFunc;
  struct ScTypeEqualFunc;

public:
  _SC_EXTERN ~ScSet();

  /*!
   * @brief Appends a sc-element to the set with specified sc-arc type.
   * @param elAddr A sc-address of sc-element to append.
   * @param arcType A sc-arc type to use for appending sc-element.
   * @return `true` if the sc-element was successfully appended, `false` otherwise.
   */
  _SC_EXTERN bool Append(ScAddr const & elAddr, ScType const & arcType = ScType::EdgeAccessConstPosPerm);

  /*!
   * @brief Appends an element to the set with a specified sc-arc type and an attribute (role relation).
   * @param elAddr A sc-address of sc-element to append.
   * @param attrAddr A sc-address of the attribute to associate with sc-element.
   * @param arcType A sc-arc type to use for appending sc-element.
   * @return `true` if the sc-element was successfully appended, `false` otherwise.
   */
  _SC_EXTERN bool Append(
      ScAddr const & elAddr,
      ScAddr const & attrAddr,
      ScType const & arcType = ScType::EdgeAccessConstPosPerm);

  /*!
   * @brief Removes a sc-element from set with specified sc-arc type.
   * @param elAddr A sc-address of sc-element to remove.
   * @return `true` if the sc-element was successfully removed, `false` otherwise.
   */
  _SC_EXTERN bool Remove(ScAddr const & elAddr, ScType const & arcType = ScType::EdgeAccessPos);

  _SC_EXTERN ScSet & operator<<(ScAddr const & elAddr);
  _SC_EXTERN ScSet & operator<<(class ScTemplateResultItem const & searchResultItem);

  /* Operator equal to remove */
  _SC_EXTERN ScSet & operator>>(ScAddr const & elAddr);

  /*!
   * @brief Checks if a sc-element is present in set with specified arc type.
   * @param elAddr A sc-address of sc-element to check.
   * @param arcType A sc-arc type to use for checking sc-element's presence.
   * @return `true` if the sc-element is present in the set with the specified arc type, `false` otherwise.
   */
  _SC_EXTERN bool Has(ScAddr const & elAddr, ScType const & arcType = ScType::EdgeAccessPos) const;

  /*!
   * @brief Checks if the set is empty.
   * @return `true` if the set is empty, `false` otherwise.
   */
  _SC_EXTERN bool IsEmpty() const;

  /*!
   * @brief Gets a power (size) of the set.
   * @return A number of sc-elements in the set.
   */
  _SC_EXTERN size_t GetPower() const;

  /*!
   * @brief Performs the union operation with another set.
   * @param otherSet A set to perform the union with.
   * @return A resulting set after the union operation.
   * @throw utils::ExceptionInvalidParams if either set contains elements with both permanent and temporary arcs.
   */
  _SC_EXTERN ScSet & Unite(ScSet const & otherSet);

  /*!
   * @brief Performs the intersection operation with another set.
   * @param otherSet A set to perform the intersection with.
   * @return A resulting set after the intersection operation.
   * @throw utils::ExceptionInvalidParams if either set contains elements with both permanent and temporary arcs.
   */
  _SC_EXTERN ScSet & Intersect(ScSet const & otherSet);

  /*!
   * @brief Performs the difference operation with another set.
   * @param otherSet A set to perform the difference with.
   * @return A resulting set after the difference operation.
   * @throw utils::ExceptionInvalidParams if either set contains elements with both permanent and temporary arcs.
   */
  _SC_EXTERN ScSet & Subtract(ScSet const & otherSet);

  _SC_EXTERN friend ScSet const operator+(ScSet const & leftSet, ScSet const & rightSet);

  _SC_EXTERN friend ScSet const operator+=(ScSet const & leftSet, ScSet const & rightSet);

  _SC_EXTERN friend ScSet const operator*(ScSet const & leftSet, ScSet const & rightSet);

  _SC_EXTERN friend ScSet const operator*=(ScSet const & leftSet, ScSet const & rightSet);

  _SC_EXTERN friend ScSet const operator-(ScSet const & leftSet, ScSet const & rightSet);

  _SC_EXTERN friend ScSet const operator-=(ScSet const & leftSet, ScSet const & rightSet);

  _SC_EXTERN friend bool operator==(ScSet const & leftSet, ScSet const & rightSet);

protected:
  static std::unordered_map<std::pair<ScType, ScType>, std::array<ScType, 3>, ScTypeHashFunc, ScTypeEqualFunc>
      m_setsAccessArcTypesToUnionAndInteractionResultAccessArcTypes;
  static size_t const UNITE_SETS_ID;
  static size_t const INTERSECT_SETS_ID;
  static size_t const SUBTRACT_SETS_ID;

  /*!
   * @brief Constructs a new `ScSet` instance.
   * @param context A sc-memory context in which the set resides.
   * @param setAddr A sc-address of set in the sc-memory.
   */
  _SC_EXTERN ScSet(class ScMemoryContext * context, ScAddr const & setAddr);

private:
  ScMemoryContext * m_context;

  static void GetBinaryOperationResultTemplate(
      ScAddr const & leftSet,
      ScAddr const & rightSet,
      ScAddr const & relationAddr,
      ScAddr const & resultSetAddr,
      ScTemplate & resultTemplate);

  static ScAddr SearchBinaryOperationResult(
      ScMemoryContext * context,
      ScAddr const & leftSet,
      ScAddr const & rightSet,
      ScAddr const & relationAddr,
      ScAddr const & resultSetAddr = ScAddr::Empty);

  static ScAddr GenerateBinaryOperationResult(
      ScMemoryContext * context,
      ScAddr const & leftSet,
      ScAddr const & rightSet,
      ScAddr const & relationAddr,
      ScAddr const & resultSetAddr = ScAddr::Empty);

  bool GetElements(ScAddrToValueUnorderedMap<ScType> & elements) const;

  static ScType GetResultAccessArcType(ScType const & arcType, ScType const & otherArcType, size_t operationId);

  static ScType GetPermanencyAccessArcSubtype(ScType const & arcType);
};
