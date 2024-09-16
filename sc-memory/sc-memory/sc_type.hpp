/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-core/sc-store/sc_types.h"
}

#include "sc_defines.hpp"

class ScMemoryContext;
class ScAddr;

class _SC_EXTERN ScType
{
public:
  using RealType = sc_type;

  explicit constexpr ScType()
    : m_realType(0)
  {
  }

  constexpr ScType(RealType type) noexcept
    : m_realType(type)
  {
  }

  ~ScType();

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `IsConnector` instead for better readability and standards "
      "compliance.")
  bool IsEdge() const;

  bool IsConnector() const;

  bool IsArc() const;

  bool IsCommonEdge() const;

  bool IsCommonArc() const;

  bool IsMembershipArc() const;

  bool IsNode() const;

  bool IsLink() const;

  bool IsConst() const;

  bool IsVar() const;

  bool IsUnknown() const;

  bool HasConstancyFlag() const;

  // Returns copy of this type, but with variable replaced to const
  ScType AsConst() const;

  // Returns copy of this type, but replace constancy type upward (var -> const)
  ScType UpConstType() const;

  sc_type operator*() const;

  ScType & operator()(RealType bits);

  bool operator==(ScType const & other);

  bool operator!=(ScType const & other);

  RealType BitAnd(RealType const & inMask) const;

  ScType operator|(ScType const & other);

  ScType operator&(ScType const & other);

  ScType & operator|=(ScType const & other);

  ScType & operator&=(ScType const & other);

  operator RealType() const;

  /* Check if specified type can be extended by another one to be a valid type/
   * For example this function returns false, if you try to extend node by
   * connectors type, or const by var and etc.
   */
  bool CanExtendTo(ScType const & extType) const;

private:
  RealType m_realType;

public:
  static ScType const Unknown;

  // sc-elements
  static ScType const Node;
  static ScType const Connector;
  static ScType const CommonEdge;
  static ScType const Arc;
  static ScType const CommonArc;
  static ScType const MembershipArc;

  // constancy
  static ScType const Const;
  static ScType const Var;

  static ScType const ConstNode;
  static ScType const VarNode;
  static ScType const ConstConnector;
  static ScType const VarConnector;
  static ScType const ConstCommonEdge;
  static ScType const VarCommonEdge;
  static ScType const ConstArc;
  static ScType const VarArc;
  static ScType const ConstCommonArc;
  static ScType const VarCommonArc;
  static ScType const ConstMembershipArc;
  static ScType const VarMembershipArc;

  // permanency
  static ScType const PermArc;
  static ScType const TempArc;

  static ScType const ConstPermArc;
  static ScType const VarPermArc;
  static ScType const ConstTempArc;
  static ScType const VarTempArc;

  // actuality
  static ScType const ActualTempArc;
  static ScType const InactualTempArc;

  static ScType const ConstActualTempArc;
  static ScType const VarActualTempArc;
  static ScType const ConstInactualTempArc;
  static ScType const VarInactualTempArc;

  // positivity
  static ScType const PosArc;
  static ScType const NegArc;
  static ScType const FuzArc;

  // positive sc-arcs
  static ScType const ConstPosArc;
  static ScType const VarPosArc;

  static ScType const PermPosArc;
  static ScType const TempPosArc;
  static ScType const ActualTempPosArc;
  static ScType const InactualTempPosArc;

  static ScType const ConstPermPosArc;
  static ScType const ConstTempPosArc;
  static ScType const ConstActualTempPosArc;
  static ScType const ConstInactualTempPosArc;

  static ScType const VarPermPosArc;
  static ScType const VarTempPosArc;
  static ScType const VarActualTempPosArc;
  static ScType const VarInactualTempPosArc;

  // negative sc-arcs
  static ScType const ConstNegArc;
  static ScType const VarNegArc;

  static ScType const PermNegArc;
  static ScType const TempNegArc;
  static ScType const ActualTempNegArc;
  static ScType const InactualTempNegArc;

  static ScType const ConstPermNegArc;
  static ScType const ConstTempNegArc;
  static ScType const ConstActualTempNegArc;
  static ScType const ConstInactualTempNegArc;

  static ScType const VarPermNegArc;
  static ScType const VarTempNegArc;
  static ScType const VarActualTempNegArc;
  static ScType const VarInactualTempNegArc;

  // fuzzy sc-arcs
  static ScType const ConstFuzArc;
  static ScType const VarFuzArc;

  // semantic sc-node types
  static ScType const NodeLink;
  static ScType const NodeLinkClass;
  static ScType const NodeTuple;
  static ScType const NodeStructure;
  static ScType const NodeRole;
  static ScType const NodeNoRole;
  static ScType const NodeClass;
  static ScType const NodeSuperclass;
  static ScType const NodeMaterial;

  static ScType const ConstNodeLink;
  static ScType const ConstNodeLinkClass;
  static ScType const ConstNodeTuple;
  static ScType const ConstNodeStructure;
  static ScType const ConstNodeRole;
  static ScType const ConstNodeNoRole;
  static ScType const ConstNodeClass;
  static ScType const ConstNodeSuperclass;
  static ScType const ConstNodeMaterial;

  static ScType const VarNodeLink;
  static ScType const VarNodeLinkClass;
  static ScType const VarNodeTuple;
  static ScType const VarNodeStructure;
  static ScType const VarNodeRole;
  static ScType const VarNodeNoRole;
  static ScType const VarNodeClass;
  static ScType const VarNodeSuperclass;
  static ScType const VarNodeMaterial;

  // deprecated types
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::CommonEdge` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeUCommon;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::CommonArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeDCommon;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstCommonEdge` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeUCommonConst;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstCommonArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeDCommonConst;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::MembershipArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccess;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstPermPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstPosPerm;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstPermNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstNegPerm;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstFuzPerm;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstTempPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstPosTemp;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstTempNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstNegTemp;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstFuzTemp;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarCommonEdge` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeUCommonVar;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarCommonArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeDCommonVar;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarPermPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarPosPerm;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarPermNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarNegPerm;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarFuzPerm;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarTempPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarPosTemp;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarTempNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarNegTemp;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarFuzTemp;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNode` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConst;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNode` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVar;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::NodeLink` instead for better readability and standards "
      "compliance.")
  static ScType const Link;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::NodeLinkClass` instead for better readability and standards "
      "compliance.")
  static ScType const LinkClass;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::NodeStructure` instead for better readability and standards "
      "compliance.")
  static ScType const NodeStruct;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeLink` instead for better readability and standards "
      "compliance.")
  static ScType const LinkConst;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeLinkClass` instead for better readability and standards "
      "compliance.")
  static ScType const LinkConstClass;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeStructure` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstStruct;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeTuple` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstTuple;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstRole;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeNoRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstNoRole;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeClass` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstClass;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::ConstNodeMaterial` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstMaterial;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeLink` instead for better readability and standards "
      "compliance.")
  static ScType const LinkVar;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeLinkClass` instead for better readability and standards "
      "compliance.")
  static ScType const LinkVarClass;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeStructure` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarStruct;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeTuple` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarTuple;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarRole;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeNoRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarNoRole;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeClass` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarClass;
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ScType::VarNodeMaterial` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarMaterial;
};
