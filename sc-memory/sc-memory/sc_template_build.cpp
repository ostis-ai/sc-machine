/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"
#include "sc_debug.hpp"

#include <algorithm>
#include <iostream>
#include <utility>

namespace
{
class ObjectInfo
{
public:
  ObjectInfo(ScAddr const & inAddr, ScType const & inType, std::string inSysIdtf)
    : m_addr(inAddr)
    , m_type(inType)
    , m_sysIdtf(std::move(inSysIdtf))
    , m_srcHash(0)
    , m_trgHash(0)
  {
  }

  inline bool IsEdge() const
  {
    return m_type.IsEdge();
  }

  inline bool IsUnknown() const
  {
    return m_type.IsUnknown();
  }

  inline ScAddr const & GetAddr() const
  {
    return m_addr;
  }

  inline std::string const & GetIdtf() const
  {
    return m_sysIdtf;
  }

  inline ScType const & GetType() const
  {
    return m_type;
  }

  inline ScAddr::HashType GetHash() const
  {
    return m_addr.Hash();
  }

  inline void SetSourceHash(ScAddr::HashType const & srcHash)
  {
    m_srcHash = srcHash;
  }

  inline void SetTargetHash(ScAddr::HashType const & trgHash)
  {
    m_trgHash = trgHash;
  }

  inline ScAddr::HashType const & GetSourceHash() const
  {
    return m_srcHash;
  }

  inline ScAddr::HashType const & GetTargetHash() const
  {
    return m_trgHash;
  }

private:
  ScAddr m_addr;
  ScType m_type;
  std::string m_sysIdtf;

  ScAddr::HashType m_srcHash;
  ScAddr::HashType m_trgHash;
};

}  // namespace

class ScTemplateBuilder
{
  friend class ScTemplate;
  using EdgeDependenceMap = std::unordered_multimap<ScAddr::HashType, ScAddr::HashType>;
  using ObjectToIdtfMap = std::map<ScAddr::HashType, ObjectInfo>;
  using ScAddrHashVector = std::vector<ScAddr::HashType>;

protected:
  ScTemplateBuilder(ScAddr const & inScTemplateAddr, ScMemoryContext & inCtx, ScTemplateParams const & params)
    : m_templateAddr(inScTemplateAddr)
    , m_context(inCtx)
    , m_params(params)
  {
  }

  ScTemplate::Result operator()(ScTemplate * inTemplate)
  {
    // mark template to don't force order of triples
    inTemplate->m_isForceOrder = false;

    // TODO: add struct blocking
    ScAddrHashVector independentEdges;
    independentEdges.reserve(512);

    size_t index = 0;
    ScIterator3Ptr iter = m_context.Iterator3(m_templateAddr, *ScType::EdgeAccessConstPosPerm, *ScType());

    // define edges set and independent edges set
    while (iter->Next())
    {
      ScAddr const objAddr = iter->Get(2);

      ObjectInfo obj = CollectObjectInfo(objAddr, "..obj_" + std::to_string(index++));
      if (obj.IsUnknown())
        return ScTemplate::Result(false, "Can't determine type of ScElement");  // template corrupted

      ScAddr objSrc, objTrg;
      if (obj.IsEdge() && m_context.GetEdgeInfo(objAddr, objSrc, objTrg))
      {
        obj.SetSourceHash(objSrc.Hash());
        obj.SetTargetHash(objTrg.Hash());

        ScType const srcType = m_context.GetElementType(objSrc), trgType = m_context.GetElementType(objTrg);
        if (!srcType.IsEdge() && !trgType.IsEdge())
        {
          auto const & it = std::find(independentEdges.begin(), independentEdges.end(), obj.GetHash());
          if (it == independentEdges.end())
            independentEdges.emplace_back(obj.GetHash());
        }
        if (srcType.IsEdge())
          m_edgeDependenceMap.insert({obj.GetHash(), objSrc.Hash()});
        if (trgType.IsEdge())
          m_edgeDependenceMap.insert({obj.GetHash(), objTrg.Hash()});
      }

      m_elements.insert({obj.GetHash(), obj});
    }

    // split edges set by their power
    std::vector<ScAddrHashVector> powerDependentEdges;
    powerDependentEdges.emplace_back(independentEdges);
    SplitEdgesByDependencePower(powerDependentEdges);

    // make tuple of partitions set subsets
    ScAddrHashVector edges;
    edges.reserve(512);
    for (auto const & equalDependentEdges : powerDependentEdges)
      for (auto const & edge : equalDependentEdges)
        edges.emplace_back(edge);

    // build template
    for (auto const i : edges)
    {
      ObjectInfo const & edge = m_elements.at(i);
      SC_ASSERT(edge.GetType().IsVar(), ());

      ObjectInfo const src = ReplaceWithParam(&m_elements.at(edge.GetSourceHash()));
      ObjectInfo const trg = ReplaceWithParam(&m_elements.at(edge.GetTargetHash()));

      auto const & param = [&inTemplate](ObjectInfo const & obj) -> ScTemplateItemValue {
        return obj.GetType().IsConst()
                   ? obj.GetAddr() >> obj.GetIdtf()
                   : (inTemplate->HasReplacement(obj.GetIdtf()) ? obj.GetIdtf() : obj.GetType() >> obj.GetIdtf());
      };

      inTemplate->Triple(param(src), edge.GetType() >> edge.GetIdtf(), param(trg));
    }

    return ScTemplate::Result(true);
  }

protected:
  ScAddr m_templateAddr;
  ScMemoryContext & m_context;
  ScTemplateParams m_params;

