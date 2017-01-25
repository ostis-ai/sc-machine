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
  typedef sc_type tRealType;

  explicit ScType() : mRealType(0)
  {
  }

  explicit ScType(tRealType type)
    : mRealType(type)
  {
  }

  inline bool isEdge() const
  {
    return (mRealType & sc_type_arc_mask) != 0;
  }

  inline bool isNode() const
  {
    return (mRealType & sc_type_node) != 0;
  }

  inline bool isLink() const
  {
    return (mRealType & sc_type_link) != 0;
  }

  inline bool isConst() const
  {
    return (mRealType & sc_type_const) != 0;
  }

  inline bool isVar() const
  {
    return (mRealType & sc_type_var) != 0;
  }

  inline bool isValid() const
  {
    return (mRealType != 0);
  }

  // Returns copy of this type, but with variable raplaced to const
  inline ScType asConst() const
  {
    return ScType((mRealType & ~sc_type_var) | sc_type_const);
  }

  // Returns copy of this type, but replace constancy type upward (metavar -> var -> const)
  inline ScType upConstType() const
  {
    /// TODO: metavar
    //if (isVar())
    return ScType((mRealType & ~sc_type_var) | sc_type_const); // copied from asConst for maximum perfomance
  }

  inline sc_type operator * () const
  {
    return mRealType;
  }

  ScType & operator() (tRealType bits)
  {
    mRealType |= bits;
    return *this;
  }

  bool operator == (ScType const & other)
  {
    return (mRealType == other.mRealType);
  }

  inline tRealType bitAnd(tRealType const & inMask) const
  {
    return (mRealType & inMask);
  }

  operator tRealType () const
  {
    return mRealType;
  }

private:
  tRealType mRealType;

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
