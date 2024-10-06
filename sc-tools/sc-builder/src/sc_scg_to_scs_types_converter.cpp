/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scg_to_scs_types_converter.hpp"

std::unordered_map<std::string, std::string> const SCgToSCsTypesConverter::m_nodeTypeSets = {
    {"node/-/-/not_define", "sc_node"},
    {"node/-/not_define", "sc_node"},

    {"node/const/general_node", "sc_node"},
    {"node/const/terminal", "sc_node"},
    {"node/const/struct", "sc_node_structure"},
    {"node/const/tuple", "sc_node_tuple"},
    {"node/const/role", "sc_node_role_relation"},
    {"node/const/attribute", "sc_node_role_relation"},
    {"node/const/relation", "sc_node_norole_relation"},
    {"node/const/group", "sc_node_class"},
    {"node/const/superclass", "sc_node_superclass"},

    {"node/var/general_node", "sc_node"},
    {"node/var/terminal", "sc_node"},
    {"node/var/struct", "sc_node_structure"},
    {"node/var/tuple", "sc_node_tuple"},
    {"node/var/role", "sc_node_role_relation"},
    {"node/var/attribute", "sc_node_role_relation"},
    {"node/var/relation", "sc_node_norole_relation"},
    {"node/var/group", "sc_node_class"},
    {"node/var/superclass", "sc_node_superclass"},
};

std::unordered_map<std::string, std::string> const SCgToSCsTypesConverter::m_backwardNodeTypes = {
    {"node/-/not_define", "node/-/-/not_define"},      {"node/var/symmetry", "node/var/tuple"},
    {"node/const/general_node", "node/const/general"}, {"node/const/relation", "node/const/relation"},
    {"node/const/attribute", "node/const/role"},       {"node/const/nopredmet", "node/const/struct"},
    {"node/const/material", "node/const/terminal"},    {"node/const/asymmetry", "node/const/tuple"},
    {"node/var/general_node", "node/var/general"},     {"node/var/relation", "node/var/relation"},
    {"node/var/attribute", "node/var/role"},           {"node/var/nopredmet", "node/var/struct"},
    {"node/var/material", "node/var/terminal"},        {"node/const/predmet", "node/const/struct"},
    {"node/const/group", "node/const/group"},          {"node/const/super_group", "node/const/superclass"},
    {"node/var/predmet", "node/var/struct"},           {"node/var/group", "node/var/group"},
    {"node/var/super_group", "node/var/superclass"},
};

std::unordered_map<std::string, std::string> const SCgToSCsTypesConverter::m_unsupportedNodeTypeSets = {
    {"node/const/perm/general", "sc_node"},
    {"node/const/perm/general_node", "sc_node"},
    {"node/const/perm/terminal", "sc_node"},
    {"node/const/perm/struct", "sc_node_structure"},
    {"node/const/perm/tuple", "sc_node_tuple"},
    {"node/const/perm/role", "sc_node_role_relation"},
    {"node/const/perm/attribute", "sc_node_role_relation"},
    {"node/const/perm/relation", "sc_node_norole_relation"},
    {"node/const/perm/group", "sc_node_class"},
    {"node/const/perm/super_group", "sc_node_superclass"},

    {"node/const/temp/general", "sc_node"},
    {"node/const/temp/general_node", "sc_node"},
    {"node/const/temp/terminal", "sc_node"},
    {"node/const/temp/struct", "sc_node_structure"},
    {"node/const/temp/tuple", "sc_node_tuple"},
    {"node/const/temp/role", "sc_node_role_relation"},
    {"node/const/temp/attribute", "sc_node_role_relation"},
    {"node/const/temp/relation", "sc_node_norole_relation"},
    {"node/const/temp/group", "sc_node_class"},
    {"node/const/temp/super_group", "sc_node_superclass"},

    {"node/var/temp/general", "sc_node"},
    {"node/var/temp/general_node", "sc_node"},
    {"node/var/temp/terminal", "sc_node_abstract"},
    {"node/var/temp/struct", "sc_node_structure"},
    {"node/var/temp/tuple", "sc_node_tuple"},
    {"node/var/temp/role", "sc_node_role_relation"},
    {"node/var/temp/attribute", "sc_node_role_relation"},
    {"node/var/temp/relation", "sc_node_norole_relation"},
    {"node/var/temp/group", "sc_node_class"},
    {"node/var/temp/super_group", "sc_node_super_group"},

    {"node/var/perm/general", "sc_node"},
    {"node/var/perm/general_node", "sc_node"},
    {"node/var/perm/terminal", "sc_node"},
    {"node/var/perm/struct", "sc_node_structure"},
    {"node/var/perm/tuple", "sc_node_tuple"},
    {"node/var/perm/role", "sc_node_role_relation"},
    {"node/var/perm/attribute", "sc_node_role_relation"},
    {"node/var/perm/relation", "sc_node_norole_relation"},
    {"node/var/perm/group", "sc_node_class"},
    {"node/var/perm/super_group", "sc_node_superclass"},

    {"node/var/temp/general", "sc_node"},
    {"node/var/temp/general_node", "sc_node"},
    {"node/var/temp/terminal", "sc_node"},
    {"node/var/temp/struct", "sc_node_structure"},
    {"node/var/temp/tuple", "sc_node_tuple"},
    {"node/var/temp/role", "sc_node_role_relation"},
    {"node/var/temp/attribute", "sc_node_role_relation"},
    {"node/var/temp/relation", "sc_node_norole_relation"},
    {"node/var/temp/group", "sc_node_class"},
    {"node/var/temp/super_group", "sc_node_superclass"},

    {"node/meta/perm/general", "sc_node"},
    {"node/meta/perm/general_node", "sc_node"},
    {"node/meta/perm/terminal", "sc_node_abstract"},
    {"node/meta/perm/struct", "sc_node_structure"},
    {"node/meta/perm/tuple", "sc_node_tuple"},
    {"node/meta/perm/role", "sc_node_role_relation"},
    {"node/meta/perm/attribute", "sc_node_role_relation"},
    {"node/meta/perm/relation", "sc_node_norole_relation"},
    {"node/meta/perm/group", "sc_node_class"},
    {"node/meta/perm/super_group", "sc_node_super_group"},

    {"node/meta/temp/general", "sc_node"},
    {"node/meta/temp/general_node", "sc_node"},
    {"node/meta/temp/terminal", "sc_node_abstract"},
    {"node/meta/temp/struct", "sc_node_structure"},
    {"node/meta/temp/tuple", "sc_node_tuple"},
    {"node/meta/temp/role", "sc_node_role_relation"},
    {"node/meta/temp/attribute", "sc_node_role_relation"},
    {"node/meta/temp/relation", "sc_node_norole_relation"},
    {"node/meta/temp/group", "sc_node_class"},
    {"node/meta/temp/super_group", "sc_node_super_group"},
};

