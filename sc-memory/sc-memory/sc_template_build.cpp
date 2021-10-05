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
  {
  }

  inline bool IsEdge() const
  {
    return m_type.IsEdge();
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

private:
  ScAddr m_addr;
  ScType m_type;
  std::string m_sysIdtf;
};

}

class ScTemplateBuilder
{
  friend class ScTemplate;
  using EdgeDependMap = std::unordered_multimap<ScAddr::HashType, ScAddr::HashType>;
  using ObjectToIdtfMap = std::map<ScAddr::HashType, ObjectInfo>;
  using ScAddrHashVector = std::vector<ScAddr::HashType>;

protected:
  ScTemplateBuilder(
    ScAddr const & inScTemplateAddr,
    ScMemoryContext & inCtx,
    ScTemplateParams const & params)
    : m_templateAddr(inScTemplateAddr)
    , m_context(inCtx)
    , m_params(params)
  {
  }

  ScTemplate::Result operator() (ScTemplate * inTemplate)
  {
    // mark template to don't force order of triples
    inTemplate->m_isForceOrder = false;

    // TODO: add struct blocking
    ScAddrHashVector independentEdges;
    independentEdges.reserve(512);

    size_t index = 0;
    ScIterator3Ptr iter = m_context.Iterator3(
          m_templateAddr,
          *ScType::EdgeAccessConstPosPerm,
          *ScType());

    // define edges set and independent edges set
    while (iter->Next())
    {
      ScAddr const objAddr = iter->Get(2);
      std::string objIdtf = m_context.HelperGetSystemIdtf(objAddr);

      if (objIdtf.empty())
      {
        objIdtf = "..obj_" + std::to_string(index++);
      }

      ScAddr::HashType const objHash = objAddr.Hash();
      ScType const objType = m_context.GetElementType(objAddr);

      if (objType.IsUnknown())
      {
        return ScTemplate::Result(false, "Can't determine type of ScElement"); // template corrupted
      }

      m_elements.insert({objHash, ObjectInfo(objAddr, objType, objIdtf)});

      ScAddr objSrc, objTrg;
      if (objType.IsEdge() && m_context.GetEdgeInfo(objAddr, objSrc, objTrg))
      {
        ScType const srcType = m_context.GetElementType(objSrc),
              trgType = m_context.GetElementType(objTrg);

        if (!srcType.IsEdge() && !trgType.IsEdge())
        {
          auto const & it = std::find(independentEdges.begin(), independentEdges.end(), objHash);
          if (it == independentEdges.end())
          {
            independentEdges.emplace_back(objHash);
          }
        }
        if (srcType.IsEdge())
        {
          m_edgeDependenceMap.insert({objHash, objSrc.Hash()});
        }
        if (trgType.IsEdge())
        {
          m_edgeDependenceMap.insert({objHash, objTrg.Hash()});
        }
      }
    }

    // split edges set by their power
    std::vector<ScAddrHashVector> powerDependentEdges;
    powerDependentEdges.emplace_back(independentEdges);
    size_t size = powerDependentEdges.size();

    for (auto const & hPair : m_edgeDependenceMap)
    {
      index = GetEdgeDependencePower(hPair.first, hPair.second);
      if (index >= powerDependentEdges.size())
      {
        while (size <= index)
        {
          powerDependentEdges.emplace_back(ScAddrHashVector());
          size++;
        }
      }

      auto const & equalDependentEdges = powerDependentEdges.at(index);
      auto const & equalDependentEdgesIt = std::find(equalDependentEdges.begin(), equalDependentEdges.end(), hPair.first);
      if (equalDependentEdgesIt == equalDependentEdges.end())
      {
        powerDependentEdges.at(index).emplace_back(hPair.first);
      }
    }

    // make tuple of partitions set subsets
    ScAddrHashVector edges;
    edges.reserve(512);
    for (auto const & equalDependentEdges : powerDependentEdges)
    {
      for (auto const & edge : equalDependentEdges)
      {
        edges.emplace_back(edge);
      }
    }

    // form template
    for (auto const i : edges)
    {
      ObjectInfo const & edge = m_elements.at(i);
      SC_ASSERT(edge.GetType().IsVar(), ());

      ScAddr srcAddr, trgAddr;
      m_context.GetEdgeInfo(edge.GetAddr(), srcAddr, trgAddr);
      ObjectInfo const src = replaceWithParam(&m_elements.at(srcAddr.Hash()));
      ObjectInfo const trg = replaceWithParam(&m_elements.at(trgAddr.Hash()));

      ScType const srcType = src.GetType();
      ScType const trgType = trg.GetType();

      if (srcType.IsConst())
      {
        if (trgType.IsConst())  // F_A_F
        {
          inTemplate->Triple(
                src.GetAddr() >> src.GetIdtf(),
                edge.GetType() >> edge.GetIdtf(),
                trg.GetAddr() >> trg.GetIdtf());
        }
        else
        {
          if (inTemplate->HasReplacement(trg.GetIdtf())) // F_A_F
          {
            inTemplate->Triple(
                  src.GetAddr() >> src.GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trg.GetIdtf());
          }
          else // F_A_A
          {
            inTemplate->Triple(
                  src.GetAddr() >> src.GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trgType >> trg.GetIdtf());
          }
        }
      }
      else if (trgType.IsConst())
      {
        if (inTemplate->HasReplacement(src.GetIdtf())) // F_A_F
        {
          inTemplate->Triple(
                src.GetIdtf(),
                edge.GetType() >> edge.GetIdtf(),
                trg.GetAddr() >> trg.GetIdtf());
        }
        else // A_A_F
        {
          inTemplate->Triple(
                srcType >> src.GetIdtf(),
                edge.GetType() >> edge.GetIdtf(),
                trg.GetAddr() >> trg.GetIdtf());
        }
      }
      else
      {
        bool const srcRepl = inTemplate->HasReplacement(src.GetIdtf());
        bool const trgRepl = inTemplate->HasReplacement(trg.GetIdtf());

        if (srcRepl && trgRepl) // F_A_F
        {
          inTemplate->Triple(
                src.GetIdtf(),
                edge.GetType() >> edge.GetIdtf(),
                trg.GetIdtf());
        }
        else if (!srcRepl && trgRepl) // A_A_F
        {
          inTemplate->Triple(
                srcType >> src.GetIdtf(),
                edge.GetType() >> edge.GetIdtf(),
                trg.GetIdtf());
        }
        else if (srcRepl && !trgRepl) // F_A_A
        {
          inTemplate->Triple(
                src.GetIdtf(),
                edge.GetType() >> edge.GetIdtf(),
                trgType >> trg.GetIdtf());
        }
        else
        {
          inTemplate->Triple(
                srcType >> src.GetIdtf(),
                edge.GetType() >> edge.GetIdtf(),
                trgType >> trg.GetIdtf());
        }
      }
    }

    return ScTemplate::Result(true);
  }

protected:
  ScAddr m_templateAddr;
  ScMemoryContext & m_context;
  ScTemplateParams m_params;

