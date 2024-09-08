/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scg_to_scs_types.hpp"

std::unordered_map<std::string, std::string> const SCgToSCsTypes::m_nodeTypeSets = {
    {"node/-/-/not_define", "sc_node"},
    {"node/-/not_define", "sc_node"},

    {"node/const/general_node", "sc_node"},
    {"node/const/terminal", "sc_node_abstract"},
    {"node/const/struct", "sc_node_struct"},
    {"node/const/tuple", "sc_node_tuple"},
    {"node/const/role", "sc_node_role_relation"},
    {"node/const/attribute", "sc_node_role_relation"},
    {"node/const/relation", "sc_node_norole_relation"},
    {"node/const/group", "sc_node_class"},

    {"node/const/perm/general", "sc_node"},
    {"node/const/perm/general_node", "sc_node"},
    {"node/const/perm/terminal", "sc_node_abstract"},
    {"node/const/perm/struct", "sc_node_struct"},
    {"node/const/perm/tuple", "sc_node_tuple"},
    {"node/const/perm/role", "sc_node_role_relation"},
    {"node/const/perm/attribute", "sc_node_role_relation"},
    {"node/const/perm/relation", "sc_node_norole_relation"},
    {"node/const/perm/group", "sc_node_class"},

    {"node/const/temp/general", "sc_node"},
    {"node/const/temp/general_node", "sc_node"},
    {"node/const/temp/terminal", "sc_node_abstract"},
    {"node/const/temp/struct", "sc_node_struct"},
    {"node/const/temp/tuple", "sc_node_tuple"},
    {"node/const/temp/role", "sc_node_role_relation"},
    {"node/const/temp/attribute", "sc_node_role_relation"},
    {"node/const/temp/relation", "sc_node_norole_relation"},
    {"node/const/temp/group", "sc_node_class"},

    {"node/var/general_node", "sc_node"},
    {"node/var/terminal", "sc_node_abstract"},
    {"node/var/struct", "sc_node_struct"},
    {"node/var/tuple", "sc_node_tuple"},
    {"node/var/role", "sc_node_role_relation"},
    {"node/var/attribute", "sc_node_role_relation"},
    {"node/var/relation", "sc_node_norole_relation"},
    {"node/var/group", "sc_node_class"},

    {"node/var/perm/general", "sc_node"},
    {"node/var/perm/general_node", "sc_node"},
    {"node/var/perm/terminal", "sc_node_abstract"},
    {"node/var/perm/struct", "sc_node_struct"},
    {"node/var/perm/tuple", "sc_node_tuple"},
    {"node/var/perm/role", "sc_node_role_relation"},
    {"node/var/perm/attribute", "sc_node_role_relation"},
    {"node/var/perm/relation", "sc_node_norole_relation"},
    {"node/var/perm/group", "sc_node_class"},

    {"node/var/temp/general", "sc_node"},
    {"node/var/temp/general_node", "sc_node"},
    {"node/var/temp/terminal", "sc_node_abstract"},
    {"node/var/temp/struct", "sc_node_struct"},
    {"node/var/temp/tuple", "sc_node_tuple"},
    {"node/var/temp/role", "sc_node_role_relation"},
    {"node/var/temp/attribute", "sc_node_role_relation"},
    {"node/var/temp/relation", "sc_node_norole_relation"},
    {"node/var/temp/group", "sc_node_class"},
};

std::unordered_map<std::string, std::string> const SCgToSCsTypes::m_backwardNodeTypes = {
    {"node/-/not_define", "node/-/-/not_define"},
    {"node/var/symmetry", "node/var/perm/tuple"},
    {"node/const/general_node", "node/const/perm/general"},
    {"node/const/relation", "node/const/perm/relation"},
    {"node/const/attribute", "node/const/perm/role"},
    {"node/const/nopredmet", "node/const/perm/struct"},
    {"node/const/material", "node/const/perm/terminal"},
    {"node/const/asymmetry", "node/const/perm/tuple"},
    {"node/var/general_node", "node/var/perm/general"},
    {"node/var/relation", "node/var/perm/relation"},
    {"node/var/attribute", "node/var/perm/role"},
    {"node/var/nopredmet", "node/var/perm/struct"},
    {"node/var/material", "node/var/perm/terminal"},
    {"node/const/predmet", "node/const/temp/struct"},
    {"node/const/group", "node/const/perm/group"},
    {"node/var/predmet", "node/var/temp/struct"},
    {"node/var/group", "node/var/perm/group"},
};

