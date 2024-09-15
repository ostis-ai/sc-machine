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

  explicit ScType();
  ScType(RealType type) noexcept;
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
