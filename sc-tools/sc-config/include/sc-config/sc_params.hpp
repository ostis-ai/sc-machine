/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

extern "C"
{
#include <sc-core/sc_memory_params.h>
}

class ScOptions;
class ScParams;
class ScConfig;
class ScMemoryConfig;

/*!
 * @class ScParams
 * @brief A class for managing parameters derived from command-line options.
 *
 * The ScParams class is designed to store and manage key-value pairs of parameters
 * obtained from command-line options. It allows for easy insertion and retrieval of
 * parameter values.
 */
class ScParams
{
public:
  /*!
   * @brief Constructs an ScParams object from command-line options and a set of keys.
   *
   * This constructor initializes the ScParams object by checking if any of the provided
   * keys are present in the given ScOptions. If found, it associates each key with its
   * corresponding value.
   *
   * @param options A ScOptions object containing command-line options.
   * @param keysSet A vector of vectors containing sets of keys to check against the options.
   */
  ScParams(ScOptions const & options, std::vector<std::vector<std::string>> const & keysSet);

  /*!
   * @brief Default constructor for ScParams.
   *
   * Initializes an empty ScParams object.
   */
  ScParams();

  /*!
   * @brief Retrieves a parameter value associated with a specified key.
   *
   * This template method allows retrieval of parameter values as a specific type.
   * If the key does not exist, an exception is thrown.
   *
   * @tparam TContentType The type to which the parameter value should be converted.
   * @param key A key for which to retrieve the parameter value.
   * @return A constant reference to the parameter value of type TContentType.
   * @throws utils::ExceptionItemNotFound if the key is not found.
   */
  template <typename TContentType>
  TContentType const & Get(std::string const & key) const;

  /*!
   * @brief Retrieves a parameter value associated with a specified key, with a default value.
   *
   * This template method allows retrieval of parameter values as a specific type, returning
   * a default value if the key does not exist. If the type is `std::string`, it returns
   * the default value directly if the key is not found.
   *
   * @tparam TContentType The type to which the parameter value should be converted.
   * @param key A key for which to retrieve the parameter value.
   * @param defaultValue A default value to return if the key is not found.
   * @return A constant reference to the parameter value of type TContentType or defaultValue if not found.
   */
  template <typename TContentType>
  TContentType const & Get(std::string const & key, TContentType const & defaultValue) const;

  /*!
   * @brief Inserts a key-value pair into the parameters.
   *
   * This method allows adding a new parameter to the internal storage.
   *
   * @param pair A pair containing the key and its associated value.
   */
  void Insert(std::pair<std::string, std::string> const & pair);

  /*!
   * @brief Checks if a specific key exists in the parameters.
   *
   * This method returns true if the specified key is present in the stored parameters; otherwise, false.
   *
   * @param key A key to check for existence.
   * @return True if the key exists; otherwise, false.
   */
  sc_bool Has(std::string const & key) const;

private:
  std::unordered_map<std::string, std::string> m_params;  ///< Storage for key-value pairs of parameters.
};

#include "sc-config/_template/sc_params.tpp"
