#pragma once

#include <unordered_set>

#include "sc-core/sc-store/sc_types.h"

class ScOptions
{
public:
  ScOptions(sc_int32 const & argc, char ** argv)
  {
    for (sc_uint32 i = 1; i < argc; ++i)
      m_tokens.insert(argv[i]);
  }
  std::string operator[](std::string const & option) const
  {
    auto it = m_tokens.find(option);
    if (it != m_tokens.end() && ++it != m_tokens.end())
      return *it;

    return {};
  }

  bool Has(std::string const & option) const
  {
    return m_tokens.find(option) != m_tokens.end();
  }

private:
  std::unordered_set<std::string> m_tokens;
};
