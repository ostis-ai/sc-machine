/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <algorithm>
#include <vector>
#include <string>

extern "C"
{
#include <sc-core/sc_types.h>
}

/*!
 * @class ScOptions
 * @brief A class for handling command-line options.
 *
 * The ScOptions class provides functionality to parse and manage command-line options
 * passed to a program. It allows users to retrieve option values and check for the presence
 * of specific options.
 */
class ScOptions
{
public:
  /*!
   * @brief Constructs an ScOptions object.
   *
   * This constructor takes the command-line arguments and stores them as tokens for
   * further processing.
   *
   * @param argc A number of command-line arguments.
   * @param argv An array of command-line argument strings.
   */
  ScOptions(sc_int const & argc, sc_char ** argv);

  /*!
   * @brief Retrieves the value associated with a specified option.
   *
   * This operator allows access to the value of a given option from the stored tokens.
   * If the option is found, it returns a pair containing the option and its corresponding value.
   *
   * @param options A vector of option strings to search for.
   * @return A pair containing the found option and its value, or an empty pair if not found.
   */
  std::pair<std::string, std::string> operator[](std::vector<std::string> const & options) const;

  /*!
   * @brief Checks if any of the specified options are present in the command-line arguments.
   *
   * This method iterates through the provided options and checks if at least one is present
   * in the stored tokens.
   *
   * @param options A vector of option strings to check for presence.
   * @return True if any option is found; otherwise, false.
   */
  bool Has(std::vector<std::string> const & options) const;

private:
  /*!
   * @brief Formats an option string for upstream processing.
   *
   * This method prefixes an option with one or two dashes based on its length,
   * preparing it for comparison with stored tokens.
   *
   * @param option An original option string to format.
   * @return A formatted string with appropriate dashes.
   */
  static std::string Upstream(std::string const & option);

  std::vector<std::string> m_tokens;  ///< Vector storing command-line argument tokens.
};
