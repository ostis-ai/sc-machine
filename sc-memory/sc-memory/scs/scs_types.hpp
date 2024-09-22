/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "sc-memory/sc_type.hpp"

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace scs
{
class TypeResolver final
{
public:
  static std::string GetDirectSCsConnectorType(ScType const & type);
  static std::string GetReverseSCsConnectorType(ScType const & type);

  static std::string GetSCsElementKeynode(ScType const & type);

  static ScType const & GetConnectorType(std::string const & connectorAlias);
  static ScType const & GetKeynodeType(std::string const & keynodeAlias);

  static bool IsConnectorReversed(std::string const & connectorAlias);
  static bool IsConst(std::string const & idtf);
  static bool IsConnectorAttrConst(std::string const & attr);
  static bool IsKeynodeType(std::string const & alias);
  static bool IsUnnamed(std::string const & alias);

protected:
  using SCsDesignationsToScTypes = std::unordered_map<std::string, ScType>;

  struct ScTypeHashFunc;
  using ScTypesToSCsDesignations = std::unordered_map<ScType, std::string, TypeResolver::ScTypeHashFunc>;
  using SCsConnectorDesignations = std::unordered_set<std::string>;

  static SCsDesignationsToScTypes ms_connectorsToTypes;
  static SCsDesignationsToScTypes ms_deprecatedConnectorsToTypes;
  static SCsDesignationsToScTypes ms_deprecatedReverseConnectorsToTypes;

  static ScTypesToSCsDesignations ms_typesToConnectors;
  static ScTypesToSCsDesignations ms_typesToReverseConnectors;

  static SCsDesignationsToScTypes ms_keynodesToTypes;
  static SCsDesignationsToScTypes ms_deprecatedKeynodesToTypes;
  
  struct ScTypeEqualFunc;
  static ScTypesToSCsDesignations ms_typesToKeynodes;

  static SCsConnectorDesignations ms_reversedConnectors;
  static SCsConnectorDesignations ms_deprecatedReversedConnectors;
};
}  // namespace scs
