/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_type.hpp"

extern "C"
{
#include "sc-core/sc_memory.h"
}

#include "sc-memory/scs/scs_types.hpp"

struct ScType::ScTypeHashFunc
{
  sc_type operator()(ScType const & type) const
  {
    return type;
  }
};

ScType::~ScType() = default;

bool ScType::IsEdge() const
{
  return IsConnector();
}

bool ScType::IsConnector() const
{
  return sc_type_is_connector(m_realType);
}

bool ScType::IsCommonEdge() const
{
  return sc_type_is_common_edge(m_realType);
}

bool ScType::IsArc() const
{
  return sc_type_is_arc(m_realType);
}

bool ScType::IsCommonArc() const
{
  return sc_type_is_common_arc(m_realType);
}

bool ScType::IsMembershipArc() const
{
  return sc_type_is_membership_arc(m_realType);
}

bool ScType::IsNode() const
{
  return sc_type_is_node(m_realType);
}

bool ScType::IsLink() const
{
  return sc_type_is_node_link(m_realType);
}

bool ScType::IsConst() const
{
  return sc_type_has_subtype(m_realType, sc_type_const);
}

bool ScType::IsVar() const
{
  return sc_type_has_subtype(m_realType, sc_type_var);
}

bool ScType::IsUnknown() const
{
  return m_realType == 0;
}

bool ScType::HasConstancyFlag() const
{
  return sc_type_has_subtype_in_mask(m_realType, sc_type_constancy_mask);
}

ScType ScType::AsConst() const
{
  return ScType((m_realType & ~sc_type_var) | sc_type_const);
}

ScType ScType::UpConstType() const
{
  // TODO(NikitaZotov): implement meta-variables
  return AsConst();
}

sc_type ScType::operator*() const
{
  return m_realType;
}

ScType & ScType::operator()(RealType bits)
{
  m_realType |= bits;
  return *this;
}

bool ScType::operator==(ScType const & other)
{
  return m_realType == other.m_realType;
}

bool ScType::operator!=(ScType const & other)
{
  return m_realType != other.m_realType;
}

ScType::RealType ScType::BitAnd(RealType const & inMask) const
{
  return m_realType & inMask;
}

ScType ScType::operator|(ScType const & other)
{
  return ScType(m_realType | other.m_realType);
}

ScType ScType::operator&(ScType const & other) const
{
  return ScType(m_realType & other.m_realType);
}

ScType & ScType::operator|=(ScType const & other)
{
  m_realType |= other.m_realType;
  return *this;
}

ScType & ScType::operator&=(ScType const & other)
{
  m_realType &= other.m_realType;
  return *this;
}

ScType::operator RealType() const
{
  return m_realType;
}

ScType::operator std::string() const
{
  auto const & it = m_typesToNames.find(*this);
  if (it == m_typesToNames.cend())
    return "";

  return it->second;
}

std::string ScType::GetDirectSCsConnector() const
{
  return scs::TypeResolver::GetDirectSCsConnector(*this);
}

std::string ScType::GetReverseSCsConnector() const
{
  return scs::TypeResolver::GetReverseSCsConnector(*this);
}

std::string ScType::GetSCsElementKeynode() const
{
  return scs::TypeResolver::GetSCsElementKeynode(*this);
}

bool ScType::CanExtendTo(ScType const & extType) const
{
  return sc_memory_is_type_extendable_to(*this, extType);
}

std::set<ScType> ScType::GetConnectorTypes()
{
  std::set<ScType> connectorTypes;
  for (auto const & [type, name] : m_typesToNames)
  {
    if (type.IsCommonEdge() || type.IsCommonArc() || type.IsMembershipArc())
      connectorTypes.insert(type);
  }
  return connectorTypes;
}

ScType const ScType::Unknown;

// sc-elements
ScType const ScType::Node(sc_type_node);
ScType const ScType::Connector(sc_type_connector);
ScType const ScType::CommonEdge(sc_type_common_edge);
ScType const ScType::Arc(sc_type_arc);
ScType const ScType::CommonArc(sc_type_common_arc);
ScType const ScType::MembershipArc(sc_type_membership_arc);

// constancy
ScType const ScType::Const(sc_type_const);
ScType const ScType::Var(sc_type_var);

