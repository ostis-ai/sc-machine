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
  /*!
   * Prepares input sc-template to minimize search
   */
  void PrepareSearch()
  {
    SetUpDependenciesBetweenTriples();
    RemoveCycledDependenciesBetweenTriples();
    FindConnectivityComponents();
    FindTriplesWithMostMinimalArcsForFirstItem();
  }

  /*!
   * Find all dependencies between triples. Compares replacement name of each item of the triple
   * with replacement name of each item of the other triple, and if they are equal, then adds
   * dependencies between them.
   * @note All triple items that have valid address must have replacement names to set up dependencies with them.
   */
  void SetUpDependenciesBetweenTriples()
  {
    auto const & AddDependenceFromTripleItemToOtherTriple = [this](
                                                                ScTemplateTriple const * triple,
                                                                ScTemplateItemValue const & tripleItem,
                                                                ScTemplateTriple const * otherTriple) {
      std::string const & key = GetKey(triple, tripleItem);

      auto const & found = m_itemsToTriples.find(key);
      if (found == m_itemsToTriples.cend())
        m_itemsToTriples.insert({key, {otherTriple->m_index}});
      else
        found->second.insert(otherTriple->m_index);
    };

    auto const & TryAddDependenceBetweenTriples = [&AddDependenceFromTripleItemToOtherTriple](
                                                      ScTemplateTriple const * triple,
                                                      ScTemplateItemValue const & tripleItem,
                                                      ScTemplateTriple const * otherTriple,
                                                      ScTemplateItemValue const & otherTripleItem1,
                                                      ScTemplateItemValue const & otherTripleItem2,
                                                      ScTemplateItemValue const & otherTripleItem3) {
      // don't set up dependency with self
      if (triple->m_index == otherTriple->m_index)
        return;

      // don't set up dependency if item of triple has empty replacement name
      if (tripleItem.m_replacementName.empty())
        return;

      // check triple item name with other triple items names and dependencies
      tripleItem.m_replacementName == otherTripleItem1.m_replacementName
          ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
          : (tripleItem.m_replacementName == otherTripleItem2.m_replacementName
                 ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
                 : (tripleItem.m_replacementName == otherTripleItem3.m_replacementName
                        ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
                        : (void)(null_ptr)));
    };

    for (ScTemplateTriple const * triple : m_template.m_triples)
    {
      ScTemplateItemValue const & item1 = (*triple)[0];
      ScTemplateItemValue const & item2 = (*triple)[1];
      ScTemplateItemValue const & item3 = (*triple)[2];

      for (ScTemplateTriple const * otherTriple : m_template.m_triples)
      {
        ScTemplateItemValue const & otherItem1 = (*otherTriple)[0];
        ScTemplateItemValue const & otherItem2 = (*otherTriple)[1];
        ScTemplateItemValue const & otherItem3 = (*otherTriple)[2];

        TryAddDependenceBetweenTriples(triple, item1, otherTriple, otherItem1, otherItem2, otherItem3);
        TryAddDependenceBetweenTriples(triple, item2, otherTriple, otherItem1, otherItem2, otherItem3);
        TryAddDependenceBetweenTriples(triple, item3, otherTriple, otherItem1, otherItem2, otherItem3);
      }
    }
  };

  /*!
   * Finds triples that loop sc-template and eliminates transitions from them
   */
  void RemoveCycledDependenciesBetweenTriples()
  {
    auto const & CheckIfItemIsNodeVarStruct = [this](ScTemplateItemValue const & item) -> bool {
      auto const & found = m_template.m_namesToTypes.find(item.m_replacementName);
      return found != m_template.m_namesToTypes.cend() && found->second == ScType::NodeVarStruct;
    };

    auto const & faeTriples = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::FAN];
    auto const & CheckIfItemIsFixedAndOtherEdgeItemIsEdge =
        [&faeTriples](size_t const tripleIdx, ScTemplateItemValue const & item) -> bool {
      return item.IsAddr() && faeTriples.find(tripleIdx) != faeTriples.cend();
    };

    auto const & UpdateCycledTriples = [this](ScTemplateTriple const * triple, ScTemplateItemValue const & item) {
      std::string const & key = GetKey(triple, item);

      auto const & dependedTriples = m_itemsToTriples.find(key);
      if (dependedTriples != m_itemsToTriples.cend())
      {
        for (size_t const dependedTripleIdx : dependedTriples->second)
        {
          if (IsTriplesEqual(triple, m_template.m_triples[dependedTripleIdx]))
          {
            m_cycledTriples.insert(dependedTripleIdx);
          }
        }
      }

      m_cycledTriples.insert(triple->m_index);
    };

    // save all triples that form cycles
    for (ScTemplateTriple const * triple : m_template.m_triples)
    {
      ScTemplateItemValue const & item1 = (*triple)[0];

      bool isFound = false;
      if (m_cycledTriples.find(triple->m_index) == m_cycledTriples.cend() &&
          (CheckIfItemIsNodeVarStruct(item1) || CheckIfItemIsFixedAndOtherEdgeItemIsEdge(triple->m_index, item1)))
      {
        ScTemplateGroupedTriples checkedTriples;
        FindCycleWithFAATriple(item1, triple, triple, checkedTriples, isFound);
      }

      if (isFound)
      {
        UpdateCycledTriples(triple, item1);
      }
    }

    // remove dependencies with all triples that form cycles
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

  void FindCycleWithFAATriple(
      ScTemplateItemValue const & item,
      ScTemplateTriple const * triple,
      ScTemplateTriple const * tripleToFind,
      ScTemplateGroupedTriples checkedTriples,
      bool & isFound)
  {
    // no iterate more if cycle is found
    if (isFound)
      return;

    auto const & FindCycleWithFAATripleByTripleItem = [this, &tripleToFind, &checkedTriples](
                                                          ScTemplateItemValue const & item,
                                                          ScTemplateTriple const * triple,
                                                          ScTemplateItemValue const & previousItem,
                                                          bool & isFound) {
      // no iterate back by the same item replacement name
      if (!item.m_replacementName.empty() && item.m_replacementName == previousItem.m_replacementName)
        return;

      // no iterate back by the same item address
      if (item.m_addrValue.IsValid() && item.m_addrValue == previousItem.m_addrValue)
        return;

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

      // check if triple was passed in branch of sc-template
      if (checkedTriples.find(otherTripleIdx) != checkedTriples.cend())
        continue;

      // iterate by all triple items
      {
        checkedTriples.insert(otherTripleIdx);

        FindCycleWithFAATripleByTripleItem((*otherTriple)[0], otherTriple, item, isFound);
        FindCycleWithFAATripleByTripleItem((*otherTriple)[1], otherTriple, item, isFound);
        FindCycleWithFAATripleByTripleItem((*otherTriple)[2], otherTriple, item, isFound);
      }
    }
  }

  void FindConnectivityComponents()
  {
    ScTemplateGroupedTriples checkedTriples;

    for (ScTemplateTriple const * triple : m_template.m_triples)
    {
      ScTemplateGroupedTriples connectivityComponentTriples;
      FindConnectivityComponent(triple, checkedTriples, connectivityComponentTriples);

      m_connectivityComponentsTriples.push_back(connectivityComponentTriples);
    }
  }

  void FindConnectivityComponent(
      ScTemplateTriple const * triple,
      ScTemplateGroupedTriples & checkedTriples,
      ScTemplateGroupedTriples & connectivityComponentTriples)
  {
    // check if triple was passed in branch of sc-template
    if (checkedTriples.find(triple->m_index) != checkedTriples.cend())
      return;

    connectivityComponentTriples.insert(triple->m_index);

    FindConnectivityComponentByItem((*triple)[0], triple, checkedTriples, connectivityComponentTriples);
    FindConnectivityComponentByItem((*triple)[1], triple, checkedTriples, connectivityComponentTriples);
    FindConnectivityComponentByItem((*triple)[2], triple, checkedTriples, connectivityComponentTriples);
  }

  void FindConnectivityComponentByItem(
      ScTemplateItemValue const & item,
      ScTemplateTriple const * triple,
      ScTemplateGroupedTriples & checkedTriples,
      ScTemplateGroupedTriples & connectivityComponentTriples)
  {
    ScTemplateGroupedTriples nextTriples;
    FindDependedTriple(item, triple, nextTriples);

    for (size_t const otherTripleIdx : nextTriples)
    {
      // check if triple was passed in branch of sc-template
      if (checkedTriples.find(otherTripleIdx) != checkedTriples.cend())
        continue;

      // iterate by all triple items
      {
        checkedTriples.insert(otherTripleIdx);
        connectivityComponentTriples.insert(otherTripleIdx);

        ScTemplateTriple const * otherTriple = m_template.m_triples[otherTripleIdx];

        FindConnectivityComponentByItem((*otherTriple)[0], otherTriple, checkedTriples, connectivityComponentTriples);
        FindConnectivityComponentByItem((*otherTriple)[1], otherTriple, checkedTriples, connectivityComponentTriples);
        FindConnectivityComponentByItem((*otherTriple)[2], otherTriple, checkedTriples, connectivityComponentTriples);
      }
    }
  }

  /*!
   * Finds all connectivity component triples among all triples that have the fixed first item, but not fixed
   * other items, for which the minimum number of arcs goes/incomes out of the first item compared to the other triples.
   */
  void FindTriplesWithMostMinimalArcsForFirstItem()
  {
    for (ScTemplateGroupedTriples const & connectivityComponentsTriples : m_connectivityComponentsTriples)
    {
      sc_int32 priorityTripleIdx = -1;
      auto & afaTriples = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::AFA];
      if (!afaTriples.empty())
      {
        priorityTripleIdx = (sc_int32)*afaTriples.cbegin();
      }

      if (priorityTripleIdx == -1)
      {
        priorityTripleIdx = FindTripleWithMostMinimalInputArcsForFirstItem(connectivityComponentsTriples);
        if (priorityTripleIdx == -1)
        {
          priorityTripleIdx = FindTripleWithMostMinimalOutputArcsForFirstItem(connectivityComponentsTriples);
        }
      }

      // save triple in which the first item address has the most minimal count of input/output arcs in vector
      // with more priority
      if (priorityTripleIdx != -1)
      {
        m_connectivityComponentPriorityTriples.emplace_back(priorityTripleIdx);
      }
    }
  }

  sc_int32 FindTripleWithMostMinimalInputArcsForFirstItem(
      ScTemplateGroupedTriples const & connectivityComponentsTriples)
  {
    auto triplesWithConstEndElement = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::FAF];
    if (triplesWithConstEndElement.empty())
    {
      triplesWithConstEndElement = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::AAF];
    }

    // find triple in which the third item address has the most minimal count of input arcs
    sc_int32 priorityTripleIdx = -1;
    sc_int32 minInputArcsCount = -1;
    for (size_t const tripleIdx : triplesWithConstEndElement)
    {
      ScTemplateTriple const * triple = m_template.m_triples[tripleIdx];
      auto const count = (sc_int32)m_context.GetElementInputArcsCount(triple->GetValues()[2].m_addrValue);

      // check if triple in connectivity component
      if (connectivityComponentsTriples.find(tripleIdx) == connectivityComponentsTriples.cend())
        continue;

      if (minInputArcsCount == -1 || count < minInputArcsCount)
      {
        priorityTripleIdx = (sc_int32)tripleIdx;
        minInputArcsCount = (sc_int32)count;
      }
    }

    return priorityTripleIdx;
  }

  sc_int32 FindTripleWithMostMinimalOutputArcsForFirstItem(
      ScTemplateGroupedTriples const & connectivityComponentsTriples)
  {
    auto triplesWithConstBeginElement = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::FAE];
    // if there are no triples with the no edge third item than sort triples with the edge third item
    if (triplesWithConstBeginElement.empty())
    {
      triplesWithConstBeginElement = m_template.m_orderedTriples[(size_t)ScTemplateTripleType::FAN];
    }

    // find triple in which the first item address has the most minimal count of output arcs
    sc_int32 priorityTripleIdx = -1;
    sc_int32 minOutputArcsCount = -1;
    for (size_t const tripleIdx : triplesWithConstBeginElement)
    {
      ScTemplateTriple const * triple = m_template.m_triples[tripleIdx];
      auto const count = (sc_int32)m_context.GetElementOutputArcsCount(triple->GetValues()[0].m_addrValue);

      // check if triple in connectivity component
      if (connectivityComponentsTriples.find(tripleIdx) == connectivityComponentsTriples.cend())
        continue;

      if (minOutputArcsCount == -1 || count < minOutputArcsCount)
      {
        priorityTripleIdx = (sc_int32)tripleIdx;
        minOutputArcsCount = (sc_int32)count;
      }
    }

    return priorityTripleIdx;
  }

  //! Returns key - "${item replacement name}${triple index}"
  static std::string GetKey(ScTemplateTriple const * triple, ScTemplateItemValue const & item)
  {
    std::ostringstream stream;
    stream << item.m_replacementName << "_" << triple->m_index;
    return stream.str();
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
      return true;

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
            isEqual = item.m_typeValue == found->second;
        }
        else
          isEqual = found->second == otherItem.m_typeValue;
      }

      if (isEqual)
        isEqual = item.m_addrValue == otherItem.m_addrValue;

      if (!isEqual)
      {
        auto found = m_template.m_namesToAddrs.find(item.m_replacementName);
        if (found == m_template.m_namesToAddrs.cend())
        {
          found = m_template.m_namesToAddrs.find(item.m_replacementName);
          if (found != m_template.m_namesToAddrs.cend())
            isEqual = item.m_addrValue == found->second;
        }
        else
          isEqual = found->second == otherItem.m_addrValue;
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
    auto const & GetItemAddrInReplacements = [&resultAddrs,
                                              &result](ScTemplateItemValue const & item) -> ScAddr const & {
      auto const & it = result.m_replacements.equal_range(item.m_replacementName);
      for (auto curIt = it.first; curIt != it.second; ++curIt)
      {
        ScAddr const & addr = resultAddrs[curIt->second];
        if (addr.IsValid())
          return addr;
      }

      return ScAddr::Empty;
    };

    switch (item.m_itemType)
    {
    case ScTemplateItemValue::Type::Addr:
    {
      return item.m_addrValue;
    }

    case ScTemplateItemValue::Type::Replace:
    {
      ScAddr const & replacementAddr = GetItemAddrInReplacements(item);
      if (replacementAddr.IsValid())
        return replacementAddr;

      auto const & addrsIt = m_template.m_namesToAddrs.find(item.m_replacementName);
      if (addrsIt != m_template.m_namesToAddrs.cend())
        return addrsIt->second;

      return ScAddr::Empty;
    }

    case ScTemplateItemValue::Type::Type:
    {
      if (!item.m_replacementName.empty())
      {
        return GetItemAddrInReplacements(item);
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
    ScAddr const & addr1 = ResolveAddr(item1, resultAddrs, result);
    ScAddr const & addr2 = ResolveAddr(item2, resultAddrs, result);
    ScAddr const & addr3 = ResolveAddr(item3, resultAddrs, result);

    auto const & PrepareType = [](ScType const & type) {
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

  using UsedEdges = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;

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

        if (!isFinished)
        {
          break;
        }
      }
    }
  }

  using UnusedTriples = std::unordered_map<ScAddr, std::array<ScAddr, 3>, ScAddrHashFunc<size_t>, ScAddLessFunc>;

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
                                     ScAddr const & addr3,
                                     UnusedTriples & unusedTriples) {
      m_resultCheckedTriples[resultIdx].insert(triple->m_index);
      m_resultUsedEdges[resultIdx].insert(addr2);
      unusedTriples.erase(addr2);

      size_t itemIdx = triple->m_index * 3;

      for (size_t i = resultIdx; i < result.Size(); ++i)
      {
        ScAddrVector & resultAddrs = result.m_results[i];

        UpdateItemResults((*triple)[0], addr1, itemIdx, resultAddrs);
        UpdateItemResults((*triple)[1], addr2, itemIdx + 1, resultAddrs);
        UpdateItemResults((*triple)[2], addr3, itemIdx + 2, resultAddrs);
      }
    };

    auto const & ClearResults =
        [this](
            size_t const tripleIdx, size_t const resultIdx, ScAddrVector & resultAddrs, UnusedTriples & unusedTriples) {
          m_resultCheckedTriples[resultIdx].erase(tripleIdx);

          size_t itemIdx = tripleIdx * 3;

          unusedTriples.insert(
              {resultAddrs[itemIdx], {resultAddrs[itemIdx], resultAddrs[itemIdx + 1], resultAddrs[itemIdx + 2]}});

          resultAddrs[itemIdx] = ScAddr::Empty;
          m_resultUsedEdges[resultIdx].erase(resultAddrs[++itemIdx]);
          resultAddrs[itemIdx] = ScAddr::Empty;
          resultAddrs[++itemIdx] = ScAddr::Empty;
        };

    auto const & CheckTripleForVarTriple = [this, &result](
                                               ScAddr const & addr1,
                                               ScAddr const & addr2,
                                               ScAddr const & addr3,
                                               ScTemplateItemValue const & item1,
                                               ScTemplateItemValue const & item2,
                                               ScTemplateItemValue const & item3,
                                               ScAddrVector & resultAddrs,
                                               UnusedTriples & unusedTriples) {
      ScAddr const & resolvedAddr1 = ResolveAddr(item1, resultAddrs, result);
      if (resolvedAddr1.IsValid() && resolvedAddr1 != addr1)
      {
        unusedTriples.insert({addr2, {addr1, addr2, addr3}});
        return false;
      }

      ScAddr const & resolvedAddr2 = ResolveAddr(item2, resultAddrs, result);
      if (resolvedAddr2.IsValid() && resolvedAddr2 != addr2)
      {
        unusedTriples.insert({addr2, {addr1, addr2, addr3}});
        return false;
      }

      ScAddr const & resolvedAddr3 = ResolveAddr(item3, resultAddrs, result);
      if (resolvedAddr3.IsValid() && resolvedAddr3 != addr3)
      {
        unusedTriples.insert({addr2, {addr1, addr2, addr3}});
        return false;
      }

      return true;
    };

    ScTemplateTriple * triple = m_template.m_triples[*triples.begin()];

    bool isAllChildrenFinished = true;
    bool isLast = false;

    ScTemplateItemValue item1 = (*triple)[0];
    ScTemplateItemValue item2 = (*triple)[1];
    ScTemplateItemValue item3 = (*triple)[2];

    ScIterator3Ptr it = CreateIterator(item1, item2, item3, result.m_results[resultIdx], result);
    if (!it || !it->IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "During search procedure has been chosen var triple");
    }

    size_t unusedTriplesCount = 0;
    size_t usedTriplesCount = 0;
    size_t allUsedTriplesCountForIteratorCycle = 0;

    ScAddrVector currentResultAddrs{result.m_results[resultIdx]};
    ScTemplateGroupedTriples currentCheckedTriples{m_resultCheckedTriples[resultIdx]};
    UsedEdges currentUsedEdges{m_resultUsedEdges[resultIdx]};

    ScAddrVector nextResultAddrs{result.m_results[resultIdx]};
    ScTemplateGroupedTriples nextCheckedTriples{m_resultCheckedTriples[resultIdx]};
    UsedEdges nextUsedEdges{m_resultUsedEdges[resultIdx]};

    UnusedTriples restoreUnusedTriples;
    UnusedTriples unusedTriples;

    bool isIteratorNext = false;
    bool isUnusedIteratorNext = false;
    UnusedTriples::const_iterator unusedTriplesIt;
    do
    {
      ScAddr addr1;
      ScAddr addr2;
      ScAddr addr3;

      isIteratorNext = it->Next();
      if (isIteratorNext)
      {
        addr1 = it->Get(0);
        addr2 = it->Get(1);
        addr3 = it->Get(2);
      }
      else
      {
        if (allUsedTriplesCountForIteratorCycle == 0)
        {
          break;
        }

        if (unusedTriplesIt == unusedTriples.cend())
        {
          isUnusedIteratorNext = false;
        }

        if (!isUnusedIteratorNext)
        {
          isUnusedIteratorNext = true;
          allUsedTriplesCountForIteratorCycle = 0;
          unusedTriples = restoreUnusedTriples;
          restoreUnusedTriples.clear();
          unusedTriplesIt = unusedTriples.cbegin();
        }
        else
        {
          ++unusedTriplesIt;
        }

        if (unusedTriplesIt == unusedTriples.cend())
        {
          break;
        }

        addr1 = unusedTriplesIt->second[0];
        addr2 = unusedTriplesIt->second[1];
        addr3 = unusedTriplesIt->second[2];
      }

      // check triple elements by structure belonging or predicate callback
      if ((IsStructureValid() && (!IsInStructure(addr1) || !IsInStructure(addr2) || !IsInStructure(addr3))) ||
          (m_checkCallback && !m_checkCallback(addr1, addr2, addr3)))
      {
        m_resultUsedEdges[resultIdx].insert(addr2);
        continue;
      }

      // check if edge is used for other equal triple
      if (std::any_of(m_resultUsedEdges.begin(), m_resultUsedEdges.end(), [&](UsedEdges const & edges) {
            return edges.find(addr2) != edges.cend();
          }))
      {
        continue;
      }

      for (size_t const tripleIdx : triples)
      {
        triple = m_template.m_triples[tripleIdx];

        // check if all equal triples found to make a new search result item
        if (usedTriplesCount == triples.size())
        {
          resultIdx = ++m_lastResultIdx;
          ++allUsedTriplesCountForIteratorCycle;
          usedTriplesCount = 0;
          result.m_results.emplace_back(nextResultAddrs);
          m_resultCheckedTriples.emplace_back(nextCheckedTriples);
          m_resultUsedEdges.emplace_back(nextUsedEdges);
        }
        else if (!isAllChildrenFinished)
        {
          result.m_results[resultIdx] = currentResultAddrs;
          m_resultCheckedTriples[resultIdx] = currentCheckedTriples;
          m_resultUsedEdges[resultIdx] = currentUsedEdges;
        }
        if (unusedTriplesCount == triples.size())
        {
          unusedTriplesCount = 0;
          currentResultAddrs = nextResultAddrs;
          currentCheckedTriples = nextCheckedTriples;
          currentUsedEdges = nextUsedEdges;
        }

        if (m_resultCheckedTriples[resultIdx].find(tripleIdx) != m_resultCheckedTriples[resultIdx].cend())
        {
          continue;
        }

        if (!CheckTripleForVarTriple(
                addr1, addr2, addr3, item1, item2, item3, result.m_results[resultIdx], unusedTriples))
        {
          continue;
        }

        item1 = (*triple)[0];
        item2 = (*triple)[1];
        item3 = (*triple)[2];

        // update data
        {
          UpdateResults(triple, resultIdx, addr1, addr2, addr3, restoreUnusedTriples);
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
            ClearResults(tripleIdx, resultIdx, result.m_results[resultIdx], restoreUnusedTriples);
            ++unusedTriplesCount;
            continue;
          }

          TryDoNextDependenceIteration(triple, item1, addr1, resultIdx, triples, isChildFinished, isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultIdx, result.m_results[resultIdx], restoreUnusedTriples);
            ++unusedTriplesCount;
            continue;
          }

          TryDoNextDependenceIteration(triple, item3, addr3, resultIdx, triples, isChildFinished, isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(tripleIdx, resultIdx, result.m_results[resultIdx], restoreUnusedTriples);
            ++unusedTriplesCount;
            continue;
          }

          // all connected triples found
          if (isAllChildrenFinished)
          {
            ++usedTriplesCount;

            // current edge is busy for all equal triples
            m_resultUsedEdges[resultIdx].insert(addr2);
            m_usedEdges.insert(addr2);

            currentResultAddrs = result.m_results[resultIdx];
            currentCheckedTriples = m_resultCheckedTriples[resultIdx];
            currentUsedEdges = m_resultUsedEdges[resultIdx];

            break;
          }
        }
      }

      // there are no next triples for current triple, it is last
      if (isLast && isAllChildrenFinished && m_resultCheckedTriples[resultIdx].size() == m_template.m_triples.size())
      {
        FormResult(result, resultIdx);
      }
    } while ((isIteratorNext || unusedTriplesIt != unusedTriples.cend()) && !isStopped);
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

    ScAddrVector newResult;
    newResult.resize(CalculateOneResultSize());
    result.m_results.reserve(16);
    result.m_results.emplace_back(newResult);

    m_resultUsedEdges.reserve(16);
    m_resultUsedEdges.emplace_back();
    m_resultCheckedTriples.reserve(16);
    m_resultCheckedTriples.emplace_back();

    bool isFinished = false;
    bool isLast = false;

    for (size_t const tripleIdx : m_connectivityComponentPriorityTriples)
    {
      DoIterationOnNextEqualTriples({tripleIdx}, "", 0, {}, result, isFinished, isLast);
    }
  }

public:
  ScTemplate::Result operator()(ScTemplateSearchResult & result)
  {
    result.Clear();

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
  std::vector<ScTemplateGroupedTriples> m_connectivityComponentsTriples;
  std::vector<size_t> m_connectivityComponentPriorityTriples;

  std::vector<UsedEdges> m_resultUsedEdges;
  UsedEdges m_usedEdges;
  std::vector<std::unordered_set<size_t>> m_resultCheckedTriples;

  size_t m_lastResultIdx = 0;
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