std::unordered_map<std::string, std::string> const SCgToSCsTypes::m_unsupportedNodeTypeSets = {
    {"node/const/perm/super_group", "sc_node_super_group"},

    {"node/const/temp/general", "sc_node_temp"},
    {"node/const/temp/general_node", "sc_node_temp"},
    {"node/const/temp/terminal", "sc_node_abstract_temp"},
    {"node/const/temp/struct", "sc_node_struct_temp"},
    {"node/const/temp/tuple", "sc_node_tuple_temp"},
    {"node/const/temp/role", "sc_node_role_relation_temp"},
    {"node/const/temp/attribute", "sc_node_role_relation_temp"},
    {"node/const/temp/relation", "sc_node_norole_relation_temp"},
    {"node/const/temp/group", "sc_node_class_temp"},
    {"node/const/temp/super_group", "sc_node_super_group_temp"},

    {"node/var/perm/super_group", "sc_node_super_group"},

    {"node/var/temp/general", "sc_node_temp"},
    {"node/var/temp/general_node", "sc_node_temp"},
    {"node/var/temp/terminal", "sc_node_abstract_temp"},
    {"node/var/temp/struct", "sc_node_struct_temp"},
    {"node/var/temp/tuple", "sc_node_tuple_temp"},
    {"node/var/temp/role", "sc_node_role_relation_temp"},
    {"node/var/temp/attribute", "sc_node_role_relation_temp"},
    {"node/var/temp/relation", "sc_node_norole_relation_temp"},
    {"node/var/temp/group", "sc_node_class_temp"},
    {"node/var/temp/super_group", "sc_node_super_group_temp"},

    {"node/meta/perm/general", "sc_node_meta"},
    {"node/meta/perm/general_node", "sc_node_meta"},
    {"node/meta/perm/terminal", "sc_node_abstract_meta"},
    {"node/meta/perm/struct", "sc_node_struct_meta"},
    {"node/meta/perm/tuple", "sc_node_tuple_meta"},
    {"node/meta/perm/role", "sc_node_role_relation_meta"},
    {"node/meta/perm/attribute", "sc_node_role_relation_meta"},
    {"node/meta/perm/relation", "sc_node_norole_relation_meta"},
    {"node/meta/perm/group", "sc_node_class_meta"},
    {"node/meta/perm/super_group", "sc_node_super_group_meta"},

    {"node/meta/temp/general", "sc_node_meta_temp"},
    {"node/meta/temp/general_node", "sc_node_meta_temp"},
    {"node/meta/temp/terminal", "sc_node_abstract_meta_temp"},
    {"node/meta/temp/struct", "sc_node_struct_meta_temp"},
    {"node/meta/temp/tuple", "sc_node_tuple_meta_temp"},
    {"node/meta/temp/role", "sc_node_role_relation_meta_temp"},
    {"node/meta/temp/attribute", "sc_node_role_relation_meta_temp"},
    {"node/meta/temp/relation", "sc_node_norole_relation_meta_temp"},
    {"node/meta/temp/group", "sc_node_class_meta_temp"},
    {"node/meta/temp/super_group", "sc_node_super_group_meta_temp"},
};

