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
    {"=>", ScType::EdgeDCommonConst},
    {"<=", ScType::EdgeDCommonConst},
    {"_=>", ScType::EdgeDCommonVar},
    {"_<=", ScType::EdgeDCommonVar},
    {"->", ScType::EdgeAccessConstPosPerm},
    {"<-", ScType::EdgeAccessConstPosPerm},
    {"_->", ScType::EdgeAccessVarPosPerm},
    {"_<-", ScType::EdgeAccessVarPosPerm},
    {"-|>", ScType::EdgeAccessConstNegPerm},
    {"<|-", ScType::EdgeAccessConstNegPerm},
    {"_-|>", ScType::EdgeAccessVarNegPerm},
    {"_<|-", ScType::EdgeAccessVarNegPerm},
    {"-/>", ScType::EdgeAccessConstFuzPerm},
    {"</-", ScType::EdgeAccessConstFuzPerm},
    {"_-/>", ScType::EdgeAccessVarFuzPerm},
    {"_</-", ScType::EdgeAccessVarFuzPerm},
    {"~>", ScType::EdgeAccessConstPosTemp},
    {"<~", ScType::EdgeAccessConstPosTemp},
    {"_~>", ScType::EdgeAccessVarPosTemp},
    {"_<~", ScType::EdgeAccessVarPosTemp},
    {"~|>", ScType::EdgeAccessConstNegTemp},
    {"<|~", ScType::EdgeAccessConstNegTemp},
    {"_~|>", ScType::EdgeAccessVarNegTemp},
    {"_<|~", ScType::EdgeAccessVarNegTemp},
    {"~/>", ScType::EdgeAccessConstFuzTemp},
    {"</~", ScType::EdgeAccessConstFuzTemp},
    {"_~/>", ScType::EdgeAccessVarFuzTemp},
    {"_</~", ScType::EdgeAccessVarFuzTemp}};

TypeResolver::MapType TypeResolver::ms_keynodeToType = {
    {"sc_node", ScType::Node},
    {"sc_link", ScType::Link},
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
    {"<", "<..", "<=", "_<=", "<-", "_<-", "<|-", "_<|-", "</-", "_</-", "<~", "_<~", "<|~", "_<|~", "</~", "_</~"};

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

bool TypeResolver::IsConst(std::string const & idtf)
{
  SC_ASSERT(!idtf.empty(), ());

  size_t const n = idtf.size() - 1;
  size_t i = 0;
  while ((idtf[i] == '.') && (i < n))
    ++i;

  return (idtf[i] != '_');
}

bool TypeResolver::IsEdgeAttrConst(std::string const & attr)
{
  return (attr == ":");
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
