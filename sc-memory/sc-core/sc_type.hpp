#pragma once

#include "sc_defines.hpp"

#include <cstdint>

#if SC_IS_PLATFORM_WIN32
#define NOMINMAX
#endif

class SC_API ScType
{
public:
  using ValueType = uint16_t;

  ScType() = default;

  explicit ScType(ValueType type);

  bool IsEdge() const;
  bool IsNode() const;
  bool IsLink() const;
  bool IsConst() const;
  bool IsVar() const;

  SC_DEPRECATED(0.4.0, "Use !IsUnknown() instead")
  inline bool IsValid() const { return !IsUnknown(); }

  bool IsUnknown() const;

  bool HasConstancyFlag() const;

  // Returns copy of this type, but with variable raplaced to const
  ScType AsConst() const;

  // Returns copy of this type, but replace constancy type upward (metavar -> var -> const)
  ScType UpConstType() const;

  bool operator == (ScType const & other) const;
  bool operator != (ScType const & other) const;

  ScType operator | (ScType const & other) const;
  ScType operator & (ScType const & other) const;

  ScType & operator |= (ScType const & other);
  ScType & operator &= (ScType const & other);

  /* Check if specified type can be extended by another one to be a valid type/
   * For example this function returns false, if you try to extend node by
   * edge type, or const by var and etc.
   */
  bool CanExtendTo(ScType const & extType) const;

private:
  ValueType m_value = 0;

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
