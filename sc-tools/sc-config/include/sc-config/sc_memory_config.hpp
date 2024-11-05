/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

extern "C"
{
#include <sc-core/sc_memory_params.h>
}

#include "sc-config/sc_params.hpp"

class ScOptions;
class ScConfig;

/*!
 * @class ScMemoryConfig
 * @brief A class for configuring sc-memory parameters based on configuration and options.
 *
 * The ScMemoryConfig class is responsible for reading configuration settings and
 * initializing memory parameters accordingly. It provides methods to retrieve specific
 * settings as needed by other components of the system.
 */
class ScMemoryConfig
{
public:
  /*!
   * @brief Constructs an ScMemoryConfig object from configuration and parameters.
   *
   * This constructor initializes the memory configuration based on a given configuration
   * object and a set of parameters. It populates memory parameters based on valid configuration groups.
   *
   * @param config A ScConfig object containing configuration settings.
   * @param params A ScParams object holding parameter values.
   * @param groupName A name of the configuration group to read settings from.
   */
  ScMemoryConfig(ScConfig const & config, ScParams const & params, std::string groupName = "sc-memory");

  ~ScMemoryConfig();

  /*!
   * @brief Retrieves a string value associated with a specified key.
   *
   * This method returns the string value for a given key, or a default value if the key is not found.
   *
   * @param key A key for which to retrieve the string value.
   * @param defaultValue A default value to return if the key is not found.
   * @return An associated string value or defaultValue if not found.
   */
  sc_char const * GetStringByKey(std::string const & key, sc_char const * defaultValue = nullptr);

  /*!
   * @brief Retrieves a list of strings associated with a given key from the configuration parameters.
   *
   * This function checks if the specified key exists in the configuration parameters. If it does,
   * it parses the associated string value into a list of unique paths, which are separated by a
   * defined separator. The resulting list and its size are returned through the provided pointers.
   *
   * @param key A constant reference to a string that represents the key for which to retrieve the
   *            associated list of paths.
   * @param list A pointer to a pointer where the resulting array of paths will be stored.
   *             The caller is not responsible for freeing this memory after use. Destructor does it.
   * @param list_size A pointer to an unsigned integer where the size of the resulting list will
   *                  be stored. If no paths are found, this will be set to 0.
   *
   * @note If the specified key does not exist in the configuration parameters, both `list` and
   *       `list_size` will be set to nullptr and 0, respectively.
   */
  void GetStringListByKey(std::string const & key, sc_char *** list, sc_uint32 * list_size);

  /*!
   * @brief Retrieves an integer value associated with a specified key.
   *
   * This method returns the integer value for a given key, or a default value if the key is not found.
   *
   * @param key A key for which to retrieve the integer value.
   * @param defaultValue A default integer value to return if the key is not found.
   * @return An associated integer value or defaultValue if not found.
   */
  sc_int32 GetIntByKey(std::string const & key, sc_int32 const defaultValue = 0);

  /*!
   * @brief Retrieves a boolean value associated with a specified key.
   *
   * This method returns true or false based on the string representation of the associated
   * value for a given key, or a default boolean value if the key is not found.
   *
   * @param key A key for which to retrieve the boolean value.
   * @param defaultValue A default boolean value to return if the key is not found.
   * @return True or false based on the associated string value or defaultValue if not found.
   */
  bool GetBoolByKey(std::string const & key, bool const defaultValue = false);

  /*!
   * @brief Checks if a specified key exists in memory parameters.
   *
   * This method checks whether a given key is present in the stored memory parameters.
   *
   * @param key A key to check for existence in memory parameters.
   * @return True if the key exists; otherwise, false.
   */
  bool HasKey(std::string const & key);

  /*!
   * @brief Retrieves all memory parameters as a structured object.
   *
   * @return A structured object containing all relevant memory parameters populated from configuration and user input.
   */
  sc_memory_params GetParams();

  //! Returns current sc-machine version.
  static std::string GetVersion();

private:
  ScParams m_params;                ///< Parameters associated with this memory configuration instance
  std::string m_groupName;          ///< Name of the configuration group
  sc_memory_params m_memoryParams;  ///< Structure holding memory-related parameters
};