ScType const ScType::ConstNode(sc_type_const_node);
ScType const ScType::VarNode(sc_type_var | sc_type_node);
ScType const ScType::ConstConnector(sc_type_const | sc_type_connector);
ScType const ScType::VarConnector(sc_type_var | sc_type_connector);
ScType const ScType::ConstCommonEdge(sc_type_const | sc_type_common_edge);
ScType const ScType::VarCommonEdge(sc_type_var | sc_type_common_edge);
ScType const ScType::ConstArc(sc_type_const | sc_type_arc);
ScType const ScType::VarArc(sc_type_var | sc_type_arc);
ScType const ScType::ConstCommonArc(sc_type_const | sc_type_common_arc);
ScType const ScType::VarCommonArc(sc_type_var | sc_type_common_arc);
ScType const ScType::ConstMembershipArc(sc_type_const | sc_type_membership_arc);
ScType const ScType::VarMembershipArc(sc_type_var | sc_type_membership_arc);

// permanency
ScType const ScType::PermArc(sc_type_perm_arc);
ScType const ScType::TempArc(sc_type_temp_arc);

ScType const ScType::ConstPermArc(sc_type_const | sc_type_perm_arc);
ScType const ScType::VarPermArc(sc_type_var | sc_type_perm_arc);
ScType const ScType::ConstTempArc(sc_type_const | sc_type_temp_arc);
ScType const ScType::VarTempArc(sc_type_var | sc_type_temp_arc);

// actuality
ScType const ScType::ActualTempArc(sc_type_actual_arc | sc_type_temp_arc);
ScType const ScType::InactualTempArc(sc_type_inactual_arc | sc_type_temp_arc);

ScType const ScType::ConstActualTempArc(sc_type_const | sc_type_actual_arc | sc_type_temp_arc);
ScType const ScType::VarActualTempArc(sc_type_var | sc_type_actual_arc | sc_type_temp_arc);
ScType const ScType::ConstInactualTempArc(sc_type_const | sc_type_inactual_arc | sc_type_temp_arc);
ScType const ScType::VarInactualTempArc(sc_type_var | sc_type_inactual_arc | sc_type_temp_arc);

// positivity
ScType const ScType::PosArc(sc_type_pos_arc);
ScType const ScType::NegArc(sc_type_neg_arc);

// fuzzy sc-arc
ScType const ScType::FuzArc(sc_type_fuz_arc);

// positive sc-arcs
ScType const ScType::ConstPosArc(sc_type_const | sc_type_pos_arc);
ScType const ScType::VarPosArc(sc_type_var | sc_type_pos_arc);

