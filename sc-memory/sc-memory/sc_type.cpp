/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_type.hpp"

ScType::ScType()
  : m_realType(0)
{
}

ScType::ScType(RealType type) noexcept
  : m_realType(type)
{
}

ScType::~ScType() = default;

bool ScType::IsEdge() const
{
  return IsConnector();
}

bool ScType::IsConnector() const
{
  return sc_type_has_subtype_in_mask(m_realType, sc_type_connector_mask);
}

bool ScType::IsArc() const
{
  return sc_type_has_subtype_in_mask(m_realType, sc_type_arc_mask);
}

bool ScType::IsCommonEdge() const
{
  return sc_type_is_common_edge(m_realType);
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
  return sc_type_is_node(m_realType) && sc_type_has_subtype(m_realType, sc_type_node_link);
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
  /// TODO: metavar
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

ScType ScType::operator&(ScType const & other)
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

bool ScType::CanExtendTo(ScType const & extType) const
{
  RealType const selfSemType = m_realType & sc_type_element_mask;
  RealType const extSemType = extType.m_realType & sc_type_element_mask;

  // check semantic type
  if (selfSemType != 0 && selfSemType != extSemType)
    return false;

  // check constancy
  RealType const selfConstType = m_realType & sc_type_constancy_mask;
  RealType const extConstType = extType.m_realType & sc_type_constancy_mask;

  if (selfConstType != 0 && selfConstType != extConstType)
    return false;

  if (IsLink())
  {
    if (!extType.IsLink())
      return false;

    ScType const currentType = m_realType & ~sc_type_node_link;
    ScType const extendedType = extType.m_realType & ~sc_type_node_link;

    ScType const selfLinkType = currentType & sc_type_node_link_mask;
    ScType const extLinkType = extendedType & sc_type_node_link_mask;
    if (!selfLinkType.IsUnknown() && selfLinkType != extLinkType)
      return false;
  }
  else if (IsNode())
  {
    if (!extType.IsNode())
      return false;

    ScType const currentType = m_realType & ~sc_type_node;
    ScType const extendedType = extType.m_realType & ~sc_type_node;

    ScType const selfNodeType = currentType & sc_type_node_mask;
    ScType const extNodeType = extendedType & sc_type_node_mask;
    if (!selfNodeType.IsUnknown() && selfNodeType != extNodeType)
      return false;
  }
  else if (IsConnector())
  {
    if (!extType.IsConnector())
      return false;

    ScType const currentType = m_realType & ~sc_type_connector_mask;
    ScType const extendedType = extType.m_realType & ~sc_type_connector_mask;

    ScType const selfActualityType = currentType & sc_type_actuality_mask;
    ScType const extActualityType = extendedType & sc_type_actuality_mask;
    if (!selfActualityType.IsUnknown() && selfActualityType != extActualityType)
      return false;

    ScType const selfPermType = currentType & sc_type_permanency_mask;
    ScType const extPermType = extendedType & sc_type_permanency_mask;
    if (!selfPermType.IsUnknown() && selfPermType != extPermType)
      return false;

    ScType const selfPosType = currentType & sc_type_positivity_mask;
    ScType const extPosType = extendedType & sc_type_positivity_mask;
    if (!selfPosType.IsUnknown() && selfPosType != extPosType)
      return false;
  }

  return true;
}

ScType const ScType::EdgeUCommon(sc_type_common_edge);
ScType const ScType::EdgeDCommon(sc_type_common_arc);

ScType const ScType::EdgeUCommonConst(sc_type_common_edge | sc_type_const);
ScType const ScType::EdgeDCommonConst(sc_type_common_arc | sc_type_const);

ScType const ScType::EdgeAccess(sc_type_membership_arc);
ScType const ScType::EdgeAccessConstPosPerm(
    sc_type_const | sc_type_membership_arc | sc_type_perm_arc | sc_type_pos_arc);
ScType const ScType::EdgeAccessConstNegPerm(
    sc_type_const | sc_type_membership_arc | sc_type_perm_arc | sc_type_neg_arc);
ScType const ScType::EdgeAccessConstFuzPerm(
    sc_type_const | sc_type_membership_arc | sc_type_perm_arc | sc_type_fuz_arc);
ScType const ScType::EdgeAccessConstPosTemp(
    sc_type_const | sc_type_membership_arc | sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::EdgeAccessConstNegTemp(
    sc_type_const | sc_type_membership_arc | sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::EdgeAccessConstFuzTemp(
    sc_type_const | sc_type_membership_arc | sc_type_temp_arc | sc_type_fuz_arc);

ScType const ScType::EdgeUCommonVar(sc_type_common_edge | sc_type_var);
ScType const ScType::EdgeDCommonVar(sc_type_common_arc | sc_type_var);
ScType const ScType::EdgeAccessVarPosPerm(sc_type_var | sc_type_membership_arc | sc_type_perm_arc | sc_type_pos_arc);
ScType const ScType::EdgeAccessVarNegPerm(sc_type_var | sc_type_membership_arc | sc_type_perm_arc | sc_type_neg_arc);
ScType const ScType::EdgeAccessVarFuzPerm(sc_type_var | sc_type_membership_arc | sc_type_perm_arc | sc_type_fuz_arc);
ScType const ScType::EdgeAccessVarPosTemp(sc_type_var | sc_type_membership_arc | sc_type_temp_arc | sc_type_pos_arc);
ScType const ScType::EdgeAccessVarNegTemp(sc_type_var | sc_type_membership_arc | sc_type_temp_arc | sc_type_neg_arc);
ScType const ScType::EdgeAccessVarFuzTemp(sc_type_var | sc_type_membership_arc | sc_type_temp_arc | sc_type_fuz_arc);

ScType const ScType::Const(sc_type_const);
ScType const ScType::Var(sc_type_var);

ScType const ScType::Node(sc_type_node);
ScType const ScType::Link(sc_type_node | sc_type_node_link);
ScType const ScType::Unknown;

ScType const ScType::NodeConst(sc_type_node | sc_type_const);
ScType const ScType::NodeVar(sc_type_node | sc_type_var);

ScType const ScType::LinkConst(sc_type_node | sc_type_node_link | sc_type_const);
ScType const ScType::LinkVar(sc_type_node | sc_type_node_link | sc_type_var);
ScType const ScType::LinkClass(sc_type_node | sc_type_node_link | sc_type_node_class);
ScType const ScType::LinkConstClass(sc_type_node | sc_type_node_link | sc_type_const | sc_type_node_class);
ScType const ScType::LinkVarClass(sc_type_node | sc_type_node_link | sc_type_var | sc_type_node_class);

ScType const ScType::NodeStruct(sc_type_node | sc_type_node_structure);
ScType const ScType::NodeTuple(sc_type_node | sc_type_node_tuple);
ScType const ScType::NodeRole(sc_type_node | sc_type_node_role);
ScType const ScType::NodeNoRole(sc_type_node | sc_type_node_norole);
ScType const ScType::NodeClass(sc_type_node | sc_type_node_class);
ScType const ScType::NodeAbstract(sc_type_node | sc_type_node_superclass);
ScType const ScType::NodeMaterial(sc_type_node | sc_type_node_material);

ScType const ScType::NodeConstStruct(sc_type_node | sc_type_const | sc_type_node_structure);
ScType const ScType::NodeConstTuple(sc_type_node | sc_type_const | sc_type_node_tuple);
ScType const ScType::NodeConstRole(sc_type_node | sc_type_const | sc_type_node_role);
ScType const ScType::NodeConstNoRole(sc_type_node | sc_type_const | sc_type_node_norole);
ScType const ScType::NodeConstClass(sc_type_node | sc_type_const | sc_type_node_class);
ScType const ScType::NodeConstAbstract(sc_type_node | sc_type_const | sc_type_node_superclass);
ScType const ScType::NodeConstMaterial(sc_type_node | sc_type_const | sc_type_node_material);

ScType const ScType::NodeVarStruct(sc_type_node | sc_type_var | sc_type_node_structure);
ScType const ScType::NodeVarTuple(sc_type_node | sc_type_var | sc_type_node_tuple);
ScType const ScType::NodeVarRole(sc_type_node | sc_type_var | sc_type_node_role);
ScType const ScType::NodeVarNoRole(sc_type_node | sc_type_var | sc_type_node_norole);
ScType const ScType::NodeVarClass(sc_type_node | sc_type_var | sc_type_node_class);
ScType const ScType::NodeVarAbstract(sc_type_node | sc_type_var | sc_type_node_superclass);
ScType const ScType::NodeVarMaterial(sc_type_node | sc_type_var | sc_type_node_material);
