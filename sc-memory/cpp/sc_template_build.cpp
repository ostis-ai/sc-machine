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

namespace
{

class ObjectInfo
{
public:
  ObjectInfo(ScAddr const & inAddr, ScType const & inType, std::string const & inSysIdtf)
    : m_addr(inAddr)
    , m_type(inType)
    , m_sysIdtf(inSysIdtf)
    , m_source(nullptr)
    , m_target(nullptr)
  {
  }

  inline bool IsEdge() const
  {
    return m_type.IsEdge();
  }

  inline ObjectInfo const * GetSource() const
  {
    return m_source;
  }

  inline ObjectInfo const * GetTarget() const
  {
    return m_target;
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

  inline void SetSource(ObjectInfo * src)
  {
    m_source = src;
  }

  inline void SetTarget(ObjectInfo * trg)
  {
    m_target = trg;
  }

  inline uint32_t CalculateEdgeRank() const
  {
    SC_ASSERT(IsEdge(), ());
    SC_ASSERT(m_source, ());
    SC_ASSERT(m_target, ());
    return (m_source->GetType().IsConst() ? 1 : 0) +
           (m_target->GetType().IsConst() ? 1 : 0);
  }

private:
  ScAddr m_addr;
  ScType m_type;
  std::string m_sysIdtf;

  // edge data
  ObjectInfo * m_source;
  ObjectInfo * m_target;
};

class EdgeLessFunctor
{
  using EdgeDependMap = std::unordered_multimap<ScAddr::HashType, ScAddr::HashType>;
  using ObjectVector = std::vector<ObjectInfo>;

public:
  explicit EdgeLessFunctor(EdgeDependMap const & edgeDependMap, ObjectVector const & objects)
    : m_edgeDependMap(edgeDependMap)
    , m_objects(objects)
  {
  }

  // returns true, when edge srouce/target objects depend on otherEdge construction
  bool IsEdgeDependOnOther(ScAddr::HashType const & edge, ScAddr::HashType const & otherEdge) const
  {
    // TODO: support possible loops
    auto const range = m_edgeDependMap.equal_range(edge);
    for (auto it = range.first; it != range.second; ++it)
    {
      if (it->second || IsEdgeDependOnOther(it->second, otherEdge))
        return true;
    }

    return false;
  }

  bool operator() (size_t const indexA, size_t const indexB) const
  {
    ObjectInfo const & objA = m_objects[indexA];
    ObjectInfo const & objB = m_objects[indexB];

    SC_ASSERT(objA.IsEdge(), ());
    SC_ASSERT(objB.IsEdge(), ());

    if (IsEdgeDependOnOther(objA.GetAddr().Hash(), objB.GetAddr().Hash()))
      return false;
    else if (IsEdgeDependOnOther(objB.GetAddr().Hash(), objA.GetAddr().Hash()))
      return true;

    uint32_t const rankA = objA.CalculateEdgeRank();
    uint32_t const rankB = objB.CalculateEdgeRank();

    if (rankA != rankB)
      return rankA > rankB;

    return objA.GetAddr().Hash() < objB.GetAddr().Hash();
  }

private:
  EdgeDependMap const & m_edgeDependMap;
  ObjectVector const & m_objects;
};

} // namespace

class ScTemplateBuilder
{
  friend class ScTemplate;

protected:
  ScTemplateBuilder(
    ScAddr const & inScTemplateAddr,
    ScMemoryContext & inCtx,
    const ScTemplateParams & params)
    : m_templateAddr(inScTemplateAddr)
    , m_context(inCtx)
    , m_params(params)
  {
  }

