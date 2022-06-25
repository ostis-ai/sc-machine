/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

#include "sc_debug.hpp"
#include "sc_memory.hpp"

#include <algorithm>
#include <stack>

class ScTemplateSearch
{
public:
  ScTemplateSearch(ScTemplate const & templ, ScMemoryContext & context, ScAddr const & scStruct)
    : m_template(templ)
    , m_context(context)
    , m_struct(scStruct)
  {
    UpdateSearchCache();
  }

  void UpdateSearchCache()
  {
    if (m_template.m_isForceOrder)
    {
      m_template.m_searchCachedOrder.resize(m_template.m_constructions.size());
      for (size_t i = 0; i < m_template.m_constructions.size(); ++i)
        m_template.m_searchCachedOrder[i] = i;
    }
    else if (!m_template.IsSearchCacheValid() && !m_template.m_constructions.empty())
    {
      // update it
      ScTemplate::ProcessOrder preCache(m_template.m_constructions.size());
      for (size_t i = 0; i < preCache.size(); ++i)
        preCache[i] = i;

      static const size_t kScoreEdge = 5;
      static const size_t kScoreOther = 1;

      /** First of all we need to calculate scores for all triples
       * (more scores - should be search first).
       * Also need to store all replacements that need to be resolved
       */
      std::vector<uint8_t> tripleScores(m_template.m_constructions.size());
      std::unordered_map<std::string, std::vector<size_t>> replDependMap;
      for (size_t i = 0; i < m_template.m_constructions.size(); ++i)
      {
        ScTemplateConstr3 const & triple = m_template.m_constructions[i];
        auto const CalculateScore = [](ScTemplateConstr3 const & constr) {
          uint8_t score = 0;
          auto const & values = constr.GetValues();
          if (values[1].IsAddr() && values[0].IsAssign() && values[2].IsAssign())
            score += kScoreEdge;
          else if (values[0].IsAddr() && values[1].IsAssign() && values[2].IsAddr())
            score += kScoreOther * 2;  // should be a sum of (f_a_a and a_a_f)
          else if (values[0].IsAddr() || values[2].IsAddr())
            score += kScoreOther;

          return score;
        };
        tripleScores[i] = CalculateScore(triple);
        // doesn't add edges into depend map
        auto const TryAppendRepl = [&](ScTemplateItemValue const & value, size_t idx) {
          SC_ASSERT(idx < 3, ());
          if (!value.IsAddr() && !value.m_replacementName.empty())
            replDependMap[value.m_replacementName].push_back((i << 2) + idx);
        };

        // do not use loop, to make it faster (not all compilators will make a linear code)
        TryAppendRepl(triple.GetValues()[0], 0);
        TryAppendRepl(triple.GetValues()[1], 1);
        TryAppendRepl(triple.GetValues()[2], 2);
      }

      // sort by scores
      std::sort(preCache.begin(), preCache.end(), [&](size_t a, size_t b) {
        return (tripleScores[a] > tripleScores[b]);
      });

      // now we need to append triples, in order, when previous resolve replacement for a next one
      ScTemplate::ProcessOrder & cache = m_template.m_searchCachedOrder;
      cache.resize(preCache.size());

      std::vector<bool> isTripleCached(cache.size(), false);
      std::unordered_set<std::string> usedReplacements;

      size_t preCacheIdx = 1;
      size_t orderIdx = 1;
      cache[0] = preCache[0];
      isTripleCached[cache[0]] = true;
      while (orderIdx < cache.size())
      {
        size_t const curTripleIdx = cache[orderIdx - 1];
        auto const & triple = m_template.m_constructions[curTripleIdx];

        // get resolved replacements by the last triple
        std::vector<std::string> resolvedReplacements;
        resolvedReplacements.reserve(3);
        for (auto const & value : triple.GetValues())
        {
          if (!value.m_replacementName.empty())
            resolvedReplacements.emplace_back(value.m_replacementName);
        }

        // collect triples, that depend on resolved replacements and update their scores
        std::unordered_set<size_t> resolvedTriples;
        for (auto const & name : resolvedReplacements)
        {
          if (usedReplacements.find(name) != usedReplacements.end())
            continue;

          auto const it = replDependMap.find(name);
          if (it != replDependMap.end())
            resolvedTriples.insert(it->second.begin(), it->second.end());

          usedReplacements.insert(name);
        }

        // find next non used triple
        while (isTripleCached[preCache[preCacheIdx]])
          preCacheIdx++;

        size_t bestTripleIdx = preCache[preCacheIdx];
        // now update scores of resolved triples and find best one scores
        for (size_t idx : resolvedTriples)
        {
          // unpack it
          size_t const tripleIdx = idx >> 2;

          if (isTripleCached[tripleIdx])
            continue;

          // for a constants see initial calculation
          if (idx == 1)
            tripleScores[tripleIdx] += kScoreEdge;
          else
            tripleScores[tripleIdx] += kScoreOther;

          // check if it better
          if (tripleScores[bestTripleIdx] < tripleScores[tripleIdx])
            bestTripleIdx = tripleIdx;
        }

        // append new best triple
        cache[orderIdx++] = bestTripleIdx;
        isTripleCached[bestTripleIdx] = true;
      }

      m_template.m_isSearchCacheValid = true;
    }
  }