std::unordered_map<std::string, std::string> const SCgToSCsTypesConverter::m_connectorTypes = {
    {"pair/const/-/perm/noorien", "<=>"},
    {"pair/const/-/perm/orient", "=>"},
    {"pair/const/fuz/perm/orient/membership", "/>"},
    {"pair/const/neg/perm/orient/membership", "-|>"},
    {"pair/const/pos/perm/orient/membership", "->"},
    {"pair/const/fuz/temp/orient/membership", "/>"},
    {"pair/const/neg/temp/orient/membership", "~|>"},
    {"pair/const/pos/temp/orient/membership", "~>"},
    {"pair/const/-/temp/noorien", "<=>"},
    {"pair/const/-/temp/orient", "=>"},

    {"pair/var/-/perm/noorien", "_<=>"},
    {"pair/var/-/perm/orient", "_=>"},
    {"pair/var/fuz/perm/orient/membership", "_/>"},
    {"pair/var/neg/perm/orient/membership", "_-|>"},
    {"pair/var/pos/perm/orient/membership", "_->"},
    {"pair/var/fuz/temp/orient/membership", "_/>"},
    {"pair/var/neg/temp/orient/membership", "_~|>"},
    {"pair/var/pos/temp/orient/membership", "_~>"},

    {"pair/-/-/-/orient", "?=>"},
    {"pair/-/-/-/noorient", "?<=>"},

    {"arc/-/-", "?.?>"},

    {"arc/const/pos", ".>"},
    {"arc/const/fuz", "/>"},
    {"arc/const/neg", ".|>"},
    {"arc/const/pos/perm", "->"},
    {"arc/const/fuz/perm", "/>"},
    {"arc/const/neg/perm", "-|>"},
    {"arc/const/pos/temp", "~>"},
    {"arc/const/fuz/temp", "/>"},
    {"arc/const/neg/temp", "~|>"},

    {"arc/var/pos", "_.>"},
    {"arc/var/fuz", "_/>"},
    {"arc/var/neg", "_.|>"},
    {"arc/var/pos/perm", "_->"},
    {"arc/var/fuz/perm", "_/>"},
    {"arc/var/neg/perm", "_-|>"},
    {"arc/var/pos/temp", "_~>"},
    {"arc/var/fuz/temp", "_/>"},
    {"arc/var/neg/temp", "_~|>"},

    {"pair/orient", "?=>"},
    {"pair/noorient", "?<=>"},

    {"pair/const/orient", "=>"},
    {"pair/const/noorient", "<=>"},
    {"pair/const/synonym", "<=>"},

    {"pair/var/orient", "_=>"},
    {"pair/var/noorient", "_<=>"}};

