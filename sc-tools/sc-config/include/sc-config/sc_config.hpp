/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>

extern "C"
{
#include "sc_config.h"
}

/*!
 * @class ScConfigGroup
 * @brief A class representing a group of configuration settings.
 *
 * The ScConfigGroup class is responsible for managing a specific group of configuration
 * settings within a larger configuration context. It allows retrieval of values associated
 * with keys that belong to the specified group.
 */
class ScConfigGroup
{
public:
  /*!
   * @brief Constructs an ScConfigGroup object.
   *
   * This constructor initializes the ScConfigGroup by extracting keys from the provided
   * configuration that belong to the specified group. It filters out unused keys and stores
   * valid keys for later access.
   *
   * @param config A pointer to the configuration instance.
   * @param configPath A base path for configuration values.
   * @param pathKeys A set of keys that are expected to represent paths.
   * @param notUsedKeys A set of keys that should be ignored.
   * @param group A name of the configuration group to manage.
   */
  ScConfigGroup(
      sc_config * config,
      std::string configPath,
      std::unordered_set<std::string> const & pathKeys,
      std::unordered_set<std::string> const & notUsedKeys,
      std::string group);

  /*!
   * @brief Retrieves the value associated with a given key from the configuration group.
   *
   * This operator overload allows access to configuration values using the subscript operator `[]`.
   * It retrieves the string value for the specified key from the configuration group. If the key
   * does not exist or if its associated value is null, an empty string is returned. If the key is
   * found in config, it processes the value to ensure that paths are correctly formatted,
   * prepending the configuration path if necessary.
   *
   * @param key A constant reference to a string representing the key for which to retrieve the
   *            associated value.
   *
   * @return A string containing the value associated with the specified key. If the key is not found
   *         or if its value is null, an empty string is returned.
   *
   * @note The function handles path separators and trims whitespace from the paths. If a path does
   *       not start with a '/', it prepends path to current config to ensure correct path resolution.
   */
  std::string operator[](std::string const & key) const;

  /*!
   * @brief Normalizes a given file path relative to the current working directory.
   *
   * This method takes a file path as input and normalizes it by resolving any
   * relative components, removing unnecessary parts, and ensuring that it is
   * represented in a consistent format. If the path is empty, an empty string is
   * returned.
   *
   * @param path A file path to normalize. This should be a string that may
   *              represent either an absolute or relative path.
   * @return A normalized string representation of the file path. This will be
   *         relative to the configuration directory and will not include
   *         unnecessary components or prefixes.
   */
  std::string NormalizePath(std::string const & path) const;

  /*!
   * @brief Retrieves all valid keys in this configuration group.
   *
   * This operator returns a set of keys that are valid for this configuration group, excluding
   * any keys marked as unused.
   *
   * @return An unordered set containing all valid keys in this group.
   */
  std::unordered_set<std::string> operator*() const;

private:
  sc_config * m_config;                           ///< Pointer to the underlying configuration instance.
  std::string m_configPath;                       ///< Base path for resolving relative paths.
  std::unordered_set<std::string> m_pathKeys;     ///< Set of keys representing paths.
  std::unordered_set<std::string> m_notUsedKeys;  ///< Set of keys that are not used.
  std::unordered_set<std::string> m_keys;         ///< Set of valid keys in this group.
  std::string m_group;                            ///< Name of this configuration group.
};

/*!
 * @class ScConfig
 * @brief A class for managing overall configuration settings.
 *
 * The ScConfig class is responsible for initializing and managing a complete set of
 * configuration settings. It allows access to specific groups of settings and checks
 * the validity of the configuration instance.
 */
class ScConfig
{
public:
  static inline sc_char const PATHS_SEPARATOR = ';';
  static inline sc_char const SLASH = '/';

  /*!
   * @brief Constructs an ScConfig object.
   *
   * This constructor initializes the ScConfig instance by loading settings from a specified
   * path and filtering out unused keys. It prepares the instance for further access to
   * configuration groups.
   *
   * @param path A file path to the configuration settings.
   * @param pathKeys A set of keys that are expected to represent paths.
   * @param notUsedKeys A set of keys that should be ignored during initialization.
   */
  ScConfig(
      std::string path,
      std::unordered_set<std::string> const & pathKeys = {},
      std::unordered_set<std::string> const & notUsedKeys = {});

  /*!
   * @brief Checks if the configuration instance is valid.
   *
   * This method returns true if the configuration was successfully initialized; otherwise, false.
   *
   * @return true if valid, false otherwise.
   */
  sc_bool IsValid() const;

  /*!
   * @brief Retrieves a specific configuration group by name.
   *
   * This operator allows access to a specific ScConfigGroup based on its name, providing
   * methods to retrieve values associated with keys in that group.
   *
   * @param group A name of the desired configuration group.
   * @return A ScConfigGroup object representing the specified group.
   */
  ScConfigGroup operator[](std::string const & group) const;

  /*!
   * @brief Retrieves a value associated with a specified key in a given configuration group.
   *
   * This template method allows for the retrieval of configuration values as a specific type.
   * It reads the value associated with the provided group and key, converting it to the desired
   * type using a stringstream.
   *
   * @tparam Type A type to which the parameter value should be converted.
   * @param group A name of the configuration group containing the desired key.
   * @param key A key for which to retrieve the associated value.
   * @return A value associated with the specified group and key, converted to type Type.
   */
  template <class Type>
  Type Get(std::string const & group, std::string const & key) const;

  /*!
   * @brief Retrieves the directory portion of the configuration file path.
   *
   * @return A string representing the directory where the configuration file is located.
   */
  std::string GetDirectory() const;

  /*!
   * @brief Destructor for ScConfig, cleaning up resources.
   */
  ~ScConfig();

private:
  sc_config * m_instance = nullptr;               ///< Pointer to the underlying sc_config instance managing settings.
  std::string m_path;                             ///< Path to the configuration file.
  std::unordered_set<std::string> m_pathKeys;     ///< Set of keys representing paths in config values.
  std::unordered_set<std::string> m_notUsedKeys;  ///< Set of keys that are ignored during initialization.
  sc_bool m_result;                               ///< Result indicating if initialization was successful.
};

#include "sc-config/_template/sc_config.tpp"
