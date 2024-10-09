/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <unordered_map>
#include <string>

class SCgToSCsTypesConverter
{
public:
  static void ConvertSCgNodeTypeToSCsNodeType(std::string const & nodeType, std::string & symbol);
  static bool ConvertSCgConnectorTypeToSCsConnectorDesignation(std::string const & edgeType, std::string & symbol);

private:
  static std::string GetSCsElementDesignation(
      std::unordered_map<std::string, std::string> const & dictionary,
      std::string const & key);

  static std::unordered_map<std::string, std::string> const m_nodeTypeSets;
  static std::unordered_map<std::string, std::string> const m_backwardNodeTypes;
  static std::unordered_map<std::string, std::string> const m_unsupportedNodeTypeSets;
  static std::unordered_map<std::string, std::string> const m_connectorTypes;
  static std::unordered_map<std::string, std::string> const m_backwardConnectorTypes;
  static std::unordered_map<std::string, std::string> const m_unsupportedConnectorTypes;
};
