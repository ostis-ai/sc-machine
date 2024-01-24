/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <algorithm>
#include <vector>
#include <string>

#include "sc-core/sc-store/sc_types.h"

class ScOptions
{
public:
  ScOptions(sc_int const & argc, sc_char ** argv);

  std::pair<std::string, std::string> operator[](std::vector<std::string> const & options) const;

  bool Has(std::vector<std::string> const & options) const;

private:
  std::vector<std::string> m_tokens;

  static std::string Upstream(std::string const & option);
};