std::unordered_map<std::string, std::string> const SCgToSCsTypes::m_connectorTypes = {
    {"pair/const/-/perm/noorien", "<=>"},
    {"pair/const/-/perm/orient", "=>"},
    {"pair/const/fuz/perm/orient/membership", "-/>"},
    {"pair/const/neg/perm/orient/membership", "-|>"},
    {"pair/const/pos/perm/orient/membership", "->"},
    {"pair/const/fuz/temp/orient/membership", "~/>"},
    {"pair/const/neg/temp/orient/membership", "~|>"},
    {"pair/const/pos/temp/orient/membership", "~>"},
    {"pair/const/-/temp/noorien", "<=>"},
    {"pair/const/-/temp/orient", "=>"},

    {"pair/var/-/perm/noorien", "_<=>"},
    {"pair/var/-/perm/orient", "_=>"},
    {"pair/var/fuz/perm/orient/membership", "_-/>"},
    {"pair/var/neg/perm/orient/membership", "_-|>"},
    {"pair/var/pos/perm/orient/membership", "_->"},
    {"pair/var/fuz/temp/orient/membership", "_~/>"},
    {"pair/var/neg/temp/orient/membership", "_~|>"},
    {"pair/var/pos/temp/orient/membership", "_~>"},

    {"pair/-/-/-/orient", ">"},
    {"pair/-/-/-/noorient", "<>"},

    {"arc/-/-", "..>"},

    {"arc/const/pos", "->"},
    {"arc/const/fuz", "-/>"},
    {"arc/const/neg", "-|>"},
    {"arc/const/pos/temp", "~>"},
    {"arc/const/fuz/temp", "~/>"},
    {"arc/const/neg/temp", "~|>"},

    {"arc/var/pos", "_->"},
    {"arc/var/fuz", "_-/>"},
    {"arc/var/neg", "_-|>"},
    {"arc/var/pos/temp", "_~>"},
    {"arc/var/fuz/temp", "_~/>"},
    {"arc/var/neg/temp", "_~|>"},

    {"pair/orient", ">"},
    {"pair/noorient", "<>"},

    {"pair/const/orient", "=>"},
    {"pair/const/noorient", "<=>"},
    {"pair/const/synonym", "<=>"},

    {"pair/var/orient", "_=>"},
    {"pair/var/noorient", "_<=>"}};

std::unordered_map<std::string, std::string> const SCgToSCsTypes::m_backwardConnectorTypes = {
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

std::unordered_map<std::string, std::string> const SCgToSCsTypes::m_unsupportedConnectorTypes = {
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

std::string SCgToSCsTypes::FindValue(
    std::unordered_map<std::string, std::string> const & dictionary,
    std::string const & key)
{
  auto it = dictionary.find(key);
  return it != dictionary.cend() ? it->second : "";
}

bool SCgToSCsTypes::ConvertSCgNodeTypeToSCsElementType(std::string const & nodeType, std::string & symbol)
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

bool SCgToSCsTypes::ConvertSCgEdgeTypeToSCsElementType(std::string const & edgeType, std::string & symbol)
{
  symbol = GetSCsElementTypeBySCgElementType(edgeType, "EdgeTypes");

  if (symbol.empty())
    symbol = GetSCsElementTypeBySCgElementType(
        GetSCsElementTypeBySCgElementType(edgeType, "BackwardEdgeTypes"), "EdgeTypes");

  if (symbol.empty())
  {
    symbol = GetSCsElementTypeBySCgElementType(edgeType, "UnsupportedEdgeTypes");
    return true;
  }

  return false;
}

std::string SCgToSCsTypes::GetSCsElementTypeBySCgElementType(std::string const & scgElement, std::string const & dict)
{
  static std::unordered_map<std::string, std::unordered_map<std::string, std::string> const *> const dictMap = {
      {"NodeTypeSets", &m_nodeTypeSets},
      {"BackwardNodeTypes", &m_backwardNodeTypes},
      {"UnsupportedNodeTypeSets", &m_unsupportedNodeTypeSets},
      {"EdgeTypes", &m_connectorTypes},
      {"BackwardEdgeTypes", &m_backwardConnectorTypes},
      {"UnsupportedEdgeTypes", &m_unsupportedConnectorTypes}};

  auto it = dictMap.find(dict);
  if (it == dictMap.end())
    return "";

  return FindValue(*(it->second), scgElement);
}
