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
  static ScType const EdgeUCommon;
  static ScType const EdgeDCommon;

  static ScType const EdgeUCommonConst;
  static ScType const EdgeDCommonConst;

  static ScType const EdgeAccess;

  static ScType const EdgeAccessConstPosPerm;
  static ScType const EdgeAccessConstNegPerm;
  static ScType const EdgeAccessConstFuzPerm;
  static ScType const EdgeAccessConstPosTemp;
  static ScType const EdgeAccessConstNegTemp;
  static ScType const EdgeAccessConstFuzTemp;

  static ScType const EdgeUCommonVar;
  static ScType const EdgeDCommonVar;
  static ScType const EdgeAccessVarPosPerm;
  static ScType const EdgeAccessVarNegPerm;
  static ScType const EdgeAccessVarFuzPerm;
  static ScType const EdgeAccessVarPosTemp;
  static ScType const EdgeAccessVarNegTemp;
  static ScType const EdgeAccessVarFuzTemp;

  static ScType const NodeConst;
  static ScType const NodeVar;

  static ScType const Link;
  static ScType const LinkClass;
  static ScType const NodeStruct;

  static ScType const LinkConst;
  static ScType const LinkConstClass;
  static ScType const NodeConstStruct;
  static ScType const NodeConstTuple;
  static ScType const NodeConstRole;
  static ScType const NodeConstNoRole;
  static ScType const NodeConstClass;
  static ScType const NodeConstMaterial;

  static ScType const LinkVar;
  static ScType const LinkVarClass;
  static ScType const NodeVarStruct;
  static ScType const NodeVarTuple;
  static ScType const NodeVarRole;
  static ScType const NodeVarNoRole;
  static ScType const NodeVarClass;
  static ScType const NodeVarMaterial;
};
