/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

extern "C"
{
#include "sc-memory/sc_memory.h"
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
#include <assert.h>
#include <stdint.h>

#include <fstream>

#include "sc_defines.hpp"

typedef std::set<std::string> tStringSet;
typedef std::vector<std::string> tStringVector;
typedef std::map<std::string, std::string> tStringMap;

class ScMemoryContext;
class ScAddr;

class _SC_EXTERN ScType
{
public:
  using RealType = sc_type;

  explicit ScType() : m_realType(0)
  {
  }

  explicit ScType(RealType type)
    : m_realType(type)
  {
  }

  inline bool IsEdge() const { return (m_realType & sc_type_arc_mask) != 0; }
  inline bool IsNode() const { return (m_realType & sc_type_node) != 0; }
  inline bool IsLink() const { return (m_realType & sc_type_link) != 0; }
  inline bool IsConst() const { return (m_realType & sc_type_const) != 0; }
  inline bool IsVar() const { return (m_realType & sc_type_var) != 0; }
  inline bool IsValid() const { return (m_realType != 0); }

  // Returns copy of this type, but with variable raplaced to const
  inline ScType AsConst() const { return ScType((m_realType & ~sc_type_var) | sc_type_const); }

  // Returns copy of this type, but replace constancy type upward (metavar -> var -> const)
  inline ScType UpConstType() const
  {
    /// TODO: metavar
    //if (isVar())
    return ScType((m_realType & ~sc_type_var) | sc_type_const); // copied from asConst for maximum perfomance
  }

  inline sc_type operator * () const { return m_realType; }

  ScType & operator() (RealType bits)
  {
    m_realType |= bits;
    return *this;
  }

  inline bool operator == (ScType const & other) { return (m_realType == other.m_realType); }
  inline RealType BitAnd(RealType const & inMask) const { return (m_realType & inMask); }

  operator RealType () const { return m_realType; }

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