  bool operator()(ScTemplate * inTemplate)
  {
    // mark template to don't force order of triples
    inTemplate->m_isForceOrder = false;

    // TODO: add struct blocking
    // TODO: provide error codes
    std::unordered_map<ScAddr::HashType, size_t> addrToObjectIndex;
    std::unordered_multimap<ScAddr::HashType, ScAddr::HashType> edgeDependMap;
    std::vector<size_t> edgeIndices;

    m_objects.reserve(1024);
    edgeIndices.reserve(512);

    size_t index = 0;

    ScIterator3Ptr iter = m_context.Iterator3(
          m_templateAddr,
          *ScType::EdgeAccessConstPosPerm,
          *ScType());

    while (iter->Next())
    {
      ScAddr const templateItem = iter->Get(2);

      ScAddr::HashType const objHash = templateItem.Hash();
      auto const it = addrToObjectIndex.find(objHash);
      if (it != addrToObjectIndex.end())
        continue; // object already exist
      addrToObjectIndex[objHash] = m_objects.size();

      ScType const templateItemType = m_context.GetElementType(templateItem);
      if (templateItemType.IsUnknown())
        return false; // template corrupted

      std::string templateItemIdtf = m_context.HelperGetSystemIdtf(templateItem);
      if (templateItemIdtf.empty())
        templateItemIdtf = "..obj_" + std::to_string(index++);

      if (templateItemType.IsEdge())
        edgeIndices.emplace_back(m_objects.size());

      m_objects.emplace_back(templateItem, templateItemType, templateItemIdtf);
    }

    // iterate all edges and determine source/target objects
    for (auto const i : edgeIndices)
    {
      ObjectInfo & obj = m_objects[i];

      // determine source and target objects
      ScAddr src, trg;
      if (!m_context.GetEdgeInfo(obj.GetAddr(), src, trg))
        return false; // edge already doesn't exist

      auto const itSrc = addrToObjectIndex.find(src.Hash());
      if (itSrc == addrToObjectIndex.end())
        return false; // edge source doesn't exist in template

      auto const itTrg = addrToObjectIndex.find(trg.Hash());
      if (itTrg == addrToObjectIndex.end())
        return false; // target source doesn't exist in template

      ObjectInfo * srcObj = &(m_objects[itSrc->second]);
      ObjectInfo * trgObj = &(m_objects[itTrg->second]);

      if (srcObj->IsEdge())
        edgeDependMap.insert({ obj.GetAddr().Hash(), srcObj->GetAddr().Hash() });
      if (trgObj->IsEdge())
        edgeDependMap.insert({ obj.GetAddr().Hash(), srcObj->GetAddr().Hash() });

      obj.SetSource(srcObj);
      obj.SetTarget(trgObj);
    }

    // now need to sort edges for suitable search order
    std::sort(edgeIndices.begin(), edgeIndices.end(), EdgeLessFunctor(edgeDependMap, m_objects));

    // build template
    {
      for (auto const i : edgeIndices)
      {
        ObjectInfo const & edge = m_objects[i];
        SC_ASSERT(edge.GetType().IsVar(), ());

        const ObjectInfo * src = replaceWithParam(edge.GetSource());
        const ObjectInfo * trg = replaceWithParam(edge.GetTarget());

        ScType const srcType = src->GetType();
        ScType const trgType = trg->GetType();

        if (srcType.IsConst())
        {
          if (trgType.IsConst())  // F_A_F
          {
            inTemplate->Triple(
                  src->GetAddr() >> src->GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trg->GetAddr() >> trg->GetIdtf());
          }
          else
          {
            if (inTemplate->HasReplacement(trg->GetIdtf())) // F_A_F
            {
              inTemplate->Triple(
                    src->GetAddr() >> src->GetIdtf(),
                    edge.GetType() >> edge.GetIdtf(),
                    trg->GetIdtf());
            }
            else // F_A_A
            {
              inTemplate->Triple(
                    src->GetAddr() >> src->GetIdtf(),
                    edge.GetType() >> edge.GetIdtf(),
                    trgType >> trg->GetIdtf());
            }
          }
        }
        else if (trgType.IsConst())
        {
          if (inTemplate->HasReplacement(src->GetIdtf())) // F_A_F
          {
            inTemplate->Triple(
                  src->GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trg->GetAddr() >> trg->GetIdtf());
          }
          else // A_A_F
          {
            inTemplate->Triple(
                  srcType >> src->GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trg->GetAddr() >> trg->GetIdtf());
          }
        }
        else
        {
          bool const srcRepl = inTemplate->HasReplacement(src->GetIdtf());
          bool const trgRepl = inTemplate->HasReplacement(trg->GetIdtf());

          if (srcRepl && trgRepl) // F_A_F
          {
            inTemplate->Triple(
                  src->GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trg->GetIdtf());
          }
          else if (!srcRepl && trgRepl) // A_A_F
          {
            inTemplate->Triple(
                  srcType >> src->GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trg->GetIdtf());
          }
          else if (srcRepl && !trgRepl) // F_A_A
          {
            inTemplate->Triple(
                  src->GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trgType >> trg->GetIdtf());
          }
          else
          {
            inTemplate->Triple(
                  srcType >> src->GetIdtf(),
                  edge.GetType() >> edge.GetIdtf(),
                  trgType >> trg->GetIdtf());
          }
        }
      }
    }

    return true;
  }

protected:
  ScAddr m_templateAddr;
  ScMemoryContext & m_context;
  const ScTemplateParams & m_params;

  // all objects in template
  std::vector<ObjectInfo> m_objects;

private:
  const ObjectInfo * replaceWithParam(const ObjectInfo * templateItem)
  {
    if (!templateItem->IsEdge())
    {
      ScAddr replacedAddr;
      if (m_params.Get(templateItem->GetIdtf(), replacedAddr))
      {
        ScType type = m_context.GetElementType(replacedAddr);
        std::string idtf = m_context.HelperGetSystemIdtf(replacedAddr);
        return new ObjectInfo(replacedAddr, type, idtf);
      }
    }
    return templateItem;
  }
};

bool ScTemplate::FromScTemplate(ScMemoryContext & ctx, ScAddr const & scTemplateAddr)
{
  ScTemplateBuilder builder(scTemplateAddr, ctx, ScTemplateParams());
  return builder(this);
}

bool ScTemplate::FromScTemplate(ScMemoryContext & ctx, ScAddr const & scTemplateAddr, const ScTemplateParams & params)
{
  ScTemplateBuilder builder(scTemplateAddr, ctx, params);
  return builder(this);
}
