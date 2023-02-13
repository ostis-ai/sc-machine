/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
#include "sc-memory/scs/scs_types.hpp"

#include "sc-memory/sc_debug.hpp"

namespace scs
{

TypeResolver::MapType TypeResolver::ms_connectorToType = {
    {">", ScType::EdgeDCommon},
    {"<", ScType::EdgeDCommon},
    {"<>", ScType::EdgeUCommon},
    {"..>", ScType::EdgeAccess},
    {"<..", ScType::EdgeAccess},
    {"<=>", ScType::EdgeUCommonConst},
    {"_<=>", ScType::EdgeUCommonVar},
    {"__<=>", ScType::EdgeCommonMetaVar},
    {"=>", ScType::EdgeDCommonConst},
    {"<=", ScType::EdgeDCommonConst},
    {"_=>", ScType::EdgeDCommonVar},
    {"_<=", ScType::EdgeDCommonVar},
    {"<=_", ScType::EdgeDCommonVar},
    {"__=>", ScType::ArcCommonMetaVar},
    {"__<=", ScType::ArcCommonMetaVar},
    {"->", ScType::EdgeAccessConstPosPerm},
    {"<-", ScType::EdgeAccessConstPosPerm},
    {"_->", ScType::EdgeAccessVarPosPerm},
    {"<-_", ScType::EdgeAccessVarPosPerm},
    {"_<-", ScType::EdgeAccessVarPosPerm},
    {"__->", ScType::EdgeAccessMetaVarPosPerm},
    {"__<-", ScType::EdgeAccessMetaVarPosPerm},
    {"-|>", ScType::EdgeAccessConstNegPerm},
    {"<|-", ScType::EdgeAccessConstNegPerm},
    {"_-|>", ScType::EdgeAccessVarNegPerm},
    {"_<|-", ScType::EdgeAccessVarNegPerm},
    {"<|-_", ScType::EdgeAccessVarNegPerm},
    {"__-|>", ScType::EdgeAccessMetaVarNegPerm},
    {"__<|-", ScType::EdgeAccessMetaVarNegPerm},
    {"-/>", ScType::EdgeAccessConstFuzPerm},
    {"</-", ScType::EdgeAccessConstFuzPerm},
    {"_-/>", ScType::EdgeAccessVarFuzPerm},
    {"_</-", ScType::EdgeAccessVarFuzPerm},
    {"</-_", ScType::EdgeAccessVarFuzPerm},
    {"__-/>", ScType::EdgeAccessMetaVarFuzPerm},
    {"__</-", ScType::EdgeAccessMetaVarFuzPerm},
    {"~>", ScType::EdgeAccessConstPosTemp},
    {"<~", ScType::EdgeAccessConstPosTemp},
    {"_~>", ScType::EdgeAccessVarPosTemp},
    {"_<~", ScType::EdgeAccessVarPosTemp},
    {"<~_", ScType::EdgeAccessVarPosTemp},
    {"__~>", ScType::EdgeAccessMetaVarPosTemp},
    {"__<~", ScType::EdgeAccessMetaVarPosTemp},
    {"~|>", ScType::EdgeAccessConstNegTemp},
    {"<|~", ScType::EdgeAccessConstNegTemp},
    {"_~|>", ScType::EdgeAccessVarNegTemp},
    {"_<|~", ScType::EdgeAccessVarNegTemp},
    {"<|~_", ScType::EdgeAccessVarNegTemp},
    {"__~|>", ScType::EdgeAccessMetaVarNegTemp},
    {"__<|~", ScType::EdgeAccessMetaVarNegTemp},
    {"~/>", ScType::EdgeAccessConstFuzTemp},
    {"</~", ScType::EdgeAccessConstFuzTemp},
    {"_~/>", ScType::EdgeAccessVarFuzTemp},
    {"_</~", ScType::EdgeAccessVarFuzTemp},
    {"</~_", ScType::EdgeAccessVarFuzTemp},
    {"__~/>", ScType::EdgeAccessMetaVarFuzTemp},
    {"__</~", ScType::EdgeAccessMetaVarFuzTemp}};

TypeResolver::MapType TypeResolver::ms_keynodeToType = {
    {"sc_node", ScType::Node},
    {"sc_link", ScType::Link},
    {"sc_link_class", ScType::LinkClass},
    {"sc_arc_common", ScType::EdgeDCommon},
    {"sc_edge_dcommon", ScType::EdgeDCommon},
    {"sc_edge", ScType::EdgeUCommon},
    {"sc_edge_ucommon", ScType::EdgeUCommon},
    {"sc_arc_main", ScType::EdgeAccessConstPosPerm},
    {"sc_edge_main", ScType::EdgeAccessConstPosPerm},
    {"sc_arc_access", ScType::EdgeAccess},
    {"sc_edge_access", ScType::EdgeAccess},

    {"sc_node_tuple", ScType::NodeTuple},
    {"sc_node_struct", ScType::NodeStruct},
    {"sc_node_role_relation", ScType::NodeRole},
    {"sc_node_norole_relation", ScType::NodeNoRole},
    {"sc_node_class", ScType::NodeClass},
    {"sc_node_abstract", ScType::NodeAbstract},
    {"sc_node_material", ScType::NodeMaterial},

    // backward compatibility
    {"sc_node_not_relation", ScType::NodeClass},
    {"sc_node_not_binary_tuple", ScType::NodeTuple}};

TypeResolver::IsType TypeResolver::ms_reversedConnectors =
    {"<", "<..", "<=", "_<=", "<=_", "__<=", "<=__", "<-", "_<-", "<-_", "__<-", "<-__", "<|-", "_<|-", "<|-_", "__<|-", "<|-__", "</-", "_</-", "</-_", "__</-", "</-__", "<~", "_<~", "<~_", "__<~", "<~__", "<|~", "<|~_", "_<|~", "<|~__", "__<|~", "</~", "_</~", "</~_", "__</~", "</~__"};

std::map<std::string, std::string> TypeResolver::ms_edgeAttrToEdge = {
    {":", "->"},
    {"::", "_->"},
    {":::", "__->"}};

ScType const & TypeResolver::GetConnectorType(std::string const & connectorAlias)
{
  auto const it = ms_connectorToType.find(connectorAlias);
  return (it != ms_connectorToType.end()) ? it->second : ScType::Unknown;
}

ScType const & TypeResolver::GetKeynodeType(std::string const & keynodeAlias)
{
  auto const it = ms_keynodeToType.find(keynodeAlias);
  return (it != ms_keynodeToType.end()) ? it->second : ScType::Unknown;
}

bool TypeResolver::IsConnectorReversed(std::string const & connectorAlias)
{
  return (ms_reversedConnectors.find(connectorAlias) != ms_reversedConnectors.end());
}

ScType TypeResolver::GetType(std::string const & idtf)
{
  if (idtf.empty())
    return true;

  size_t const n = idtf.size() - 1;
  size_t i = 0;
  while ((idtf[i] == '.') && (i < n))
    ++i;

  if (idtf[i] != '_')
    return ScType::Const;
  else if (idtf[i + 1] != '_')
    return ScType::Var;

  return ScType::MetaVar;
}

std::string TypeResolver::DefineEdgeAttrType(std::string const & attr)
{
  return ms_edgeAttrToEdge[attr];
}

bool TypeResolver::IsKeynodeType(std::string const & alias)
{
  return (ms_keynodeToType.find(alias) != ms_keynodeToType.end());
}

bool TypeResolver::IsUnnamed(std::string const & alias)
{
  return (alias == "...");
}

}  // namespace scs