  // all objects in template
  ObjectToIdtfMap m_elements;
  EdgeDependenceMap m_edgeDependenceMap;

private:
  ObjectInfo CollectObjectInfo(ScAddr const & objAddr, std::string const & repl_idtf = "") const
  {
    ScType const objType = m_context.GetElementType(objAddr);
    std::string objIdtf = m_context.HelperGetSystemIdtf(objAddr);

    if (!repl_idtf.empty() && objIdtf.empty())
      objIdtf = repl_idtf;

    return {objAddr, objType, objIdtf};
  }

  void SplitEdgesByDependencePower(std::vector<ScAddrHashVector> & powerDependentEdges)
  {
    size_t size = powerDependentEdges.size();

    for (auto const & hPair : m_edgeDependenceMap)
    {
      size_t power = GetEdgeDependencePower(hPair.first, hPair.second);
      if (power >= powerDependentEdges.size())
      {
        while (size <= power)
        {
          powerDependentEdges.emplace_back(ScAddrHashVector());
          size++;
        }
      }

      auto const & equalDependentEdges = powerDependentEdges.at(power);
      auto const & equalDependentEdgesIt =
          std::find(equalDependentEdges.begin(), equalDependentEdges.end(), hPair.first);
      if (equalDependentEdgesIt == equalDependentEdges.end())
        powerDependentEdges.at(power).emplace_back(hPair.first);
    }
  }

  ObjectInfo ReplaceWithParam(ObjectInfo const * templateItem) const
  {
    if (!templateItem->IsEdge())
    {
      ScAddr replacedAddr;
      if (m_params.Get(templateItem->GetIdtf(), replacedAddr))
        return CollectObjectInfo(replacedAddr);
    }
    return *templateItem;
  }

  // get edge dependence power from other edge
  inline size_t GetEdgeDependencePower(ScAddr::HashType const & edge, ScAddr::HashType const & otherEdge) const
  {
    size_t max = 0;
    DefineEdgeDependencePower(edge, otherEdge, max, 1);
    return max;
  }

  // count edge dependence power from other edge
  inline size_t DefineEdgeDependencePower(
      ScAddr::HashType const & edge,
      ScAddr::HashType const & otherEdge,
      size_t & max,
      size_t power) const
  {
    auto const range = m_edgeDependenceMap.equal_range(edge);
    for (auto it = range.first; it != range.second; ++it)
    {
      size_t incPower = power + 1;

      if (DefineEdgeDependencePower(it->second, otherEdge, max, incPower) == incPower && power > max)
        max = power;
    }
    return power;
  }
};

ScTemplate::Result ScTemplate::FromScTemplate(
    ScMemoryContext & ctx,
    ScAddr const & scTemplateAddr,
    const ScTemplateParams & params)
{
  ScTemplateBuilder builder(scTemplateAddr, ctx, params);
  return builder(this);
}
