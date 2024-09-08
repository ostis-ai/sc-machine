/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <unordered_map>
#include <string>

class SCgToSCsTypes
{
public:
  static bool ConvertSCgNodeTypeToSCsElementType(std::string const & nodeType, std::string & symbol);
  static bool ConvertSCgEdgeTypeToSCsElementType(std::string const & edgeType, std::string & symbol);

private:
  static std::string FindValue(
      std::unordered_map<std::string, std::string> const & dictionary,
      std::string const & key);

  static std::string GetSCsElementTypeBySCgElementType(std::string const & scgElement, std::string const & dict);

  static std::unordered_map<std::string, std::string> const m_nodeTypeSets;
  static std::unordered_map<std::string, std::string> const m_backwardNodeTypes;
  static std::unordered_map<std::string, std::string> const m_unsupportedNodeTypeSets;
  static std::unordered_map<std::string, std::string> const m_connectorTypes;
  static std::unordered_map<std::string, std::string> const m_backwardConnectorTypes;
  static std::unordered_map<std::string, std::string> const m_unsupportedConnectorTypes;
};
