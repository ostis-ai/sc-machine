/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

#include "sc_debug.hpp"
#include "sc_memory.hpp"

#include <algorithm>
#include <utility>

class ScTemplateSearch
{
public:
  ScTemplateSearch(
      ScTemplate & templ,
      ScMemoryContext & context,
      ScAddr const & structure,
      ScTemplateSearchResultCallback callback = {},
      ScTemplateSearchResultCheckCallback checkCallback = {})
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
    , m_callback(std::move(callback))
    , m_checkCallback(std::move(checkCallback))
  {
    PrepareSearch();
  }

  using ScTemplateGroupedTriples = ScTemplate::ScTemplateGroupedTriples;

  void PrepareSearch()
  {
    auto triplesWithConstBeginElement = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::FAE];
    if (triplesWithConstBeginElement.empty())
    {
      triplesWithConstBeginElement = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::FAN];
    }

    sc_int32 priorityTripleIdx = -1;
    sc_int32 minOutputArcsCount = -1;
    for (size_t const tripleIdx : triplesWithConstBeginElement)
    {
      ScTemplateTriple const * triple = m_template.m_triples[tripleIdx];
      auto const count = (sc_int32)m_context.GetElementOutputArcsCount(triple->GetValues()[0].m_addrValue);

      if (minOutputArcsCount == -1 || count < minOutputArcsCount)
      {
        priorityTripleIdx = (sc_int32)tripleIdx;
        minOutputArcsCount = (sc_int32)count;
      }
    }

    if (priorityTripleIdx != -1)
    {
      m_template.m_orderedTriples[(size_t)ScTemplateTripleType::PFAE].insert(priorityTripleIdx);
    }

    for (auto const * triple : m_template.m_triples)
    {
      auto & values = triple->GetValues();
      ScTemplateItemValue const & item1 = values[0];
      ScTemplateItemValue const & item2 = values[1];
      ScTemplateItemValue const & item3 = values[2];

      for (auto * otherTriple : m_template.m_triples)
      {
        auto const & otherValues = otherTriple->GetValues();
        ScTemplateItemValue const & otherItem1 = otherValues[0];
        ScTemplateItemValue const & otherItem2 = otherValues[1];
        ScTemplateItemValue const & otherItem3 = otherValues[2];

        if (triple->m_index == otherTriple->m_index)
          continue;

        auto const TryAddDependence = [&](ScTemplateItemValue const & item,
                                          ScTemplateItemValue const & otherItem1,
                                          ScTemplateItemValue const & otherItem2,
                                          ScTemplateItemValue const & otherItem3) {
          if (item.m_replacementName.empty())
            return;

          sc_bool const withItem1Equal = item.m_replacementName == otherItem1.m_replacementName;
          sc_bool const withItem2Equal = item.m_replacementName == otherItem2.m_replacementName;
          sc_bool const withItem3Equal = item.m_replacementName == otherItem3.m_replacementName;

          if (withItem1Equal || withItem2Equal || withItem3Equal)
          {
            std::ostringstream stream{item.m_replacementName};
            stream << triple->m_index;

            std::string const key = stream.str();
            auto const & found = m_itemsToTriples.find(key);
            if (found == m_itemsToTriples.cend())
              m_itemsToTriples.insert({key, {otherTriple->m_index}});
            else
              found->second.insert(otherTriple->m_index);
          }
        };

        TryAddDependence(item1, otherItem1, otherItem2, otherItem3);
        TryAddDependence(item2, otherItem1, otherItem2, otherItem3);
        TryAddDependence(item3, otherItem1, otherItem2, otherItem3);
      }
    }
  }

  inline bool IsStructureValid()
  {
    return m_structure.IsValid();
  }

  inline bool IsInStructure(ScAddr const & addr)
  {
    return m_context.HelperCheckEdge(m_structure, addr, ScType::EdgeAccessConstPosPerm);
  }

  ScAddr const & ResolveAddr(
      ScTemplateItemValue const & value,
      ScAddrVector const & resultAddrs,
      ScTemplateSearchResult & result) const
  {
    static ScAddr empty;

    switch (value.m_itemType)
    {
    case ScTemplateItemValue::Type::Addr:
      return value.m_addrValue;

    case ScTemplateItemValue::Type::Replace:
    {
      auto const & it = result.m_replacements.equal_range(value.m_replacementName);
      for (auto curIt = it.first; curIt != it.second; ++curIt)
      {
        ScAddr const & addr = resultAddrs[curIt->second];
        if (addr.IsValid())
          return addr;
      }

      auto const & addrsIt = m_template.m_namesToAddrs.find(value.m_replacementName);
      if (addrsIt != m_template.m_namesToAddrs.cend())
      {
        return addrsIt->second;
      }

      return empty;
    }

    case ScTemplateItemValue::Type::Type:
    {
      if (!value.m_replacementName.empty())
      {
        auto const & it = result.m_replacements.equal_range(value.m_replacementName);
        for (auto curIt = it.first; curIt != it.second; ++curIt)
        {
          ScAddr const & addr = resultAddrs[curIt->second];
          if (addr.IsValid())
            return addr;
        }

        return empty;
      }
      break;
    }

    default:
      break;
    }

    return empty;
  }

  ScIterator3Ptr CreateIterator(
      ScTemplateItemValue const & value1,
      ScTemplateItemValue const & value2,
      ScTemplateItemValue const & value3,
      ScAddrVector const & resultAddrs,
      ScTemplateSearchResult & result)
  {
    ScAddr const addr1 = ResolveAddr(value1, resultAddrs, result);
    ScAddr const addr2 = ResolveAddr(value2, resultAddrs, result);
    ScAddr const addr3 = ResolveAddr(value3, resultAddrs, result);

    auto const PrepareType = [](ScType const & type) {
      if (type.HasConstancyFlag())
        return type.UpConstType();

      return type;
    };

    if (addr1.IsValid())
    {
      if (!addr2.IsValid())
      {
        if (addr3.IsValid())  // F_A_F
        {
          return m_context.Iterator3(addr1, PrepareType(value2.m_typeValue), addr3);
        }
        else  // F_A_A
        {
          return m_context.Iterator3(addr1, PrepareType(value2.m_typeValue), PrepareType(value3.m_typeValue));
        }
      }
      else
      {
        if (addr3.IsValid())  // F_F_F
        {
          return m_context.Iterator3(addr1, addr2, addr3);
        }
        else  // F_F_A
        {
          return m_context.Iterator3(addr1, addr2, PrepareType(value3.m_typeValue));
        }
      }
    }
    else if (addr3.IsValid())
    {
      if (addr2.IsValid())  // A_F_F
      {
        return m_context.Iterator3(PrepareType(value1.m_typeValue), addr2, addr3);
      }
      else  // A_A_F
      {
        return m_context.Iterator3(PrepareType(value1.m_typeValue), PrepareType(value2.m_typeValue), addr3);
      }
    }
    else if (addr2.IsValid() && !addr3.IsValid())  // A_F_A
    {
      return m_context.Iterator3(PrepareType(value1.m_typeValue), addr2, PrepareType(value3.m_typeValue));
    }

    return {};
  }

  void FindDependedTriple(
      ScTemplateItemValue const & value,
      ScTemplateTriple const * triple,
      ScTemplateGroupedTriples & nextTriples)
  {
    if (value.m_replacementName.empty())
      return;

    std::ostringstream stream{value.m_replacementName};
    stream << triple->m_index;

    auto const & found = m_itemsToTriples.find(stream.str());
    if (found != m_itemsToTriples.cend())
    {
      nextTriples = found->second;
    }
  }

  bool IsTriplesEqual(ScTemplateTriple const * triple, ScTemplateTriple const * otherTriple)
  {
    auto const & tripleValues = triple->GetValues();
    auto const & otherTripleValues = otherTriple->GetValues();

    auto const & IsTriplesItemsEqual =
        [this](ScTemplateItemValue const & value, ScTemplateItemValue const & otherValue) -> bool {
      bool isEqual = value.m_typeValue == otherValue.m_typeValue;
      if (!isEqual)
      {
        auto found = m_template.m_namesToTypes.find(value.m_replacementName);
        if (found == m_template.m_namesToTypes.cend())
        {
          found = m_template.m_namesToTypes.find(value.m_replacementName);
          if (found != m_template.m_namesToTypes.cend())
          {
            isEqual = value.m_typeValue == found->second;
          }
        }
        else
        {
          isEqual = found->second == otherValue.m_typeValue;
        }
      }

      if (isEqual)
      {
        isEqual = value.m_addrValue == otherValue.m_addrValue;
      }

      if (!isEqual)
      {
        auto found = m_template.m_namesToAddrs.find(value.m_replacementName);
        if (found == m_template.m_namesToAddrs.cend())
        {
          found = m_template.m_namesToAddrs.find(value.m_replacementName);
          if (found != m_template.m_namesToAddrs.cend())
          {
            isEqual = value.m_addrValue == found->second;
          }
        }
        else
        {
          isEqual = found->second == otherValue.m_addrValue;
        }
      }

      return isEqual;
    };

    return IsTriplesItemsEqual(tripleValues[0], otherTripleValues[0]) &&
           IsTriplesItemsEqual(tripleValues[1], otherTripleValues[1]) &&
           IsTriplesItemsEqual(tripleValues[2], otherTripleValues[2]) &&
           (tripleValues[0].m_replacementName == otherTripleValues[0].m_replacementName ||
            tripleValues[2].m_replacementName == otherTripleValues[2].m_replacementName);
  };

  using UsedEdges = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;
  using ScTriplesOrderUsedEdges = std::vector<UsedEdges>;

  void DoIterationOnNextEqualTriples(
      ScTemplateGroupedTriples const & triples,
      std::unordered_set<size_t> & checkedTriples,
      ScTemplateGroupedTriples const & currentIterableTriples,
      ScAddrVector & resultAddrs,
      ScTemplateSearchResult & result,
      bool & isFinished,
      bool & isLast)
  {
    isLast = true;
    isFinished = true;

    std::unordered_set<size_t> iteratedTriples;

    for (size_t const idx : triples)
    {
      ScTemplateTriple * triple = m_template.m_triples[idx];
      if (iteratedTriples.find(triple->m_index) != iteratedTriples.cend())
        continue;

      ScTemplateGroupedTriples equalTriples;
      for (ScTemplateTriple * otherTriple : m_template.m_triples)
      {
        // check if iterable triple is equal to current, not checked and not iterable with previous
        if (checkedTriples.find(otherTriple->m_index) == checkedTriples.cend() &&
            std::find_if(
                currentIterableTriples.cbegin(),
                currentIterableTriples.cend(),
                [&](size_t const otherIdx) {
                  return idx == otherIdx;
                }) == currentIterableTriples.cend() &&
            IsTriplesEqual(triple, otherTriple))
        {
          equalTriples.insert(otherTriple->m_index);
          iteratedTriples.insert(otherTriple->m_index);
        }
      }

      if (!equalTriples.empty())
      {
        isLast = false;
        DoDependenceIteration(equalTriples, checkedTriples, resultAddrs, result, isFinished);
      }
    }
  }

  void DoDependenceIteration(
      ScTemplateGroupedTriples const & triples,
      std::unordered_set<size_t> & checkedTriples,
      ScAddrVector & resultAddrs,
      ScTemplateSearchResult & result,
      bool & isFinished)
  {
    auto const & TryDoNextDependenceIteration = [this, &result](
                                                    ScTemplateTriple const * triple,
                                                    ScTemplateItemValue const & item,
                                                    ScAddr const & itemAddr,
                                                    std::unordered_set<size_t> & checkedTriples,
                                                    ScAddrVector & resultAddrs,
                                                    ScTemplateGroupedTriples const & currentIterableTriples,
                                                    bool & isFinished,
                                                    bool & isLast) {
      ScTemplateGroupedTriples nextTriples;
      FindDependedTriple(item, triple, nextTriples);

      DoIterationOnNextEqualTriples(
          nextTriples, checkedTriples, currentIterableTriples, resultAddrs, result, isFinished, isLast);
    };

    auto const & UpdateResults = [&result](
                                     ScTemplateItemValue const & value,
                                     ScAddr const & addr,
                                     size_t const elementNum,
                                     ScAddrVector & resultAddrs) {
      resultAddrs[elementNum] = addr;

      if (value.m_replacementName.empty())
        return;

      result.m_replacements.insert({value.m_replacementName, elementNum});
    };

    auto const & ClearResults =
        [this](size_t tripleIdx, ScAddrVector & resultAddrs, std::unordered_set<size_t> & checkedTriples) {
          checkedTriples.erase(tripleIdx);

          tripleIdx *= 3;
          resultAddrs[tripleIdx] = ScAddr::Empty;
          m_usedEdges.erase(resultAddrs[++tripleIdx]);
          resultAddrs[tripleIdx] = ScAddr::Empty;
          resultAddrs[++tripleIdx] = ScAddr::Empty;
        };

    ScTemplateTriple * triple = m_template.m_triples[*triples.begin()];

    bool isAllChildrenFinished = false;
    bool isLast = false;

    auto values = triple->GetValues();

    ScTemplateItemValue value1 = values[0];
    ScTemplateItemValue value2 = values[1];
    ScTemplateItemValue value3 = values[2];

    ScIterator3Ptr const it = CreateIterator(value1, value2, value3, resultAddrs, result);
    if (!it || !it->IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "During search procedure has been chosen var triple");
    }

    size_t count = 0;

    std::unordered_set<size_t> const currentCheckedTriples{checkedTriples};
    ScAddrVector const currentResultAddrs{resultAddrs};
    while (it->Next())
    {
      ScAddr const & addr1 = it->Get(0);
      ScAddr const & addr2 = it->Get(1);
      ScAddr const & addr3 = it->Get(2);

      // check triple elements by structure belonging or predicate callback
      if ((IsStructureValid() && (!IsInStructure(addr1) || !IsInStructure(addr2) || !IsInStructure(addr3))) ||
          (m_checkCallback && !m_checkCallback(addr1, addr2, addr3)))
      {
        for (size_t const tripleIdx : triples)
        {
          m_triplesOrderUsedEdges[tripleIdx].insert(addr2);
        }
        continue;
      }

      // check if edge is used for previous triples
      if (m_usedEdges.find(addr2) != m_usedEdges.cend())
      {
        continue;
      }

      for (size_t const tripleIdx : triples)
      {
        triple = m_template.m_triples[tripleIdx];

        // check if edge is used for other equal triple
        if (m_triplesOrderUsedEdges[tripleIdx].find(addr2) != m_triplesOrderUsedEdges[tripleIdx].cend())
        {
          continue;
        }

        // check if all equal triples found to make a new search result item
        if (isAllChildrenFinished && count == triples.size())
        {
          count = 0;
          checkedTriples = std::unordered_set<size_t>{currentCheckedTriples};
          resultAddrs.assign(currentResultAddrs.cbegin(), currentResultAddrs.cend());
        }

        if (checkedTriples.find(tripleIdx) != checkedTriples.cend())
        {
          continue;
        }

        values = triple->GetValues();
        value1 = values[0];
        value2 = values[1];
        value3 = values[2];

        {
          // don't use cycle to call this function
          size_t idx = tripleIdx * 3;
          UpdateResults(value1, addr1, idx, resultAddrs);
          UpdateResults(value2, addr2, ++idx, resultAddrs);
          UpdateResults(value3, addr3, ++idx, resultAddrs);

          checkedTriples.insert(tripleIdx);
          m_triplesOrderUsedEdges[tripleIdx].insert(addr2);
          m_usedEdges.insert(addr2);
        }

        // find next depended on triples and analyse result
        {
          bool isChildFinished = false;
          bool isNoChild = false;

          // first of all check triples by edge, it is more effectively
          TryDoNextDependenceIteration(
              triple, value2, addr2, checkedTriples, resultAddrs, triples, isChildFinished, isNoChild);
          isAllChildrenFinished = isChildFinished;
          isLast = isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultAddrs, checkedTriples);
            continue;
          }

          TryDoNextDependenceIteration(
              triple, value1, addr1, checkedTriples, resultAddrs, triples, isChildFinished, isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultAddrs, checkedTriples);
            continue;
          }

          TryDoNextDependenceIteration(
              triple, value3, addr3, checkedTriples, resultAddrs, triples, isChildFinished, isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultAddrs, checkedTriples);
            continue;
          }

          // all connected triples found
          if (isAllChildrenFinished)
          {
            // current triple is checked
            ++count;

            // current edge is busy for all equal triples
            for (size_t const idx : triples)
            {
              m_triplesOrderUsedEdges[idx].insert(addr2);
            }
            break;
          }
        }
      }

      // there are no next triples for current triple, it is last
      if (isLast & isAllChildrenFinished)
      {
        if (checkedTriples.size() == m_template.m_triples.size())
        {
          if (m_callback)
          {
            m_callback(ScTemplateSearchResultItem(&resultAddrs, &result.m_replacements));
          }
          else
          {
            result.m_results.emplace_back(resultAddrs);
          }
        }
      }
    }

    isFinished = isAllChildrenFinished;
  }

  void DoIterations(ScTemplateSearchResult & result)
  {
    if (m_template.m_triples.empty())
      return;

    auto const & DoStartIteration = [this, &result](ScTemplateGroupedTriples const & triples) {
      std::vector<ScAddr> resultAddrs;
      resultAddrs.resize(CalculateOneResultSize());
      std::unordered_set<size_t> checkedTriples;
      checkedTriples.reserve(CalculateOneResultSize());

      bool isFinished = false;
      bool isLast = false;

      DoIterationOnNextEqualTriples(triples, checkedTriples, {}, resultAddrs, result, isFinished, isLast);
    };

    auto const & triples = m_template.m_orderedTriples;
    for (ScTemplateGroupedTriples const & equalTriples : triples)
    {
      if (!equalTriples.empty())
      {
        DoStartIteration(equalTriples);

        // TODO: Provide logic for template with more connectivity components than 1 in sc-template
        break;
      }
    }
  }

  ScTemplate::Result operator()(ScTemplateSearchResult & result)
  {
    result.Clear();
    m_triplesOrderUsedEdges.resize(CalculateOneResultSize());

    DoIterations(result);

    return ScTemplate::Result(result.Size() > 0);
  }

  void operator()()
  {
    ScTemplateSearchResult result;
    m_triplesOrderUsedEdges.resize(CalculateOneResultSize());

    DoIterations(result);
  }

  size_t CalculateOneResultSize() const
  {
    return m_template.m_triples.size() * 3;
  }

private:
  ScTemplate & m_template;
  ScMemoryContext & m_context;

  ScAddr const m_structure;
  ScTemplateSearchResultCallback m_callback;
  ScTemplateSearchResultCheckCallback m_checkCallback;

  ScTriplesOrderUsedEdges m_triplesOrderUsedEdges;
  UsedEdges m_usedEdges;
  std::map<std::string, ScTemplateGroupedTriples> m_itemsToTriples;
};

ScTemplate::Result ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty);
  return search(result);
}

void ScTemplate::Search(
    ScMemoryContext & ctx,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultCheckCallback const & checkCallback) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty, callback, checkCallback);
  search();
}

ScTemplate::Result ScTemplate::SearchInStruct(
    ScMemoryContext & ctx,
    ScAddr const & scStruct,
    ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, scStruct);
  return search(result);
}
