/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/utils/sc_keynode_cache.hpp"

#include "sc-memory/sc_memory.hpp"

namespace utils
{
ScKeynodeCache::ScKeynodeCache(ScMemoryContext & ctx)
  : m_ctx(ctx)
{
}

ScAddr const & ScKeynodeCache::GetKeynode(std::string const & idtf)
{
  auto const it = m_cache.find(idtf);
  if (it == m_cache.cend())
  {
    ScAddr const addr = m_ctx.ResolveElementSystemIdentifier(idtf);
    if (addr.IsValid())
    {
      auto const it_insert = m_cache.insert(std::make_pair(idtf, addr));
      return it_insert.first->second;
    }

    return ScAddr::Empty;
  }

  return it->second;
}

}  // namespace utils