  ScAddr const & ResolveAddr(ScTemplateItemValue const & value) const
  {
    switch (value.m_itemType)
    {
    case ScTemplateItemValue::Type::Addr:
      return value.m_addrValue;

    case ScTemplateItemValue::Type::Replace:
    {
      auto it = m_template.m_replacements.find(value.m_replacementName);
      SC_ASSERT(it != m_template.m_replacements.end(), ());
      SC_ASSERT(it->second < m_resultAddrs.size(), ());
      return m_resultAddrs[it->second];
    }

    case ScTemplateItemValue::Type::Type:
    {
      if (!value.m_replacementName.empty())
      {
        auto it = m_template.m_replacements.find(value.m_replacementName);
        SC_ASSERT(it != m_template.m_replacements.end(), ());
        SC_ASSERT(it->second < m_resultAddrs.size(), ());
        return m_resultAddrs[it->second];
      }
      break;
    }

    default:
      break;
    };

    static ScAddr empty;
    return empty;
  }

  ScIterator3Ptr CreateIterator(ScTemplateConstr3 const & constr)
  {
    auto const & values = constr.GetValues();

    ScTemplateItemValue const & value0 = values[0];
    ScTemplateItemValue const & value1 = values[1];
    ScTemplateItemValue const & value2 = values[2];

    ScAddr const addr0 = ResolveAddr(value0);
    ScAddr const addr1 = ResolveAddr(value1);
    ScAddr const addr2 = ResolveAddr(value2);

    auto const PrepareType = [](ScType const & type) {
      if (type.HasConstancyFlag())
        return type.UpConstType();

      return type;
    };

    if (addr0.IsValid())
    {
      if (!addr1.IsValid())
      {
        if (addr2.IsValid())  // F_A_F
        {
          return m_context.Iterator3(addr0, PrepareType(value1.m_typeValue), addr2);
        }
        else  // F_A_A
        {
          return m_context.Iterator3(addr0, PrepareType(value1.m_typeValue), PrepareType(value2.m_typeValue));
        }
      }
      else
      {
        if (addr2.IsValid())  // F_F_F
        {
          return m_context.Iterator3(addr0, addr1, addr2);
        }
        else  // F_F_A
        {
          return m_context.Iterator3(addr0, addr1, PrepareType(value2.m_typeValue));
        }
      }
    }
    else if (addr2.IsValid())
    {
      if (addr1.IsValid())  // A_F_F
      {
        return m_context.Iterator3(PrepareType(value0.m_typeValue), addr1, addr2);
      }
      else  // A_A_F
      {
        return m_context.Iterator3(PrepareType(value0.m_typeValue), PrepareType(value1.m_typeValue), addr2);
      }
    }
    else if (addr1.IsValid() && !addr2.IsValid())  // A_F_A
    {
      return m_context.Iterator3(PrepareType(value0.m_typeValue), addr1, PrepareType(value2.m_typeValue));
    }

    return {};
  }

  bool CheckInStruct(ScAddr const & addr)
  {
    StructCache::const_iterator it = m_structCache.find(addr);
    if (it != m_structCache.end())
      return true;

    if (m_context.HelperCheckEdge(m_struct, addr, ScType::EdgeAccessConstPosPerm))
    {
      m_structCache.insert(addr);
      return true;
    }

    return false;
  }

  void RefReplacement(ScTemplateItemValue const & v, ScAddr const & addr)
  {
    if (!v.m_replacementName.empty())
    {
      auto it = m_template.m_replacements.find(v.m_replacementName);
      SC_ASSERT(it != m_template.m_replacements.end(), ());
      if (addr.IsValid())
        m_resultAddrs[it->second] = addr;
      m_replRefs[it->second]++;
    }
  }