std::unordered_map<std::string, std::string> const SCgToSCsTypesConverter::m_backwardConnectorTypes = {
    {"pair/const/synonym", "pair/const/-/perm/noorien"},
    {"pair/const/orient", "pair/const/-/perm/orient"},
    {"arc/const/fuz", "pair/const/fuz/perm/orient/membership"},
    {"arc/const/fuz/temp", "pair/const/fuz/temp/orient/membership"},
    {"arc/const/neg", "pair/const/neg/perm/orient/membership"},
    {"arc/const/neg/temp", "pair/const/neg/temp/orient/membership"},
    {"arc/const/pos", "pair/const/pos/perm/orient/membership"},
    {"arc/const/pos/temp", "pair/const/pos/temp/orient/membership"},
    {"pair/var/noorient", "pair/-/-/-/noorient"},
    {"pair/var/orient", "pair/-/-/-/orient"},
    {"arc/var/fuz", "pair/var/fuz/perm/orient/membership"},
    {"arc/var/fuz/temp", "pair/var/fuz/temp/orient/membership"},
    {"arc/var/neg", "pair/var/neg/perm/orient/membership"},
    {"arc/var/neg/temp", "pair/var/neg/temp/orient/membership"},
    {"arc/var/pos", "pair/var/-/perm/noorien"},
    {"arc/var/pos/temp", "pair/var/pos/temp/orient/membership"},
    {"pair/noorient", "pair/-/-/-/noorient"},
    {"pair/orient", "pair/-/-/-/orient"},
    {"arc/-/-", "pair/-/-/-/orient"}};

std::unordered_map<std::string, std::string> const SCgToSCsTypesConverter::m_unsupportedConnectorTypes = {
    {"pair/var/-/temp/noorien", "sc_pair_var_temp_noorient"},
    {"pair/var/-/temp/orient", "sc_pair_var_temp_orient"},

    {"pair/meta/-/perm/noorien", "sc_pair_meta_perm_noorient"},
    {"pair/meta/-/perm/orient", "sc_pair_meta_perm_orient"},
    {"pair/meta/-/temp/noorien", "sc_pair_meta_temp_noorient"},
    {"pair/meta/-/temp/orient", "sc_pair_meta_temp_orient"},

    {"pair/meta/fuz/perm/orient/membership", "sc_pair_meta_fuz_perm_orient_membership"},
    {"pair/meta/fuz/temp/orient/membership", "sc_pair_meta_fuz_temp_orient_membership"},
    {"pair/meta/neg/perm/orient/membership", "sc_pair_meta_neg_perm_orient_membership"},
    {"pair/meta/neg/temp/orient/membership", "sc_pair_meta_neg_temp_orient_membership"},
    {"pair/meta/pos/perm/orient/membership", "sc_pair_meta_pos_perm_orient_membership"},
    {"pair/meta/pos/temp/orient/membership", "sc_pair_meta_pos_temp_orient_membership"}};

std::string SCgToSCsTypesConverter::GetSCsElementDesignation(
    std::unordered_map<std::string, std::string> const & dictionary,
    std::string const & key)
{
  auto it = dictionary.find(key);
  return it != dictionary.cend() ? it->second : "";
}

bool SCgToSCsTypesConverter::ConvertSCgNodeTypeToSCsNodeType(std::string const & nodeType, std::string & symbol)
{
  symbol = GetSCsElementTypeBySCgElementType(nodeType, "NodeTypeSets");

  if (symbol.empty())
    symbol = GetSCsElementTypeBySCgElementType(
        GetSCsElementTypeBySCgElementType(nodeType, "BackwardNodeTypes"), "NodeTypeSets");

  if (symbol.empty())
  {
    symbol = GetSCsElementTypeBySCgElementType(nodeType, "UnsupportedNodeTypeSets");
    return true;
  }

  return false;
}

bool SCgToSCsTypesConverter::ConvertSCgConnectorTypeToSCsConnectorDesignation(
    std::string const & edgeType,
    std::string & symbol)
{
  symbol = GetSCsElementTypeBySCgElementType(edgeType, "ConnectorTypes");

  if (symbol.empty())
    symbol = GetSCsElementTypeBySCgElementType(
        GetSCsElementTypeBySCgElementType(edgeType, "BackwardConnectorTypes"), "ConnectorTypes");

  if (symbol.empty())
  {
    symbol = GetSCsElementTypeBySCgElementType(edgeType, "UnsupportedConnectorTypes");
    return true;
  }

  return false;
}

std::string SCgToSCsTypesConverter::GetSCsElementTypeBySCgElementType(
    std::string const & scgElement,
    std::string const & dict)
{
  static std::unordered_map<std::string, std::unordered_map<std::string, std::string> const *> const dictMap = {
      {"NodeTypeSets", &m_nodeTypeSets},
      {"BackwardNodeTypes", &m_backwardNodeTypes},
      {"UnsupportedNodeTypeSets", &m_unsupportedNodeTypeSets},
      {"ConnectorTypes", &m_connectorTypes},
      {"BackwardConnectorTypes", &m_backwardConnectorTypes},
      {"UnsupportedConnectorTypes", &m_unsupportedConnectorTypes}};

  auto const it = dictMap.find(dict);
  return GetSCsElementDesignation(*(it->second), scgElement);
}
