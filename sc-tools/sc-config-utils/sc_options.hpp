/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <algorithm>
#include <vector>
#include <sstream>

#include "sc-core/sc-store/sc_types.h"

class ScOptions
{
public:
  ScOptions(sc_int const & argc, char ** argv)
  {
    for (sc_int i = 1; i < argc; ++i)
      m_tokens.emplace_back(argv[i]);
  }
  std::pair<std::string, std::string> operator[](std::vector<std::string> const & options) const
  {
    for (auto const & item : options)
    {
      std::string const & option = Upstream(item);

      auto it = std::find(m_tokens.begin(), m_tokens.end(), option);
      if (it != m_tokens.end() && ++it != m_tokens.end())
        return {item, *it};
    }

    return {};
  }

  bool Has(std::vector<std::string> const & options) const
  {
    for (auto const & item : options)
    {
      std::string const & option = Upstream(item);

      if (std::find(m_tokens.begin(), m_tokens.end(), option) != m_tokens.end())
        return SC_TRUE;
    }

    return SC_FALSE;
  }

private:
  std::vector<std::string> m_tokens;

  static std::string Upstream(std::string const & option)
  {
    std::stringstream stream;
    stream << "-";

    if (option.length() > 1)
      stream << "-";

    stream << option;

    return stream.str();
  }
};
