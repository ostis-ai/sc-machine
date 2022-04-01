/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynode_cache.hpp"

#include "../sc_memory.hpp"

namespace utils
{
ScKeynodeCache::ScKeynodeCache(ScMemoryContext & ctx)
  : m_ctx(ctx)
{
}

ScAddr const & ScKeynodeCache::GetKeynode(std::string const & idtf)
{
  auto const it = m_cache.find(idtf);
  if (it == m_cache.end())
  {
    ScAddr const addr = m_ctx.HelperResolveSystemIdtf(idtf);
    if (addr.IsValid())
    {
      auto const it_insert = m_cache.insert(std::make_pair(idtf, addr));
      SC_CHECK(it_insert.second, ());
      return it_insert.first->second;
    }

    static ScAddr const empty;
    return empty;
  }

  return it->second;
}

}  // namespace utils
