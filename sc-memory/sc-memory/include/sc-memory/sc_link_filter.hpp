/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_addr.hpp"

class ScLinkFilter
{
public:
  enum class ScLinkFilterRequest : sc_uint8
  {
    CONTINUE,
    STOP
  };

  virtual bool CheckLink(ScAddr const & linkAddr) = 0;
  virtual ScLinkFilterRequest RequestLink(ScAddr const & linkAddr) = 0;
};
