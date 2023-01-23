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
  ScTemplateSearch(ScTemplate & templ, ScMemoryContext & context, ScAddr const & structure)
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
  {
    PrepareSearch();
  }

  ScTemplateSearch(
      ScTemplate & templ,
      ScMemoryContext & context,
      ScAddr const & structure,
      ScTemplateSearchResultCallback callback,
      ScTemplateSearchResultCheckCallback checkCallback = {})
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
    , m_callback(std::move(callback))
    , m_checkCallback(std::move(checkCallback))
  {
    PrepareSearch();
  }

  ScTemplateSearch(
      ScTemplate & templ,
      ScMemoryContext & context,
      ScAddr const & structure,
      ScTemplateSearchResultCallbackWithRequest callback,
      ScTemplateSearchResultCheckCallback checkCallback = {})
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
    , m_callbackWithRequest(std::move(callback))
    , m_checkCallback(std::move(checkCallback))
  {
    PrepareSearch();
  }

  using ScTemplateGroupedTriples = ScTemplate::ScTemplateGroupedTriples;

private:
  void PrepareSearch()
  {
    auto const & SortTriplesWithConstBeginElement = [this]() {
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
    };

    auto const & SaveDependenciesBetweenTriples = [this]() {
      for (ScTemplateTriple const * triple : m_template.m_triples)
      {
        ScTemplateItemValue const & item1 = (*triple)[0];
        ScTemplateItemValue const & item2 = (*triple)[1];
        ScTemplateItemValue const & item3 = (*triple)[2];

        for (ScTemplateTriple * otherTriple : m_template.m_triples)
        {
          ScTemplateItemValue const & otherItem1 = (*otherTriple)[0];
          ScTemplateItemValue const & otherItem2 = (*otherTriple)[1];
          ScTemplateItemValue const & otherItem3 = (*otherTriple)[2];

          if (triple->m_index == otherTriple->m_index)
            continue;

          auto const & TryAddDependence = [this, &triple, &otherTriple](
                                              ScTemplateItemValue const & item,
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
              std::string const & key = GetKey(triple, item);
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
    };

    auto const & RemoveCycledDependenciesBetweenTriples = [this]() {
      auto const & faeTriples = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::FAN];

      for (ScTemplateTriple const * triple : m_template.m_triples)
      {
        auto const & item1 = (*triple)[0];

        bool isFound = false;
        if (item1.IsAddr() && faeTriples.find(triple->m_index) != faeTriples.cend())
        {
          ScTemplateGroupedTriples checkedTriples;
          FindCycleWithFAATriple(item1, triple, triple, checkedTriples, isFound);
        }

        if (isFound)
        {
          std::string const & key = GetKey(triple, item1);
          m_cycledTriples.insert(triple->m_index);
        }
      }

      for (size_t const idx : m_cycledTriples)
      {
        ScTemplateTriple * triple = m_template.m_triples[idx];
        std::string const & key = GetKey(triple, (*triple)[0]);

        auto const & found = m_itemsToTriples.find(key);
        if (found != m_itemsToTriples.cend())
        {
          for (size_t const otherIdx : m_cycledTriples)
          {
            found->second.erase(otherIdx);
          }
        }
      }
    };

    {
      SortTriplesWithConstBeginElement();
      SaveDependenciesBetweenTriples();
      RemoveCycledDependenciesBetweenTriples();
    }
  }

  static std::string GetKey(ScTemplateTriple const * triple, ScTemplateItemValue const & item)
  {
    std::ostringstream stream;
    stream << item.m_replacementName << triple->m_index;

    return stream.str();
  }

  void FindCycleWithFAATriple(
      ScTemplateItemValue const & item,
      ScTemplateTriple const * triple,
      ScTemplateTriple const * tripleToFind,
      ScTemplateGroupedTriples checkedTriples,
      bool & isFound)
  {
    if (isFound)
    {
      return;
    }

    auto const & FindCycleWithFAATripleByTripleItem = [this, &tripleToFind, &checkedTriples, &isFound](
                                                          ScTemplateItemValue const & item,
                                                          ScTemplateTriple const * triple,
                                                          ScTemplateItemValue const & previousItem) {
      if (!item.m_replacementName.empty() && item.m_replacementName == previousItem.m_replacementName)
      {
        return;
      }

      if (item.m_addrValue.IsValid() && item.m_addrValue == previousItem.m_addrValue)
      {
        return;
      }

      FindCycleWithFAATriple(item, triple, tripleToFind, checkedTriples, isFound);
    };

    ScTemplateGroupedTriples nextTriples;
    FindDependedTriple(item, triple, nextTriples);

    for (size_t const otherTripleIdx : nextTriples)
    {
      ScTemplateTriple const * otherTriple = m_template.m_triples[otherTripleIdx];

      if ((otherTripleIdx == tripleToFind->m_index && item.m_replacementName != (*tripleToFind)[0].m_replacementName) ||
          isFound)
      {
        isFound = true;
        break;
      }

      if (checkedTriples.find(otherTripleIdx) != checkedTriples.cend())
      {
        continue;
      }

      {
        auto const & items = otherTriple->GetValues();
        checkedTriples.insert(otherTripleIdx);

        FindCycleWithFAATripleByTripleItem(items[0], otherTriple, item);
        FindCycleWithFAATripleByTripleItem(items[1], otherTriple, item);
        FindCycleWithFAATripleByTripleItem(items[2], otherTriple, item);
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
      ScTemplateItemValue const & item,
      ScAddrVector const & resultAddrs,
      ScTemplateSearchResult & result) const
  {
    switch (item.m_itemType)
    {
    case ScTemplateItemValue::Type::Addr:
    {
      return item.m_addrValue;
    }

    case ScTemplateItemValue::Type::Replace:
    {
      auto const & it = result.m_replacements.equal_range(item.m_replacementName);
      for (auto curIt = it.first; curIt != it.second; ++curIt)
      {
        ScAddr const & addr = resultAddrs[curIt->second];
        if (addr.IsValid())
          return addr;
      }

      auto const & addrsIt = m_template.m_namesToAddrs.find(item.m_replacementName);
      if (addrsIt != m_template.m_namesToAddrs.cend())
      {
        return addrsIt->second;
      }

      return ScAddr::Empty;
    }

    case ScTemplateItemValue::Type::Type:
    {
      if (!item.m_replacementName.empty())
      {
        auto const & it = result.m_replacements.equal_range(item.m_replacementName);
        for (auto curIt = it.first; curIt != it.second; ++curIt)
        {
          ScAddr const & addr = resultAddrs[curIt->second];
          if (addr.IsValid())
            return addr;
        }

        return ScAddr::Empty;
      }
    }

    default:
    {
      return ScAddr::Empty;
    }
    }
  }

  ScIterator3Ptr CreateIterator(
      ScTemplateItemValue const & item1,
      ScTemplateItemValue const & item2,
      ScTemplateItemValue const & item3,
      ScAddrVector const & resultAddrs,
      ScTemplateSearchResult & result)
  {
    ScAddr const addr1 = ResolveAddr(item1, resultAddrs, result);
    ScAddr const addr2 = ResolveAddr(item2, resultAddrs, result);
    ScAddr const addr3 = ResolveAddr(item3, resultAddrs, result);

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
          return m_context.Iterator3(addr1, PrepareType(item2.m_typeValue), addr3);
        }
        else  // F_A_A
        {
          return m_context.Iterator3(addr1, PrepareType(item2.m_typeValue), PrepareType(item3.m_typeValue));
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
          return m_context.Iterator3(addr1, addr2, PrepareType(item3.m_typeValue));
        }
      }
    }
    else if (addr3.IsValid())
    {
      if (addr2.IsValid())  // A_F_F
      {
        return m_context.Iterator3(PrepareType(item1.m_typeValue), addr2, addr3);
      }
      else  // A_A_F
      {
        return m_context.Iterator3(PrepareType(item1.m_typeValue), PrepareType(item2.m_typeValue), addr3);
      }
    }
    else if (addr2.IsValid() && !addr3.IsValid())  // A_F_A
    {
      return m_context.Iterator3(PrepareType(item1.m_typeValue), addr2, PrepareType(item3.m_typeValue));
    }

    return {};
  }

  void FindDependedTriple(
      ScTemplateItemValue const & item,
      ScTemplateTriple const * triple,
      ScTemplateGroupedTriples & nextTriples)
  {
    if (item.m_replacementName.empty())
      return;

    std::string const & key = GetKey(triple, item);
    auto const & found = m_itemsToTriples.find(key);
    if (found != m_itemsToTriples.cend())
    {
      nextTriples = found->second;
    }
  }

  bool IsTriplesEqual(
      ScTemplateTriple const * triple,
      ScTemplateTriple const * otherTriple,
      std::string const & itemName = "")
  {
    if (triple->m_index == otherTriple->m_index)
    {
      return true;
    }

    auto const & tripleValues = triple->GetValues();
    auto const & otherTripleValues = otherTriple->GetValues();

    auto const & IsTriplesItemsEqual =
        [this](ScTemplateItemValue const & item, ScTemplateItemValue const & otherItem) -> bool {
      bool isEqual = item.m_typeValue == otherItem.m_typeValue;
      if (!isEqual)
      {
        auto found = m_template.m_namesToTypes.find(item.m_replacementName);
        if (found == m_template.m_namesToTypes.cend())
        {
          found = m_template.m_namesToTypes.find(item.m_replacementName);
          if (found != m_template.m_namesToTypes.cend())
          {
            isEqual = item.m_typeValue == found->second;
          }
        }
        else
        {
          isEqual = found->second == otherItem.m_typeValue;
        }
      }

      if (isEqual)
      {
        isEqual = item.m_addrValue == otherItem.m_addrValue;
      }

      if (!isEqual)
      {
        auto found = m_template.m_namesToAddrs.find(item.m_replacementName);
        if (found == m_template.m_namesToAddrs.cend())
        {
          found = m_template.m_namesToAddrs.find(item.m_replacementName);
          if (found != m_template.m_namesToAddrs.cend())
          {
            isEqual = item.m_addrValue == found->second;
          }
        }
        else
        {
          isEqual = found->second == otherItem.m_addrValue;
        }
      }

      return isEqual;
    };

    return IsTriplesItemsEqual(tripleValues[0], otherTripleValues[0]) &&
           IsTriplesItemsEqual(tripleValues[1], otherTripleValues[1]) &&
           IsTriplesItemsEqual(tripleValues[2], otherTripleValues[2]) &&
           ((tripleValues[0].m_replacementName == otherTripleValues[0].m_replacementName &&
             (itemName.empty() || otherTripleValues[0].m_replacementName == itemName)) ||
            (tripleValues[2].m_replacementName == otherTripleValues[2].m_replacementName &&
             (itemName.empty() || otherTripleValues[0].m_replacementName == itemName)));
  };

  using UsedEdges = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;
  using ScTriplesOrderCheckedEdges = std::vector<UsedEdges>;

  void DoIterationOnNextEqualTriples(
      ScTemplateGroupedTriples const & triples,
      std::string const & itemName,
      size_t const resultIdx,
      ScTemplateGroupedTriples const & currentIterableTriples,
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
        auto const & checkedTriples = m_resultCheckedTriples[resultIdx];
        if (checkedTriples.find(otherTriple->m_index) == checkedTriples.cend() &&
            std::find_if(
                currentIterableTriples.cbegin(),
                currentIterableTriples.cend(),
                [&](size_t const otherIdx) {
                  return idx == otherIdx;
                }) == currentIterableTriples.cend() &&
            IsTriplesEqual(triple, otherTriple, itemName))
        {
          equalTriples.insert(otherTriple->m_index);
          iteratedTriples.insert(otherTriple->m_index);
        }
      }

      if (!equalTriples.empty())
      {
        isLast = false;
        DoDependenceIteration(equalTriples, resultIdx, result);

        isFinished = std::all_of(equalTriples.begin(), equalTriples.end(), [this, resultIdx](size_t const idx) {
          return m_resultCheckedTriples[resultIdx].find(idx) != m_resultCheckedTriples[resultIdx].cend();
        });
      }
    }
  }

  void DoDependenceIteration(
      ScTemplateGroupedTriples const & triples,
      size_t resultIdx,
      ScTemplateSearchResult & result)
  {
    auto const & TryDoNextDependenceIteration = [this, &result](
                                                    ScTemplateTriple const * triple,
                                                    ScTemplateItemValue const & item,
                                                    ScAddr const & itemAddr,
                                                    size_t const resultIdx,
                                                    ScTemplateGroupedTriples const & currentIterableTriples,
                                                    bool & isFinished,
                                                    bool & isLast) {
      ScTemplateGroupedTriples nextTriples;
      FindDependedTriple(item, triple, nextTriples);

      DoIterationOnNextEqualTriples(
          nextTriples, item.m_replacementName, resultIdx, currentIterableTriples, result, isFinished, isLast);
    };

    auto const & UpdateItemResults = [&result](
                                         ScTemplateItemValue const & item,
                                         ScAddr const & addr,
                                         size_t const elementNum,
                                         ScAddrVector & resultAddrs) {
      resultAddrs[elementNum] = addr;

      if (item.m_replacementName.empty())
        return;

      result.m_replacements.insert({item.m_replacementName, elementNum});
    };

    auto const & UpdateResults = [this, &UpdateItemResults, &result](
                                     ScTemplateTriple * triple,
                                     size_t const resultIdx,
                                     ScAddr const & addr1,
                                     ScAddr const & addr2,
                                     ScAddr const & addr3) {
      m_resultCheckedTriples[resultIdx].insert(triple->m_index);
      m_triplesOrderUsedEdges[triple->m_index].insert(addr2);

      size_t itemIdx = triple->m_index * 3;

      for (size_t i = resultIdx; i < result.Size(); ++i)
      {
        ScAddrVector & resultAddrs = result.m_results[i];

        UpdateItemResults((*triple)[0], addr1, itemIdx, resultAddrs);
        UpdateItemResults((*triple)[1], addr2, itemIdx + 1, resultAddrs);
        UpdateItemResults((*triple)[2], addr3, itemIdx + 2, resultAddrs);
      }
    };

    auto const & ClearResults = [this](size_t const tripleIdx, size_t const resultIdx, ScAddrVector & resultAddrs) {
      m_resultCheckedTriples[resultIdx].erase(tripleIdx);

      size_t itemIdx = tripleIdx * 3;
      resultAddrs[itemIdx] = ScAddr::Empty;
      m_triplesOrderUsedEdges[tripleIdx].erase(resultAddrs[++itemIdx]);
      resultAddrs[itemIdx] = ScAddr::Empty;
      resultAddrs[++itemIdx] = ScAddr::Empty;
    };

    ScTemplateTriple * triple = m_template.m_triples[*triples.begin()];

    bool isAllChildrenFinished = false;
    bool isLast = false;

    ScTemplateItemValue item1 = (*triple)[0];
    ScTemplateItemValue item2 = (*triple)[1];
    ScTemplateItemValue item3 = (*triple)[2];

    ScIterator3Ptr const it = CreateIterator(item1, item2, item3, result.m_results[resultIdx], result);
    if (!it || !it->IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "During search procedure has been chosen var triple");
    }

    size_t notFinishedTriplesCount = 0;
    size_t finishedTriplesCount = 0;

    std::unordered_set<size_t> const currentCheckedTriples{m_resultCheckedTriples[resultIdx]};
    ScAddrVector const currentResultAddrs{result.m_results[resultIdx]};
    while (it->Next() && !isStopped)
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

      // check if edge is used for other equal triple
      if (std::any_of(triples.begin(), triples.end(), [this, &addr2](size_t const tripleIdx) {
            return m_triplesOrderUsedEdges[tripleIdx].find(addr2) != m_triplesOrderUsedEdges[tripleIdx].cend();
          }))
      {
        continue;
      }

      for (size_t const tripleIdx : triples)
      {
        triple = m_template.m_triples[tripleIdx];

        // check if all equal triples found to make a new search result item
        if (finishedTriplesCount == triples.size())
        {
          ++resultIdx;
          finishedTriplesCount = 0;
          result.m_results.emplace_back(currentResultAddrs);
          m_resultCheckedTriples.emplace_back(currentCheckedTriples);
        }
        else if (notFinishedTriplesCount == triples.size())
        {
          notFinishedTriplesCount = 0;
          result.m_results[resultIdx] = currentResultAddrs;
          m_resultCheckedTriples[resultIdx] = currentCheckedTriples;
        }

        if (m_resultCheckedTriples[resultIdx].find(tripleIdx) != m_resultCheckedTriples[resultIdx].cend())
        {
          continue;
        }

        item1 = (*triple)[0];
        item2 = (*triple)[1];
        item3 = (*triple)[2];

        // update data
        {
          UpdateResults(triple, resultIdx, addr1, addr2, addr3);
        }

        // find next depended on triples and analyse result
        {
          bool isChildFinished = false;
          bool isNoChild = false;

          // first of all check triples by edge, it is more effectively
          TryDoNextDependenceIteration(triple, item2, addr2, resultIdx, triples, isChildFinished, isNoChild);
          isAllChildrenFinished = isChildFinished;
          isLast = isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultIdx, result.m_results[resultIdx]);
            ++notFinishedTriplesCount;
            continue;
          }

          TryDoNextDependenceIteration(triple, item1, addr1, resultIdx, triples, isChildFinished, isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultIdx, result.m_results[resultIdx]);
            ++notFinishedTriplesCount;
            continue;
          }

          TryDoNextDependenceIteration(triple, item3, addr3, resultIdx, triples, isChildFinished, isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultIdx, result.m_results[resultIdx]);
            ++notFinishedTriplesCount;
            continue;
          }

          // all connected triples found
          if (isAllChildrenFinished)
          {
            ++finishedTriplesCount;

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
      if (isLast && isAllChildrenFinished && m_resultCheckedTriples[resultIdx].size() == m_template.m_triples.size())
      {
        FormResult(result, resultIdx);
      }
    }
  }

  void FormResult(ScTemplateSearchResult & result, size_t & resultIdx)
  {
    if (m_callback)
    {
      m_callback(ScTemplateSearchResultItem(&result.m_results[resultIdx], &result.m_replacements));
    }
    else if (m_callbackWithRequest)
    {
      ScTemplateSearchRequest const & request =
          m_callbackWithRequest(ScTemplateSearchResultItem(&result.m_results[resultIdx], &result.m_replacements));
      switch (request)
      {
      case ScTemplateSearchRequest::STOP:
      {
        isStopped = true;
        break;
      }
      case ScTemplateSearchRequest::ERROR:
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Requested error state during search");
      }
      default:
        break;
      }
    }
    else
    {
      m_foundResults.insert(resultIdx);
    }
  }

  void DoIterations(ScTemplateSearchResult & result)
  {
    if (m_template.m_triples.empty())
      return;

    auto const & DoStartIteration = [this, &result](ScTemplateGroupedTriples const & triples) {
      ScAddrVector newResult;
      newResult.resize(CalculateOneResultSize());
      result.m_results.reserve(16);
      result.m_results.emplace_back(newResult);

      m_resultCheckedTriples.reserve(16);
      m_resultCheckedTriples.emplace_back();

      bool isFinished = false;
      bool isLast = false;

      DoIterationOnNextEqualTriples(triples, "", 0, {}, result, isFinished, isLast);
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

public:
  ScTemplate::Result operator()(ScTemplateSearchResult & result)
  {
    result.Clear();
    m_triplesOrderUsedEdges.resize(CalculateOneResultSize());

    DoIterations(result);

    std::vector<ScAddrVector> checkedResults;
    checkedResults.reserve(result.Size());
    for (size_t const foundIdx : m_foundResults)
    {
      checkedResults.emplace_back(result.m_results[foundIdx]);
    }
    result.m_results = checkedResults;

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

  bool isStopped = false;
  ScAddr const m_structure;
  ScTemplateSearchResultCallback m_callback;
  ScTemplateSearchResultCallbackWithRequest m_callbackWithRequest;
  ScTemplateSearchResultCheckCallback m_checkCallback;

  std::map<std::string, ScTemplateGroupedTriples> m_itemsToTriples;
  ScTemplateGroupedTriples m_cycledTriples;
  ScTriplesOrderCheckedEdges m_triplesOrderUsedEdges;
  std::vector<std::unordered_set<size_t>> m_resultCheckedTriples;

  std::unordered_set<size_t> m_foundResults;
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

void ScTemplate::Search(
    ScMemoryContext & ctx,
    ScTemplateSearchResultCallbackWithRequest const & callback,
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
