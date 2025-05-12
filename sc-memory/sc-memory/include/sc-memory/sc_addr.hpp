/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <cstdint>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <string>

extern "C"
{
#include <sc-core/sc_types.h>
}

/// Alias for the underlying sc_addr type.
using ScRealAddr = sc_addr;

/*!
 * @class ScAddr
 * @brief Class representing sc-element address in sc-memory.
 */
class _SC_EXTERN ScAddr
{
  friend class ScMemoryContext;
  friend class ScTemplateItem;

public:
  using HashType = sc_addr_hash;

  /// Represents an empty (invalid) address.
  static ScAddr const Empty;

  /*!
   * @brief Default constructor. Creates an empty (invalid) address.
   */
  ScAddr();

  /*!
   * @brief Constructs from a raw sc_addr.
   * @param addr The sc_addr to wrap.
   */
  ScAddr(sc_addr const & addr);

  /*!
   * @brief Constructs from a hash value.
   * @param hash Hash value to construct the address from.
   */
  ScAddr(ScAddr::HashType const & hash);

  /*!
   * @brief Checks if the address is not empty.
   * @return True if valid, false otherwise.
   */
  bool IsValid() const;

  /*!
   * @brief Resets the address to an invalid state.
   */
  void Reset();

  /*!
   * @brief Equality operator.
   * @param other Address to compare.
   * @return True if addresses are equal.
   */
  bool operator==(ScAddr const & other) const;

  /*!
   * @brief Inequality operator.
   * @param other Address to compare.
   * @return True if addresses are not equal.
   */
  bool operator!=(ScAddr const & other) const;

  /*!
   * @brief Dereference operator to access the underlying sc_addr.
   * @return Reference to the underlying sc_addr.
   */
  ScRealAddr const & operator*() const;

  /*!
   * @brief Returns a hash of the address.
   * @return Hash value.
   */
  HashType Hash() const;

  /*!
   * @brief Gets the underlying sc_addr.
   * @return Reference to the underlying sc_addr.
   */
  ScRealAddr const & GetRealAddr() const;

  /*!
   * @brief Converts the address to a string representation.
   * @return String representation.
   */
  operator std::string() const;

  /*!
   * @brief Stream output operator for ScAddr.
   * @param os Output stream.
   * @param addr Address to output.
   * @return Reference to the output stream.
   */
  friend std::ostream & operator<<(std::ostream & os, ScAddr const & addr);

protected:
  ScRealAddr m_realAddr;
};

/*!
 * @struct RealAddrLessFunc
 * @brief Comparator for ScRealAddr, orders first by segment, then by offset.
 */
struct _SC_EXTERN RealAddrLessFunc
{
  /*!
   * @brief Comparison operator.
   * @param a First address.
   * @param b Second address.
   * @return True if a < b.
   */
  bool operator()(ScRealAddr const & a, ScRealAddr const & b) const;
};

/*!
 * @struct ScAddrLessFunc
 * @brief Comparator for ScAddr, uses RealAddrLessFunc.
 */
struct _SC_EXTERN ScAddrLessFunc
{
  /*!
   * @brief Comparison operator.
   * @param a First ScAddr.
   * @param b Second ScAddr.
   * @return True if a < b.
   */
  bool operator()(ScAddr const & a, ScAddr const & b) const;
};

/*!
 * @struct ScAddrHashFunc
 * @brief Hash functor for ScAddr.
 */
struct _SC_EXTERN ScAddrHashFunc
{
  /*!
   * @brief Hashes the given ScAddr.
   * @param addr Address to hash.
   * @return Hash value.
   */
  ScAddr::HashType operator()(ScAddr const & addr) const;
};

/// Vector of ScAddr.
using ScAddrVector = std::vector<ScAddr>;

/// List of ScAddr.
using ScAddrList = std::list<ScAddr>;

/// Stack of ScAddr.
using ScAddrStack = std::stack<ScAddr>;

/// Queue of ScAddr.
using ScAddrQueue = std::queue<ScAddr>;

/// Ordered set of ScAddr using ScAddrLessFunc.
using ScAddrSet = std::set<ScAddr, ScAddrLessFunc>;

/// Unordered set of ScAddr using ScAddrHashFunc.
using ScAddrUnorderedSet = std::unordered_set<ScAddr, ScAddrHashFunc>;

/// Unordered map from ScAddr to Value using ScAddrHashFunc.
template <typename Value>
using ScAddrToValueUnorderedMap = std::unordered_map<ScAddr, Value, ScAddrHashFunc>;
