/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

#include "sc_debug.hpp"
#include "sc_memory.hpp"

#include <algorithm>


class ScTemplateSearch
{
public:
  ScTemplateSearch(ScTemplate const & templ,
                   ScMemoryContext & context,
                   ScAddr const & scStruct)
    : m_template(templ)
    , m_context(context)
    , m_struct(scStruct)
  {
    UpdateSearchCache();
  }

  void UpdateSearchCache()
  {
    if (!m_template.IsSearchCacheValid())
    {
      // update it
      auto const & triples = m_template.m_constructions;
      ScTemplate::ProcessOrder & cache = m_template.m_searchCachedOrder;
      cache.resize(m_template.m_constructions.size());
      size_t i = 0;
      for (size_t & v : cache)
        v = i++;

      /* Main idea of this stage, to build an order where iterators with maximum number of
       * fixed elements will runs first. This allow to minimize number of iterations. Also we can't
       * start search from template a_a_a, or any template where all fixed elements are remplacements
       */
      std::sort(cache.begin(), cache.end(), [&triples](size_t a, size_t b) {
        ScTemplateConstr3 const & aTriple = triples[a];
        ScTemplateConstr3 const & bTriple = triples[b];

        // compare by addrs arguments count
        size_t const aAddrCount = aTriple.CountAddrs();
        size_t const bAddrCount = bTriple.CountAddrs();

        if (aAddrCount != bAddrCount)
          return (aAddrCount > bAddrCount);

        // compare by fixed arguments count
        size_t const aFCount = aTriple.CountFixed();
        size_t const bFCount = bTriple.CountFixed();

        if (aFCount != bFCount)
          return (aFCount > bFCount);

        // compare replacement types
        auto const CalculateReplTypeValue = [](ScTemplateConstr3 const & tr)
        {
          size_t res = 0;
          for (size_t i = 0; i < 3; ++i)
          {
            auto const & v = tr.GetValues()[i];
            if (v.m_itemType == ScTemplateItemValue::Type::Replace)
              ++res;
          }
          return res;
        };

        size_t const aRV = CalculateReplTypeValue(aTriple);
        size_t const bRV = CalculateReplTypeValue(bTriple);

        if (aRV != bRV)
          return (aRV < bRV);

        // compare by replacements count
        size_t const aRCount = aTriple.CountReplacements();
        size_t const bRCount = bTriple.CountReplacements();

        if (aRCount != bRCount)
          return (aRCount > bRCount);

        auto const & aValues = aTriple.GetValues();
        auto const & bValues = bTriple.GetValues();

        return false;
      });
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
    ScTemplateItemValue const * values = constr.GetValues();

    ScTemplateItemValue const & value0 = values[0];
    ScTemplateItemValue const & value1 = values[1];
    ScTemplateItemValue const & value2 = values[2];

    ScAddr const addr0 = ResolveAddr(value0);
    ScAddr const addr1 = ResolveAddr(value1);
    ScAddr const addr2 = ResolveAddr(value2);

    auto const PrepareType = [](ScType const & type)
    {
      if (type.HasConstancyFlag())
        return type.UpConstType();

      return type;
    };

    if (addr0.IsValid())
    {
      if (addr2.IsValid()) // F_A_F
      {
        return m_context.Iterator3(addr0, PrepareType(value1.m_typeValue), addr2);
      }
      else // F_A_A
      {
        return m_context.Iterator3(addr0, PrepareType(value1.m_typeValue), PrepareType(value2.m_typeValue));
      }
    }
    else if (addr2.IsValid()) // A_A_F
    {
      return m_context.Iterator3(PrepareType(value0.m_typeValue), PrepareType(value1.m_typeValue), addr2);
    }
    else if (addr1.IsValid()) // A_F_A
    {
      return m_context.Iterator3(PrepareType(value0.m_typeValue), addr1, PrepareType(value2.m_typeValue));
    }
    else // unknown iterator type
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Unknown iterator type");
    }

    return ScIterator3Ptr();
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

  void Iteration(size_t orderIndex, ScTemplateSearchResult & result)
  {
    size_t const constrIndex = m_template.m_searchCachedOrder[orderIndex];

    SC_ASSERT(constrIndex < m_template.m_constructions.size(), ());
    size_t const finishIdx = m_template.m_constructions.size() - 1;
    size_t resultIdx = constrIndex * 3;

    /// TODO: prevent recursive search and make test for that case

    ScTemplateConstr3 const & constr = m_template.m_constructions[constrIndex];
    ScTemplateItemValue const * values = constr.GetValues();
    ScIterator3Ptr const it3 = CreateIterator(constr);
    while (it3->Next())
    {
      /// check if search in structure
      if (m_struct.IsValid())
      {
        if (!CheckInStruct(it3->Get(0)) ||
            !CheckInStruct(it3->Get(1)) ||
            !CheckInStruct(it3->Get(2)))
        {
          continue;
        }
      }

      // do not make cycle for optimization issues (remove comparsion expresion)
      m_resultAddrs[resultIdx] = it3->Get(0);
      m_resultAddrs[resultIdx + 1] = it3->Get(1);
      m_resultAddrs[resultIdx + 2] = it3->Get(2);

      RefReplacement(values[0], it3->Get(0));
      RefReplacement(values[1], it3->Get(1));
      RefReplacement(values[2], it3->Get(2));

      if (orderIndex == finishIdx)
      {
        result.m_results.push_back(m_resultAddrs);
      }
      else
      {
        Iteration(orderIndex + 1, result);
      }

      UnrefReplacement(values[0]);
      UnrefReplacement(values[1]);
      UnrefReplacement(values[2]);
    }
  }

  bool operator () (ScTemplateSearchResult & result)
  {
    result.clear();

    result.m_replacements = m_template.m_replacements;
    m_resultAddrs.resize(CalculateOneResultSize());
    m_replRefs.resize(m_resultAddrs.size(), 0);

    Iteration(0, result);

    return (result.Size() > 0);
  }

  size_t CalculateOneResultSize() const
  {
    return m_template.m_constructions.size() * 3;
  }

private:
  ScTemplate const & m_template;
  ScMemoryContext & m_context;
  ScAddr const & m_struct;

  using StructCache = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;
  StructCache m_structCache;

  ScAddrVector m_resultAddrs;
  using ReplRefs = std::vector<uint32_t>;
  ReplRefs m_replRefs;
};

bool ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(*this, ctx, ScAddr());
  return search(result);
}

bool ScTemplate::SearchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(*this, ctx, scStruct);
  return search(result);
}
