/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <cstdint>

#include <fstream>

extern "C"
{
#include "sc-core/sc-store/sc_types.h"
}

#include "sc_defines.hpp"

#if SC_IS_PLATFORM_WIN32
#  define NOMINMAX
#endif

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

  ~ScType() = default;

  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `IsConnector` instead for better readability and standards "
      "compliance.")

  inline bool IsEdge() const
  {
    return IsConnector();
  }

  inline bool IsConnector() const
  {
    return sc_type_has_subtype_in_mask(m_realType, sc_type_connector_mask);
  }

  inline bool IsArc() const
  {
    return sc_type_has_subtype_in_mask(m_realType, sc_type_arc_mask);
  }

  inline bool IsCommonEdge() const
  {
    return sc_type_has_subtype(m_realType, sc_type_common_edge);
  }

  inline bool IsCommonArc() const
  {
    return sc_type_has_subtype(m_realType, sc_type_common_arc);
  }

  inline bool IsMembershipArc() const
  {
    return sc_type_is_connector(m_realType);
  }

  inline bool IsNode() const
  {
    return sc_type_is_node(m_realType);
  }

  inline bool IsLink() const
  {
    return sc_type_is_node(m_realType) && sc_type_has_subtype(m_realType, sc_type_node_link);
  }

  inline bool IsConst() const
  {
    return sc_type_has_subtype(m_realType, sc_type_const);
  }

  inline bool IsVar() const
  {
    return sc_type_has_subtype(m_realType, sc_type_var);
  }

  inline bool IsUnknown() const
  {
    return m_realType == 0;
  }

  inline bool HasConstancyFlag() const
  {
    return sc_type_has_subtype_in_mask(m_realType, sc_type_constancy_mask);
  }

  // Returns copy of this type, but with variable replaced to const
  inline ScType AsConst() const
  {
    return ScType((m_realType & ~sc_type_var) | sc_type_const);
  }

  // Returns copy of this type, but replace constancy type upward (var -> const)
  inline ScType UpConstType() const
  {
    /// TODO: metavar
    return AsConst();
  }

  inline sc_type operator*() const
  {
    return m_realType;
  }

  ScType & operator()(RealType bits)
  {
    m_realType |= bits;
    return *this;
  }

  inline bool operator==(ScType const & other)
  {
    return (m_realType == other.m_realType);
  }

  inline bool operator!=(ScType const & other)
  {
    return (m_realType != other.m_realType);
  }

  inline RealType BitAnd(RealType const & inMask) const
  {
    return (m_realType & inMask);
  }

  inline ScType operator|(ScType const & other)
  {
    return ScType(m_realType | other.m_realType);
  }

  inline ScType operator&(ScType const & other)
  {
    return ScType(m_realType & other.m_realType);
  }

  inline ScType & operator|=(ScType const & other)
  {
    m_realType |= other.m_realType;
    return *this;
  }

  inline ScType & operator&=(ScType const & other)
  {
    m_realType &= other.m_realType;
    return *this;
  }

  operator RealType() const
  {
    return m_realType;
  }

  /* Check if specified type can be extended by another one to be a valid type/
   * For example this function returns false, if you try to extend node by
   * connectors type, or const by var and etc.
   */
  inline bool CanExtendTo(ScType const & extType) const
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

private:
  RealType m_realType;

public:
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

  static ScType const Const;
  static ScType const Var;

  static ScType const Node;
  static ScType const Link;
  static ScType const LinkClass;
  static ScType const LinkConstClass;
  static ScType const LinkVarClass;
  static ScType const Unknown;

  static ScType const NodeConst;
  static ScType const NodeVar;

  static ScType const LinkConst;
  static ScType const LinkVar;

  static ScType const NodeStruct;
  static ScType const NodeTuple;
  static ScType const NodeRole;
  static ScType const NodeNoRole;
  static ScType const NodeClass;
  static ScType const NodeAbstract;
  static ScType const NodeMaterial;

  static ScType const NodeConstStruct;
  static ScType const NodeConstTuple;
  static ScType const NodeConstRole;
  static ScType const NodeConstNoRole;
  static ScType const NodeConstClass;
  static ScType const NodeConstAbstract;
  static ScType const NodeConstMaterial;

  static ScType const NodeVarStruct;
  static ScType const NodeVarTuple;
  static ScType const NodeVarRole;
  static ScType const NodeVarNoRole;
  static ScType const NodeVarClass;
  static ScType const NodeVarAbstract;
  static ScType const NodeVarMaterial;
};