ScType const ScType::PermPosArc(sc_type_perm_arc | sc_type_pos_arc);
ScType const ScType::TempPosArc(sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::ActualTempPosArc(sc_type_actual_arc | sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::InactualTempPosArc(sc_type_inactual_arc | sc_type_temp_arc | sc_type_pos_arc);

ScType const ScType::ConstPermPosArc(sc_type_const | sc_type_perm_arc | sc_type_pos_arc);
ScType const ScType::ConstTempPosArc(sc_type_const | sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::ConstActualTempPosArc(sc_type_const | sc_type_actual_arc | sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::ConstInactualTempPosArc(sc_type_const | sc_type_inactual_arc | sc_type_temp_arc | sc_type_pos_arc);

ScType const ScType::VarPermPosArc(sc_type_var | sc_type_perm_arc | sc_type_pos_arc);
ScType const ScType::VarTempPosArc(sc_type_var | sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::VarActualTempPosArc(sc_type_var | sc_type_actual_arc | sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::VarInactualTempPosArc(sc_type_var | sc_type_inactual_arc | sc_type_temp_arc | sc_type_pos_arc);

// negative sc-arcs
ScType const ScType::ConstNegArc(sc_type_const | sc_type_neg_arc);
ScType const ScType::VarNegArc(sc_type_var | sc_type_neg_arc);

ScType const ScType::PermNegArc(sc_type_perm_arc | sc_type_neg_arc);
ScType const ScType::TempNegArc(sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::ActualTempNegArc(sc_type_actual_arc | sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::InactualTempNegArc(sc_type_inactual_arc | sc_type_temp_arc | sc_type_neg_arc);

ScType const ScType::ConstPermNegArc(sc_type_const | sc_type_perm_arc | sc_type_neg_arc);
ScType const ScType::ConstTempNegArc(sc_type_const | sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::ConstActualTempNegArc(sc_type_const | sc_type_actual_arc | sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::ConstInactualTempNegArc(sc_type_const | sc_type_inactual_arc | sc_type_temp_arc | sc_type_neg_arc);

ScType const ScType::VarPermNegArc(sc_type_var | sc_type_perm_arc | sc_type_neg_arc);
ScType const ScType::VarTempNegArc(sc_type_var | sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::VarActualTempNegArc(sc_type_var | sc_type_actual_arc | sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::VarInactualTempNegArc(sc_type_var | sc_type_inactual_arc | sc_type_temp_arc | sc_type_neg_arc);

// fuzzy sc-arcs
ScType const ScType::ConstFuzArc(sc_type_const | sc_type_fuz_arc);
ScType const ScType::VarFuzArc(sc_type_var | sc_type_fuz_arc);

// semantic sc-node types
ScType const ScType::NodeLink(sc_type_node_link);
ScType const ScType::NodeLinkClass(sc_type_node_link | sc_type_node_class);
ScType const ScType::NodeTuple(sc_type_node_tuple);
ScType const ScType::NodeStructure(sc_type_node_structure);
ScType const ScType::NodeRole(sc_type_node_role);
ScType const ScType::NodeNoRole(sc_type_node_no_role);
ScType const ScType::NodeClass(sc_type_node_class);
ScType const ScType::NodeSuperclass(sc_type_node_superclass);
ScType const ScType::NodeMaterial(sc_type_node_material);

ScType const ScType::ConstNodeLink(sc_type_const | sc_type_node_link);
ScType const ScType::ConstNodeLinkClass(sc_type_const | sc_type_node_link | sc_type_node_class);
ScType const ScType::ConstNodeTuple(sc_type_const | sc_type_node_tuple);
ScType const ScType::ConstNodeStructure(sc_type_const | sc_type_node_structure);
ScType const ScType::ConstNodeRole(sc_type_const | sc_type_node_role);
ScType const ScType::ConstNodeNoRole(sc_type_const | sc_type_node_no_role);
ScType const ScType::ConstNodeClass(sc_type_const | sc_type_node_class);
ScType const ScType::ConstNodeSuperclass(sc_type_const | sc_type_node_superclass);
ScType const ScType::ConstNodeMaterial(sc_type_const | sc_type_node_material);

ScType const ScType::VarNodeLink(sc_type_var | sc_type_node_link);
ScType const ScType::VarNodeLinkClass(sc_type_var | sc_type_node_link | sc_type_node_class);
ScType const ScType::VarNodeTuple(sc_type_var | sc_type_node_tuple);
ScType const ScType::VarNodeStructure(sc_type_var | sc_type_node_structure);
ScType const ScType::VarNodeRole(sc_type_var | sc_type_node_role);
ScType const ScType::VarNodeNoRole(sc_type_var | sc_type_node_no_role);
ScType const ScType::VarNodeClass(sc_type_var | sc_type_node_class);
ScType const ScType::VarNodeSuperclass(sc_type_var | sc_type_node_superclass);
ScType const ScType::VarNodeMaterial(sc_type_var | sc_type_node_material);

ScType const ScType::EdgeUCommon(ScType::CommonEdge);
ScType const ScType::EdgeDCommon(ScType::CommonArc);

ScType const ScType::EdgeUCommonConst(ScType::ConstCommonEdge);
ScType const ScType::EdgeDCommonConst(ScType::ConstCommonArc);

ScType const ScType::EdgeAccess(ScType::MembershipArc);
ScType const ScType::EdgeAccessConstPosPerm(ScType::ConstPermPosArc);
ScType const ScType::EdgeAccessConstNegPerm(ScType::ConstPermNegArc);
ScType const ScType::EdgeAccessConstFuzPerm(ScType::ConstFuzArc);
ScType const ScType::EdgeAccessConstPosTemp(ScType::ConstTempPosArc);
ScType const ScType::EdgeAccessConstNegTemp(ScType::ConstTempNegArc);
ScType const ScType::EdgeAccessConstFuzTemp(ScType::ConstFuzArc);

ScType const ScType::EdgeUCommonVar(ScType::VarCommonEdge);
ScType const ScType::EdgeDCommonVar(ScType::VarCommonArc);
ScType const ScType::EdgeAccessVarPosPerm(ScType::VarPermPosArc);
ScType const ScType::EdgeAccessVarNegPerm(ScType::VarPermNegArc);
ScType const ScType::EdgeAccessVarFuzPerm(ScType::VarFuzArc);
ScType const ScType::EdgeAccessVarPosTemp(ScType::VarTempPosArc);
ScType const ScType::EdgeAccessVarNegTemp(ScType::VarTempNegArc);
ScType const ScType::EdgeAccessVarFuzTemp(ScType::VarFuzArc);

ScType const ScType::NodeConst(ScType::ConstNode);
ScType const ScType::NodeVar(ScType::VarNode);

ScType const ScType::Link(ScType::NodeLink);
ScType const ScType::LinkClass(ScType::NodeLinkClass);
ScType const ScType::NodeStruct(ScType::NodeStructure);

ScType const ScType::LinkConst(ScType::ConstNodeLink);
ScType const ScType::LinkConstClass(ScType::ConstNodeLinkClass);
ScType const ScType::NodeConstTuple(ScType::ConstNodeTuple);
ScType const ScType::NodeConstStruct(ScType::ConstNodeStructure);
ScType const ScType::NodeConstRole(ScType::ConstNodeRole);
ScType const ScType::NodeConstNoRole(ScType::ConstNodeNoRole);
ScType const ScType::NodeConstClass(ScType::ConstNodeClass);
ScType const ScType::NodeConstMaterial(ScType::ConstNodeMaterial);

ScType const ScType::LinkVar(ScType::VarNodeLink);
ScType const ScType::LinkVarClass(ScType::VarNodeLinkClass);
ScType const ScType::NodeVarStruct(ScType::VarNodeStructure);
ScType const ScType::NodeVarTuple(ScType::VarNodeTuple);
ScType const ScType::NodeVarRole(ScType::VarNodeRole);
ScType const ScType::NodeVarNoRole(ScType::VarNodeNoRole);
ScType const ScType::NodeVarClass(ScType::VarNodeClass);
ScType const ScType::NodeVarMaterial(ScType::VarNodeMaterial);

std::unordered_map<ScType, std::string, ScType::ScTypeHashFunc> const ScType::m_typesToNames = {
    {ScType::Unknown, "Unknown"},
    {ScType::Node, "Node"},
    {ScType::Connector, "Connector"},
    {ScType::CommonEdge, "CommonEdge"},
    {ScType::Arc, "Arc"},
    {ScType::CommonArc, "CommonArc"},
    {ScType::MembershipArc, "MembershipArc"},
    {ScType::Const, "Const"},
    {ScType::Var, "Var"},
    {ScType::ConstNode, "ConstNode"},
    {ScType::VarNode, "VarNode"},
    {ScType::ConstConnector, "ConstConnector"},
    {ScType::VarConnector, "VarConnector"},
    {ScType::ConstCommonEdge, "ConstCommonEdge"},
    {ScType::VarCommonEdge, "VarCommonEdge"},
    {ScType::ConstArc, "ConstArc"},
    {ScType::VarArc, "VarArc"},
    {ScType::ConstCommonArc, "ConstCommonArc"},
    {ScType::VarCommonArc, "VarCommonArc"},
    {ScType::ConstMembershipArc, "ConstMembershipArc"},
    {ScType::VarMembershipArc, "VarMembershipArc"},
    {ScType::PermArc, "PermArc"},
    {ScType::TempArc, "TempArc"},
    {ScType::ConstPermArc, "ConstPermArc"},
    {ScType::VarPermArc, "VarPermArc"},
    {ScType::ConstTempArc, "ConstTempArc"},
    {ScType::VarTempArc, "VarTempArc"},
    {ScType::ActualTempArc, "ActualTempArc"},
    {ScType::InactualTempArc, "InactualTempArc"},
    {ScType::ConstActualTempArc, "ConstActualTempArc"},
    {ScType::VarActualTempArc, "VarActualTempArc"},
    {ScType::ConstInactualTempArc, "ConstInactualTempArc"},
    {ScType::VarInactualTempArc, "VarInactualTempArc"},
    {ScType::PosArc, "PosArc"},
    {ScType::NegArc, "NegArc"},
    {ScType::FuzArc, "FuzArc"},
    {ScType::ConstPosArc, "ConstPosArc"},
    {ScType::VarPosArc, "VarPosArc"},
    {ScType::PermPosArc, "PermPosArc"},
    {ScType::TempPosArc, "TempPosArc"},
    {ScType::ActualTempPosArc, "ActualTempPosArc"},
    {ScType::InactualTempPosArc, "InactualTempPosArc"},
    {ScType::ConstPermPosArc, "ConstPermPosArc"},
    {ScType::ConstTempPosArc, "ConstTempPosArc"},
    {ScType::ConstActualTempPosArc, "ConstActualTempPosArc"},
    {ScType::ConstInactualTempPosArc, "ConstInactualTempPosArc"},
    {ScType::VarPermPosArc, "VarPermPosArc"},
    {ScType::VarTempPosArc, "VarTempPosArc"},
    {ScType::VarActualTempPosArc, "VarActualTempPosArc"},
    {ScType::VarInactualTempPosArc, "VarInactualTempPosArc"},
    {ScType::ConstNegArc, "ConstNegArc"},
    {ScType::VarNegArc, "VarNegArc"},
    {ScType::PermNegArc, "PermNegArc"},
    {ScType::TempNegArc, "TempNegArc"},
    {ScType::ActualTempNegArc, "ActualTempNegArc"},
    {ScType::InactualTempNegArc, "InactualTempNegArc"},
    {ScType::ConstPermNegArc, "ConstPermNegArc"},
    {ScType::ConstTempNegArc, "ConstTempNegArc"},
    {ScType::ConstActualTempNegArc, "ConstActualTempNegArc"},
    {ScType::ConstInactualTempNegArc, "ConstInactualTempNegArc"},
    {ScType::VarPermNegArc, "VarPermNegArc"},
    {ScType::VarTempNegArc, "VarTempNegArc"},
    {ScType::VarActualTempNegArc, "VarActualTempNegArc"},
    {ScType::VarInactualTempNegArc, "VarInactualTempNegArc"},
    {ScType::ConstFuzArc, "ConstFuzArc"},
    {ScType::VarFuzArc, "VarFuzArc"},
    {ScType::NodeLink, "NodeLink"},
    {ScType::NodeLinkClass, "NodeLinkClass"},
    {ScType::NodeTuple, "NodeTuple"},
    {ScType::NodeStructure, "NodeStructure"},
    {ScType::NodeRole, "NodeRole"},
    {ScType::NodeNoRole, "NodeNoRole"},
    {ScType::NodeClass, "NodeClass"},
    {ScType::NodeSuperclass, "NodeSuperclass"},
    {ScType::NodeMaterial, "NodeMaterial"},
    {ScType::ConstNodeLink, "ConstNodeLink"},
    {ScType::ConstNodeLinkClass, "ConstNodeLinkClass"},
    {ScType::ConstNodeTuple, "ConstNodeTuple"},
    {ScType::ConstNodeStructure, "ConstNodeStructure"},
    {ScType::ConstNodeRole, "ConstNodeRole"},
    {ScType::ConstNodeNoRole, "ConstNodeNoRole"},
    {ScType::ConstNodeClass, "ConstNodeClass"},
    {ScType::ConstNodeSuperclass, "ConstNodeSuperclass"},
    {ScType::ConstNodeMaterial, "ConstNodeMaterial"},
    {ScType::VarNodeLink, "VarNodeLink"},
    {ScType::VarNodeLinkClass, "VarNodeLinkClass"},
    {ScType::VarNodeTuple, "VarNodeTuple"},
    {ScType::VarNodeStructure, "VarNodeStructure"},
    {ScType::VarNodeRole, "VarNodeRole"},
    {ScType::VarNodeNoRole, "VarNodeNoRole"},
    {ScType::VarNodeClass, "VarNodeClass"},
    {ScType::VarNodeSuperclass, "VarNodeSuperclass"},
    {ScType::VarNodeMaterial, "VarNodeMaterial"}};
