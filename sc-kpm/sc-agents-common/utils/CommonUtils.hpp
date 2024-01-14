/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace utils
{
class CommonUtils
{
public:
  static bool checkType(ScMemoryContext * ms_context, ScAddr const & element, ScType scType);

  SC_DEPRECATED(
      0.9.0,
      "Use bool GetLinkContent(ScAddr const & addr, TContentType & typedContent) instead of. It will be removed in "
      "sc-machine 0.10.0.")
  static std::string getLinkContent(ScMemoryContext * ms_context, ScAddr const & scLink);

  static std::string getIdtf(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScAddr const & idtfRelation,
      ScAddrVector const & linkClasses = {});

  static std::string getMainIdtf(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScAddrVector const & linkClasses = {});

  static void setIdtf(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScAddr const & relation,
      std::string const & identifier,
      ScAddrVector const & linkClasses = {});

  static void setMainIdtf(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      std::string const & identifier,
      ScAddrVector const & linkClasses = {});

  static size_t getSetPower(ScMemoryContext * ms_context, ScAddr const & set);

  static bool isEmpty(ScMemoryContext * ms_context, ScAddr const & set);

  static std::string getAddrHashString(ScAddr const & scAddr);
};

}  // namespace utils
