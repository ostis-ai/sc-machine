/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include "sc_utils.hpp"

class ScMemoryContext;

/*!
 * @brief Base class for iterators in sc-memory.
 *
 * This class provides the common interface and functionality for all sc-memory iterators.
 *
 * @tparam IterType Type of the iterator.
 * @tparam tripleSize Count of sc-element in constructions to be found.
 */
template <typename IterType, sc_uint8 tripleSize>
class _SC_EXTERN ScIterator
{
public:
  _SC_EXTERN virtual ~ScIterator() = default;

  /*!
   * @brief Checks if the iterator is valid.
   *
   * @return True if the iterator is valid, false otherwise.
   */
  inline _SC_EXTERN bool IsValid() const
  {
    return m_iterator != nullptr;
  }

  /*!
   * @brief Advances the iterator to the next construction.
   *
   * @return True if there is the next construction in sc-memory, false otherwise.
   */
  _SC_EXTERN virtual bool Next() const = 0;

  /*!
   * @brief Gets sc-address of sc-element by its index from found construction.
   *
   * @param index Index of sc-element.
   * @return sc-address of sc-element with specified index.
   */
  _SC_EXTERN virtual ScAddr Get(size_t index) const = 0;

  /*!
   * @brief Gets found construction of sc-element sc-addresses.
   *
   * @return An array containing construction of sc-element sc-addresses.
   */
  _SC_EXTERN virtual std::array<ScAddr, tripleSize> Get() const = 0;

  /*!
   * @brief Short form of Get.
   *
   * @param idx Index of sc-element.
   * @return sc-address of sc-element with specified index.
   */
  inline ScAddr operator[](size_t idx) const
  {
    return Get(idx);
  }

protected:
  IterType * m_iterator = nullptr;
  size_t m_tripleSize = tripleSize;

  static sc_type Convert(sc_type const & s);
  static sc_addr Convert(sc_addr const & s);
  static sc_addr Convert(ScAddr const & addr);
  static sc_type Convert(ScType const & type);
};

using ScAddrTriple = std::array<ScAddr, 3>;
using ScAddrQuintuple = std::array<ScAddr, 5>;

/*!
 * @brief Iterator for sc-memory triples (three-element constructions).
 *
 * This class provides functionality to iterate over triples in sc-memory.
 *
 * @tparam ParamType1 Type of the first parameter.
 * @tparam ParamType2 Type of the second parameter.
 * @tparam ParamType3 Type of the third parameter.
 */
template <typename ParamType1, typename ParamType2, typename ParamType3>
class _SC_EXTERN ScIterator3 : public ScIterator<sc_iterator3, 3>
{
  friend class ScMemoryContext;

protected:
  /*!
   * @brief Constructor for ScIterator3.
   *
   * @param context sc-memory context.
   * @param p1 The first parameter.
   * @param p2 The second parameter.
   * @param p3 The third parameter.
   */
  _SC_EXTERN ScIterator3(
      ScMemoryContext const & context,
      ParamType1 const & p1,
      ParamType2 const & p2,
      ParamType3 const & p3);

public:
  _SC_EXTERN virtual ~ScIterator3();

  /*!
   * @brief Copy constructor for ScIterator3.
   *
   * @param other Another ScIterator3 instance.
   */
  _SC_EXTERN ScIterator3(ScIterator3 const & other);

  /*!
   * @brief Assignment operator for ScIterator3.
   *
   * @param other Another ScIterator3 instance.
   * @return Reference to this instance.
   */
  _SC_EXTERN ScIterator3 & operator=(ScIterator3 const & other);

  /*!
   * @brief Destroys the iterator.
   */
  _SC_EXTERN void Destroy();

  /*!
   * @brief Moves the iterator to the next triple.
   *
   * @return True if there is the next triple in sc-memory, false otherwise.
   */
  _SC_EXTERN bool Next() const override;

  /*!
   * @brief Gets sc-address of sc-element by its index from found triple.
   *
   * @param index Index of sc-element.
   * @return sc-address of sc-element with specified index.
   */
  _SC_EXTERN ScAddr Get(size_t index) const override;

  /*!
   * @brief Gets found triple of sc-element sc-addresses.
   *
   * @return An array containing triple of sc-element sc-addresses.
   */
  _SC_EXTERN ScAddrTriple Get() const override;
};

/*!
 * @brief Iterator for sc-memory quintuples (five-element constructions).
 *
 * This class provides functionality to iterate over quintuples in sc-memory.
 *
 * @tparam ParamType1 Type of the first parameter.
 * @tparam ParamType2 Type of the second parameter.
 * @tparam ParamType3 Type of the third parameter.
 * @tparam ParamType4 Type of the fourth parameter.
 * @tparam ParamType5 Type of the fifth parameter.
 */
template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
class _SC_EXTERN ScIterator5 : public ScIterator<sc_iterator5, 5>
{
  friend class ScMemoryContext;

protected:
  /*!
   * @brief Constructor for ScIterator5.
   *
   * @param context sc-memory context.
   * @param p1 The first parameter.
   * @param p2 The second parameter.
   * @param p3 The third parameter.
   * @param p4 The fourth parameter.
   * @param p5 The fifth parameter.
   */
  _SC_EXTERN ScIterator5(
      ScMemoryContext const & context,
      ParamType1 const & p1,
      ParamType2 const & p2,
      ParamType3 const & p3,
      ParamType4 const & p4,
      ParamType5 const & p5);

public:
  _SC_EXTERN ~ScIterator5() override;

  /*!
   * @brief Destroys the iterator.
   */
  _SC_EXTERN void Destroy();

  /*!
   * @brief Moves the iterator to the next quintuple.
   *
   * @return True if there is the next quintuple in sc-memory, false otherwise.
   */
  _SC_EXTERN bool Next() const override;

  /*!
   * @brief Gets sc-address of sc-element by its index from iterator quintuple.
   *
   * @param index Index of sc-element.
   * @return sc-address of sc-element with specified index.
   */
  _SC_EXTERN ScAddr Get(size_t index) const override;

  /*!
   * @brief Gets found quintuple of sc-element sc-addresses.
   *
   * @return An array containing quintuple of sc-element sc-addresses.
   */
  _SC_EXTERN ScAddrQuintuple Get() const override;
};

#include "sc-memory/_template/sc_iterator.tpp"

using ScIterator3Ptr = std::shared_ptr<ScIterator<sc_iterator3, 3>>;
using ScIterator5Ptr = std::shared_ptr<ScIterator<sc_iterator5, 5>>;
