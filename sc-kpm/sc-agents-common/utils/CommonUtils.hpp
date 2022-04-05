/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

using namespace std;

namespace utils
{
class CommonUtils
{
public:
  static bool checkType(ScMemoryContext * ms_context, const ScAddr & element, ScType scType);

  SC_DEPRECATED(0.6.0, "Removed in 0.6.0")
  static int readInt(ScMemoryContext * ms_context, const ScAddr & scLink);

  SC_DEPRECATED(0.6.0, "Removed in 0.6.0")
  static int readNumber(ScMemoryContext * ms_context, const ScAddr & number);

  SC_DEPRECATED(
      0.6.0,
      "Use CommonUtils::getLinkContent"
      "(ScMemoryContext * ms_context, const ScAddr & scLink) instead of.")
  static string readString(ScMemoryContext * ms_context, const ScAddr & scLink);

  static string getLinkContent(ScMemoryContext * ms_context, const ScAddr & scLink);

  SC_DEPRECATED(
      0.6.0,
      "Use CommonUtils::getIdtf"
      "(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & idtfRelation) instead of.")
  static string getIdtfValue(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & idtfRelation);

  static string getIdtf(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddr & idtfRelation,
      const ScAddrVector & linkClasses = {});

  static string getMainIdtf(ScMemoryContext * ms_context, const ScAddr & node, const ScAddrVector & linkClasses = {});

  static void setIdtf(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddr & relation,
      const string & identifier,
      const ScAddrVector & linkClasses = {});

  static void setMainIdtf(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const string & identifier,
      const ScAddrVector & linkClasses = {});

  SC_DEPRECATED(
      0.6.0,
      "Use CommonUtils::getSetPower"
      "(ScMemoryContext * ms_context, const ScAddr & set) instead of.")
  static int getPowerOfSet(ScMemoryContext * ms_context, const ScAddr & set);

  static size_t getSetPower(ScMemoryContext * ms_context, const ScAddr & set);

  static bool isEmpty(ScMemoryContext * ms_context, const ScAddr & set);
};

}  // namespace utils