  // all objects in template
  ObjectToIdtfMap m_elements;
  EdgeDependMap m_edgeDependenceMap;

private:
  ObjectInfo replaceWithParam(ObjectInfo const * templateItem) const
  {
    if (!templateItem->IsEdge())
    {
      ScAddr replacedAddr;
      if (m_params.Get(templateItem->GetIdtf(), replacedAddr))
      {
        ScType const type = m_context.GetElementType(replacedAddr);
        std::string const idtf = m_context.HelperGetSystemIdtf(replacedAddr);
        return {replacedAddr, type, idtf};
      }
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
  inline size_t DefineEdgeDependencePower(ScAddr::HashType const & edge, ScAddr::HashType const & otherEdge,
                                          size_t & max, size_t power) const
  {
    auto const range = m_edgeDependenceMap.equal_range(edge);
    for (auto it = range.first; it != range.second; ++it)
    {
      size_t incPower = power + 1;

      if (DefineEdgeDependencePower(it->second, otherEdge, max, incPower) == incPower && power > max)
      {
        max = power;
      }
    }
    return power;
  }
};

ScTemplate::Result ScTemplate::FromScTemplate(ScMemoryContext & ctx, ScAddr const & scTemplateAddr, const ScTemplateParams & params)
{
  ScTemplateBuilder builder(scTemplateAddr, ctx, params);
  return builder(this);
}
