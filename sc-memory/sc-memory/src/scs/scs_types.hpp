/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "sc-memory/sc_type.hpp"

#include <unordered_map>
#include <unordered_set>

namespace scs
{
class TypeResolver final
{
public:
  static ScType const & GetConnectorType(std::string const & connectorAlias);
  static ScType const & GetKeynodeType(std::string const & keynodeAlias);

  static bool IsConnectorReversed(std::string const & connectorAlias);
  static bool IsConst(std::string const & idtf);
  static bool IsEdgeAttrConst(std::string const & attr);
  static bool IsKeynodeType(std::string const & alias);
  static bool IsUnnamed(std::string const & alias);

private:
  using MapType = std::unordered_map<std::string, ScType>;
  using IsType = std::unordered_set<std::string>;

  static MapType ms_keynodeToType;
  static MapType ms_connectorToType;
  static IsType ms_reversedConnectors;
};
}  // namespace scs
