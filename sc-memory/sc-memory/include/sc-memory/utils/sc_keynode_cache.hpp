/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../sc_defines.hpp"
#include "../sc_addr.hpp"

#include <unordered_map>
#include <string>

class ScMemoryContext;

namespace utils
{
// Simple lock without mutex
class ScKeynodeCache
{
public:
  _SC_EXTERN explicit ScKeynodeCache(ScMemoryContext & ctx);

  _SC_EXTERN ScAddr const & GetKeynode(std::string const & idtf);

private:
  ScMemoryContext & m_ctx;

  std::unordered_map<std::string, ScAddr> m_cache;
};

}  // namespace utils
