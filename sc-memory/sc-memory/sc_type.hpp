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

  ScType(RealType type) noexcept
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
  inline bool IsMetaVar() const
  {
    return (m_realType & sc_type_metavar) != 0;
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

  // Returns copy of this type, but with variable replaced to const
  inline ScType AsConst() const
  {
    return ScType((m_realType & ~sc_type_var) | sc_type_const);
  }

  // Returns copy of this type, but with variable replaced to const
  inline ScType AsVar() const
  {
    return ScType((m_realType & ~sc_type_metavar) | sc_type_const);
  }

  // Returns copy of this type, but replace constancy type upward (metavar -> var -> const)
  inline ScType UpConstType() const
  {
    if (IsVar())
      return ScType((m_realType & ~sc_type_var) | sc_type_const);  // copied from asConst for maximum performance
    else if (IsMetaVar())
      return ScType((m_realType & ~sc_type_metavar) | sc_type_var);  // copied from asVar for maximum performance
    else
      return ScType(m_realType | sc_type_const);
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

    if (IsNode())
    {
      if (!CheckMask(sc_type_node | sc_type_constancy_mask | sc_type_permanency_mask | sc_type_node_mask))
        return false;

      RealType const selfNodeType = m_realType & sc_type_node_mask;
      RealType const extNodeType = extType.m_realType & sc_type_node_mask;
      if (selfNodeType != 0 && selfNodeType != extNodeType)
        return false;
    }
    else if (IsEdge())
    {
      if (!CheckMask(sc_type_arc_common | sc_type_edge_common | sc_type_constancy_mask))
        return false;

      if (!CheckMask(sc_type_arc_access | sc_type_constancy_mask | sc_type_positivity_mask | sc_type_permanency_mask))
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
  static ScType const Unknown;

  static ScType const Const;
  static ScType const Var;
  static ScType const MetaVar;

  static ScType const Perm;
  static ScType const Temp;

  static ScType const Node;

  static ScType const NodeConst;
  static ScType const NodeVar;

  static ScType const NodePerm;
  static ScType const NodeTemp;

  static ScType const NodeStruct;
  static ScType const NodeTuple;
  static ScType const NodeRole;
  static ScType const NodeNoRole;
  static ScType const NodeClass;
  static ScType const NodeSuperClass;
  static ScType const NodeAbstract;
  static ScType const NodeMaterial;
  static ScType const Link;
  static ScType const LinkClass;

  static ScType const NodeConstStruct;
  static ScType const NodeConstTuple;
  static ScType const NodeConstRole;
  static ScType const NodeConstNoRole;
  static ScType const NodeConstClass;
  static ScType const NodeConstSuperClass;
  static ScType const NodeConstAbstract;
  static ScType const NodeConstMaterial;
  static ScType const LinkConst;
  static ScType const LinkConstClass;

  static ScType const NodeVarStruct;
  static ScType const NodeVarTuple;
  static ScType const NodeVarRole;
  static ScType const NodeVarNoRole;
  static ScType const NodeVarClass;
  static ScType const NodeVarSuperClass;
  static ScType const NodeVarAbstract;
  static ScType const NodeVarMaterial;
  static ScType const LinkVar;
  static ScType const LinkVarClass;

  static ScType const NodeMetaVarStruct;
  static ScType const NodeMetaVarTuple;
  static ScType const NodeMetaVarRole;
  static ScType const NodeMetaVarNoRole;
  static ScType const NodeMetaVarClass;
  static ScType const NodeMetaVarSuperClass;
  static ScType const NodeMetaVarAbstract;
  static ScType const NodeMetaVarMaterial;
  static ScType const LinkMetaVar;
  static ScType const LinkMetaVarClass;

  static ScType const NodePermStruct;
  static ScType const NodePermTuple;
  static ScType const NodePermRole;
  static ScType const NodePermNoRole;
  static ScType const NodePermClass;
  static ScType const NodePermSuperClass;
  static ScType const NodePermAbstract;
  static ScType const NodePermMaterial;
  static ScType const LinkPermConst;
  static ScType const LinkPermConstClass;

  static ScType const NodeTempStruct;
  static ScType const NodeTempTuple;
  static ScType const NodeTempRole;
  static ScType const NodeTempNoRole;
  static ScType const NodeTempClass;
  static ScType const NodeTempSuperClass;
  static ScType const NodeTempAbstract;
  static ScType const NodeTempMaterial;
  static ScType const LinkTempConst;
  static ScType const LinkTempConstClass;

  static ScType const NodeConstPermStruct;
  static ScType const NodeConstPermTuple;
  static ScType const NodeConstPermRole;
  static ScType const NodeConstPermNoRole;
  static ScType const NodeConstPermClass;
  static ScType const NodeConstPermSuperClass;
  static ScType const NodeConstPermAbstract;
  static ScType const NodeConstPermMaterial;
  static ScType const LinkConstPerm;
  static ScType const LinkConstPermClass;

  static ScType const NodeVarPermStruct;
  static ScType const NodeVarPermTuple;
  static ScType const NodeVarPermRole;
  static ScType const NodeVarPermNoRole;
  static ScType const NodeVarPermClass;
  static ScType const NodeVarPermSuperClass;
  static ScType const NodeVarPermAbstract;
  static ScType const NodeVarPermMaterial;
  static ScType const LinkVarPerm;
  static ScType const LinkVarPermClass;

  static ScType const NodeMetaVarPermStruct;
  static ScType const NodeMetaVarPermTuple;
  static ScType const NodeMetaVarPermRole;
  static ScType const NodeMetaVarPermNoRole;
  static ScType const NodeMetaVarPermClass;
  static ScType const NodeMetaVarPermSuperClass;
  static ScType const NodeMetaVarPermAbstract;
  static ScType const NodeMetaVarPermMaterial;
  static ScType const LinkMetaVarPerm;
  static ScType const LinkMetaVarPermClass;

  static ScType const NodeConstTempStruct;
  static ScType const NodeConstTempTuple;
  static ScType const NodeConstTempRole;
  static ScType const NodeConstTempNoRole;
  static ScType const NodeConstTempClass;
  static ScType const NodeConstTempSuperClass;
  static ScType const NodeConstTempAbstract;
  static ScType const NodeConstTempMaterial;
  static ScType const LinkConstTemp;
  static ScType const LinkConstTempClass;

  static ScType const NodeVarTempStruct;
  static ScType const NodeVarTempTuple;
  static ScType const NodeVarTempRole;
  static ScType const NodeVarTempNoRole;
  static ScType const NodeVarTempClass;
  static ScType const NodeVarTempSuperClass;
  static ScType const NodeVarTempAbstract;
  static ScType const NodeVarTempMaterial;
  static ScType const LinkVarTemp;
  static ScType const LinkVarTempClass;

  static ScType const NodeMetaVarTempStruct;
  static ScType const NodeMetaVarTempTuple;
  static ScType const NodeMetaVarTempRole;
  static ScType const NodeMetaVarTempNoRole;
  static ScType const NodeMetaVarTempClass;
  static ScType const NodeMetaVarTempSuperClass;
  static ScType const NodeMetaVarTempAbstract;
  static ScType const NodeMetaVarTempMaterial;
  static ScType const LinkMetaVarTemp;
  static ScType const LinkMetaVarTempClass;

  static ScType const Connector;
  static ScType const Arc;

  static ScType const EdgeCommon;
  static ScType const ArcCommon;
  static ScType const ArcAccess;

  // backward compatibility
  SC_DEPRECATED(0.8.0, "Use ScType::EdgeCommon instead")
  static ScType const EdgeUCommon;
  SC_DEPRECATED(0.8.0, "Use ScType::ArcCommon instead")
  static ScType const EdgeDCommon;
  static ScType const EdgeAccess;

  static ScType const EdgeCommonConst;
  static ScType const ArcCommonConst;
  static ScType const ArcAccessConst;

  // backward compatibility
  SC_DEPRECATED(0.8.0, "Use ScType::EdgeCommonConst instead")
  static ScType const EdgeUCommonConst;
  SC_DEPRECATED(0.8.0, "Use ScType::ArcCommonConst instead")
  static ScType const EdgeDCommonConst;

  static ScType const EdgeCommonVar;
  static ScType const ArcCommonVar;
  static ScType const ArcAccessVar;

  // backward compatibility
  SC_DEPRECATED(0.8.0, "Use ScType::EdgeCommonVar instead")
  static ScType const EdgeUCommonVar;
  SC_DEPRECATED(0.8.0, "Use ScType::ArcCommonVar instead")
  static ScType const EdgeDCommonVar;

  static ScType const EdgeCommonMetaVar;
  static ScType const ArcCommonMetaVar;
  static ScType const ArcAccessMetaVar;

  static ScType const EdgeCommonPerm;
  static ScType const ArcCommonPerm;
  static ScType const ArcAccessPerm;

  static ScType const EdgeCommonTemp;
  static ScType const ArcCommonTemp;
  static ScType const ArcAccessTemp;

  static ScType const EdgeCommonConstPerm;
  static ScType const ArcCommonConstPerm;

  static ScType const EdgeCommonVarPerm;
  static ScType const ArcCommonVarPerm;

  static ScType const EdgeCommonMetaVarPerm;
  static ScType const ArcCommonMetaVarPerm;

  static ScType const EdgeCommonConstTemp;
  static ScType const ArcCommonConstTemp;

  static ScType const EdgeCommonVarTemp;
  static ScType const ArcCommonVarTemp;

  static ScType const EdgeCommonMetaVarTemp;
  static ScType const ArcCommonMetaVarTemp;

  static ScType const EdgeAccessPos;
  static ScType const EdgeAccessNeg;
  static ScType const EdgeAccessFuz;

  static ScType const EdgeAccessConstPos;
  static ScType const EdgeAccessConstNeg;
  static ScType const EdgeAccessConstFuz;

  static ScType const EdgeAccessVarPos;
  static ScType const EdgeAccessVarNeg;
  static ScType const EdgeAccessVarFuz;

  static ScType const EdgeAccessMetaVarPos;
  static ScType const EdgeAccessMetaVarNeg;
  static ScType const EdgeAccessMetaVarFuz;

  static ScType const EdgeAccessConstPerm;
  static ScType const EdgeAccessConstTemp;

  static ScType const EdgeAccessVarPerm;
  static ScType const EdgeAccessVarTemp;

  static ScType const EdgeAccessMetaVarPerm;
  static ScType const EdgeAccessMetaVarTemp;

  static ScType const EdgeAccessPosPerm;
  static ScType const EdgeAccessNegPerm;
  static ScType const EdgeAccessFuzPerm;

  static ScType const EdgeAccessPosTemp;
  static ScType const EdgeAccessNegTemp;
  static ScType const EdgeAccessFuzTemp;

  static ScType const EdgeAccessConstPosPerm;
  static ScType const EdgeAccessConstNegPerm;
  static ScType const EdgeAccessConstFuzPerm;

  static ScType const EdgeAccessConstPosTemp;
  static ScType const EdgeAccessConstNegTemp;
  static ScType const EdgeAccessConstFuzTemp;

  static ScType const EdgeAccessVarPosPerm;
  static ScType const EdgeAccessVarNegPerm;
  static ScType const EdgeAccessVarFuzPerm;

  static ScType const EdgeAccessVarPosTemp;
  static ScType const EdgeAccessVarNegTemp;
  static ScType const EdgeAccessVarFuzTemp;

  static ScType const EdgeAccessMetaVarPosPerm;
  static ScType const EdgeAccessMetaVarNegPerm;
  static ScType const EdgeAccessMetaVarFuzPerm;

  static ScType const EdgeAccessMetaVarPosTemp;
  static ScType const EdgeAccessMetaVarNegTemp;
  static ScType const EdgeAccessMetaVarFuzTemp;
};