  void UnrefReplacement(ScTemplateItemValue const & v)
  {
    if (!v.m_replacementName.empty())
    {
      auto it = m_template.m_replacements.find(v.m_replacementName);
      SC_ASSERT(it != m_template.m_replacements.end(), ());

      m_replRefs[it->second]--;
      if (m_replRefs[it->second] == 0)
        m_resultAddrs[it->second].Reset();
    }
  }

  void DoIterations(ScTemplateSearchResult & result)
  {
    std::stack<ScIterator3Ptr> iterators;
    std::vector<bool> didIter(m_template.m_constructions.size(), false);
    std::vector<ScAddr> edges(m_template.m_constructions.size());
    size_t const finishIdx = m_template.m_constructions.size() - 1;
    bool newIteration = true;

    do
    {
      size_t const orderIndex = newIteration ? iterators.size() : iterators.size() - 1;
      size_t const constrIndex = m_template.m_searchCachedOrder[orderIndex];

      SC_ASSERT(constrIndex < m_template.m_constructions.size(), ());
      size_t const resultIdx = constrIndex * 3;

      ScTemplateConstr3 const & constr = m_template.m_constructions[constrIndex];
      auto const & values = constr.GetValues();

      ScIterator3Ptr it;
      if (newIteration)
      {
        it = CreateIterator(constr);
        iterators.push(it);
      }
      else
      {
        UnrefReplacement(values[0]);
        UnrefReplacement(values[1]);
        UnrefReplacement(values[2]);

        auto const itEdge = m_usedEdges.find(edges[orderIndex]);
        if (itEdge != m_usedEdges.end())
          m_usedEdges.erase(itEdge);

        it = iterators.top();
      }

      auto const applyResult = [&](ScAddr const & res1, ScAddr const & res2, ScAddr const & res3) {
        edges[orderIndex] = res2;

        // do not make cycle for optimization issues (remove comparison expression)
        m_resultAddrs[resultIdx] = res1;
        m_resultAddrs[resultIdx + 1] = res2;
        m_resultAddrs[resultIdx + 2] = res3;

        RefReplacement(values[0], res1);
        RefReplacement(values[1], res2);
        RefReplacement(values[2], res3);

        if (orderIndex == finishIdx)
        {
          result.m_results.push_back(m_resultAddrs);
          newIteration = false;
        }
        else
        {
          newIteration = true;
        }
      };

      // make one iteration
      if (it.get() && it->IsValid())
      {
        bool isFinished = true;

        while (it->Next())
        {
          ScAddr const addr1 = it->Get(0);
          ScAddr const addr2 = it->Get(1);
          ScAddr const addr3 = it->Get(2);

          // check if search in structure
          if (m_struct.IsValid())
          {
            if (!CheckInStruct(addr1) || !CheckInStruct(addr2) || !CheckInStruct(addr3))
            {
              continue;
            }
          }

          auto const res = m_usedEdges.insert(addr2);
          if (!res.second)  // don't iterate the same edge twice
            continue;

          applyResult(addr1, addr2, addr3);

          didIter[orderIndex] = true;
          isFinished = false;
          break;
        }

        if (isFinished)  // finish iterator
        {
          iterators.pop();
          newIteration = false;
        }
      }
      else  // special checks and search
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Invalid state during template search");
      }
    } while (!iterators.empty());
  }

  ScTemplate::Result operator()(ScTemplateSearchResult & result)
  {
    // if (!m_template.m_hasRequired)
    //  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Templates just with optional triples doesn't supported.");

    result.Clear();

    result.m_replacements = m_template.m_replacements;
    m_resultAddrs.resize(CalculateOneResultSize());
    m_replRefs.resize(m_resultAddrs.size(), 0);

    DoIterations(result);

    return ScTemplate::Result(result.Size() > 0);
  }

  size_t CalculateOneResultSize() const
  {
    return m_template.m_constructions.size() * 3;
  }

private:
  ScTemplate const & m_template;
  ScMemoryContext & m_context;
  ScAddr const m_struct;

  using StructCache = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;
  StructCache m_structCache;

  using UsedEdges = std::set<ScAddr, ScAddLessFunc>;
  UsedEdges m_usedEdges;

  ScAddrVector m_resultAddrs;
  using ReplRefs = std::vector<uint32_t>;
  ReplRefs m_replRefs;
};

ScTemplate::Result ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(*this, ctx, ScAddr());
  return search(result);
}

ScTemplate::Result ScTemplate::SearchInStruct(
    ScMemoryContext & ctx,
    ScAddr const & scStruct,
    ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(*this, ctx, scStruct);
  return search(result);
}
