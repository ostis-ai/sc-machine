/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-core/sc_memory.h"
}

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

  explicit ScType()
    : m_realType(0)
  {
  }

  explicit ScType(RealType type) noexcept
    : m_realType(type)
  {
  }

  inline bool IsEdge() const
  {
    return (m_realType & sc_type_arc_mask) != 0;
  }
  inline bool IsNode() const
  {
    return (m_realType & sc_type_node) != 0;
  }
  inline bool IsLink() const
  {
    return (m_realType & sc_type_link) != 0;
  }
  inline bool IsConst() const
  {
    return (m_realType & sc_type_const) != 0;
  }
  inline bool IsVar() const
  {
    return (m_realType & sc_type_var) != 0;
  }

  SC_DEPRECATED(0.4.0, "Use !IsUnknown() instead")
  inline bool IsValid() const
  {
    return !IsUnknown();
  }

  inline bool IsUnknown() const
  {
    return (m_realType == 0);
  }

  inline bool HasConstancyFlag() const
  {
    return (m_realType & sc_type_constancy_mask) != 0;
  }

  // Returns copy of this type, but with variable raplaced to const
  inline ScType AsConst() const
  {
    return ScType((m_realType & ~sc_type_var) | sc_type_const);
  }

  // Returns copy of this type, but replace constancy type upward (metavar -> var -> const)
  inline ScType UpConstType() const
  {
    /// TODO: metavar
    // if (isVar())
    return ScType((m_realType & ~sc_type_var) | sc_type_const);  // copied from asConst for maximum perfomance
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
   * edge type, or const by var and etc.
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

    auto const CheckMask = [&extType](RealType const & mask) {
      return extType.m_realType == (extType.m_realType & mask);
    };

    if (IsLink())
    {
      if (!CheckMask(sc_type_constancy_mask | sc_type_link))
        return false;
    }
    else if (IsNode())
    {
      if (!CheckMask(sc_type_constancy_mask | sc_type_node_mask | sc_type_node))
        return false;

      RealType const selfNodeType = m_realType & sc_type_node_mask;
      RealType const extNodeType = extType.m_realType & sc_type_node_mask;
      if (selfNodeType != 0 && selfNodeType != extNodeType)
        return false;
    }
    else if (IsEdge())
    {
      if (!CheckMask(sc_type_arc_common | sc_type_constancy_mask))
        return false;

      if (!CheckMask(sc_type_arc_access | sc_type_constancy_mask | sc_type_positivity_mask | sc_type_permanency_mask))
        return false;

      if (!CheckMask(sc_type_edge_common))
        return false;

      RealType const selfPermType = m_realType & sc_type_permanency_mask;
      RealType const extPermType = extType.m_realType & sc_type_permanency_mask;
      if (selfPermType != 0 && selfPermType != extPermType)
        return false;

      RealType const selfPosType = m_realType & sc_type_positivity_mask;
      RealType const extPosType = extType.m_realType & sc_type_positivity_mask;
      if (selfPosType != 0 && selfPosType != extPosType)
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
