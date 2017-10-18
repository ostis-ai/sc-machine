/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "../sc_types.hpp"

#include <unordered_map>
#include <unordered_set>

namespace scs
{
class TypeResolver final
{
public:
  _SC_EXTERN static ScType const & GetConnectorType(std::string const & connectorAlias);
  _SC_EXTERN static ScType const & GetKeynodeType(std::string const & keynodeAlias);

  _SC_EXTERN static bool IsConnectorReversed(std::string const & connectorAlias);
  _SC_EXTERN static bool IsConst(std::string const & idtf);
  _SC_EXTERN static bool IsEdgeAttrConst(std::string const & attr);
  _SC_EXTERN static bool IsKeynodeType(std::string const & alias);

private:
  using MapType = std::unordered_map<std::string, ScType>;
  using IsType = std::unordered_set<std::string>;

  static MapType ms_keynodeToType;
  static MapType ms_connectorToType;
  static IsType ms_reversedConnectors;
};
}
