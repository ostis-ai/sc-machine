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
  ScTemplateSearch(ScTemplate & templ, ScMemoryContext & context, ScAddr const & structure)
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
  {
    PrepareSearch();
  }

  using ScTemplateTriples = ScTemplate::ScTemplateGroupedTriples;
  using ScReplacementTriple = ScAddrTriple;

  void SetCallbackWithRequest(ScTemplateSearchResultCallbackWithRequest const & callback)
  {
    m_callbackWithRequest = callback;
  }

  void SetCallback(ScTemplateSearchResultCallback const & callback)
  {
    m_callback = callback;
  }

  void SetFilterCallback(ScTemplateSearchResultFilterCallback const & filterCallback)
  {
    m_filterCallback = filterCallback;
  }

  void SetCheckCallback(ScTemplateSearchResultCheckCallback const & checkCallback)
  {
    m_checkCallback = checkCallback;
  }

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
    auto const & AddDependenceFromTripleItemToOtherTriple =
        [this](
            ScTemplateTriple const * triple, ScTemplateItem const & tripleItem, ScTemplateTriple const * otherTriple) {
          std::string const & key = GetKey(triple, tripleItem);

          auto const & found = m_templateItemsNamesToDependedTemplateTriples.find(key);
          if (found == m_templateItemsNamesToDependedTemplateTriples.cend())
            m_templateItemsNamesToDependedTemplateTriples.insert({key, {otherTriple->m_index}});
          else
            found->second.insert(otherTriple->m_index);
        };

    auto const & TryAddDependenceBetweenTriples = [&AddDependenceFromTripleItemToOtherTriple](
                                                      ScTemplateTriple const * triple,
                                                      ScTemplateItem const & tripleItem,
                                                      ScTemplateTriple const * otherTriple,
                                                      ScTemplateItem const & otherTripleItem1,
                                                      ScTemplateItem const & otherTripleItem2,
                                                      ScTemplateItem const & otherTripleItem3) {
      // don't set up dependency with self
      if (triple->m_index == otherTriple->m_index)
        return;

      // don't set up dependency if item of triple has empty replacement name
      if (tripleItem.m_name.empty())
        return;

      // check triple item name with other triple items names and dependencies
      tripleItem.m_name == otherTripleItem1.m_name
          ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
          : (tripleItem.m_name == otherTripleItem2.m_name
                 ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
                 : (tripleItem.m_name == otherTripleItem3.m_name
                        ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
                        : (void)(null_ptr)));
    };

    for (ScTemplateTriple const * triple : m_template.m_templateTriples)
    {
      ScTemplateItem const & item1 = (*triple)[0];
      ScTemplateItem const & item2 = (*triple)[1];
      ScTemplateItem const & item3 = (*triple)[2];

      for (ScTemplateTriple const * otherTriple : m_template.m_templateTriples)
      {
        ScTemplateItem const & otherItem1 = (*otherTriple)[0];
        ScTemplateItem const & otherItem2 = (*otherTriple)[1];
        ScTemplateItem const & otherItem3 = (*otherTriple)[2];

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
    auto const & CheckIfItemIsNodeVarStruct = [this](ScTemplateItem const & item) -> bool {
      auto const & found = m_template.m_templateItemsNamesToTypes.find(item.m_name);
      return found != m_template.m_templateItemsNamesToTypes.cend() && found->second == ScType::NodeVarStruct;
    };

    auto const & faeTriples = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAE];
    auto const & CheckIfItemIsFixedAndOtherEdgeItemIsEdge =
        [&faeTriples](size_t const tripleIdx, ScTemplateItem const & item) -> bool {
      return item.IsAddr() && faeTriples.find(tripleIdx) != faeTriples.cend();
    };

    auto const & UpdateCycledTriples = [this](ScTemplateTriple const * triple, ScTemplateItem const & item) {
      std::string const & key = GetKey(triple, item);

      auto const & dependedTriples = m_templateItemsNamesToDependedTemplateTriples.find(key);
      if (dependedTriples != m_templateItemsNamesToDependedTemplateTriples.cend())
      {
        for (size_t const dependedTripleIdx : dependedTriples->second)
        {
          if (IsTriplesEqual(triple, m_template.m_templateTriples[dependedTripleIdx]))
            m_cycledTemplateTriples.insert(dependedTripleIdx);
        }
      }

      m_cycledTemplateTriples.insert(triple->m_index);
    };

    // save all triples that form cycles
    for (ScTemplateTriple const * triple : m_template.m_templateTriples)
    {
      ScTemplateItem const & item1 = (*triple)[0];

      bool isFound = false;
      if (m_cycledTemplateTriples.find(triple->m_index) == m_cycledTemplateTriples.cend() &&
          (CheckIfItemIsNodeVarStruct(item1) || CheckIfItemIsFixedAndOtherEdgeItemIsEdge(triple->m_index, item1)))
      {
        ScTemplateTriples checkedTriples;
        FindCycleWithFAATriple(item1, triple, triple, checkedTriples, isFound);
      }

      if (isFound)
      {
        UpdateCycledTriples(triple, item1);
      }
    }

    // remove dependencies with all triples that form cycles
    for (size_t const idx : m_cycledTemplateTriples)
    {
      ScTemplateTriple * triple = m_template.m_templateTriples[idx];
      std::string const & key = GetKey(triple, (*triple)[0]);

      auto const & found = m_templateItemsNamesToDependedTemplateTriples.find(key);
      if (found != m_templateItemsNamesToDependedTemplateTriples.cend())
      {
        for (size_t const otherIdx : m_cycledTemplateTriples)
        {
          found->second.erase(otherIdx);
        }
      }
    }
  };

  void FindCycleWithFAATriple(
      ScTemplateItem const & templateItem,
      ScTemplateTriple const * templateTriple,
      ScTemplateTriple const * templateTripleToFind,
      ScTemplateTriples checkedTemplateTriples,
      bool & isFound)
  {
    // no iterate more if cycle is found
    if (isFound)
      return;

    auto const & FindCycleWithFAATripleByTripleItem = [this, &templateTripleToFind, &checkedTemplateTriples](
                                                          ScTemplateItem const & item,
                                                          ScTemplateTriple const * triple,
                                                          ScTemplateItem const & previousItem,
                                                          bool & isFound) {
      // no iterate back by the same item name
      if (!item.m_name.empty() && item.m_name == previousItem.m_name)
        return;

      // no iterate back by the same item address
      if (item.m_addrValue.IsValid() && item.m_addrValue == previousItem.m_addrValue)
        return;

      FindCycleWithFAATriple(item, triple, templateTripleToFind, checkedTemplateTriples, isFound);
    };

    ScTemplateTriples nextTemplateTriples;
    FindDependedTriple(templateItem, templateTriple, nextTemplateTriples);

    for (size_t const otherTemplateTripleIdx : nextTemplateTriples)
    {
      ScTemplateTriple const * otherTriple = m_template.m_templateTriples[otherTemplateTripleIdx];

      if ((otherTemplateTripleIdx == templateTripleToFind->m_index &&
           templateItem.m_name != (*templateTripleToFind)[0].m_name) ||
          isFound)
      {
        isFound = true;
        break;
      }

      // check if triple was passed in branch of sc-template
      if (checkedTemplateTriples.find(otherTemplateTripleIdx) != checkedTemplateTriples.cend())
        continue;

      // iterate by all triple items
      {
        checkedTemplateTriples.insert(otherTemplateTripleIdx);

        FindCycleWithFAATripleByTripleItem((*otherTriple)[0], otherTriple, templateItem, isFound);
        FindCycleWithFAATripleByTripleItem((*otherTriple)[1], otherTriple, templateItem, isFound);
        FindCycleWithFAATripleByTripleItem((*otherTriple)[2], otherTriple, templateItem, isFound);
      }
    }
  }

  void FindConnectivityComponents()
  {
    ScTemplateTriples checkedTriples;

    for (ScTemplateTriple const * triple : m_template.m_templateTriples)
    {
      ScTemplateTriples connectivityComponentTriples;
      FindConnectivityComponent(triple, checkedTriples, connectivityComponentTriples);

      m_connectivityComponentsTemplateTriples.push_back(connectivityComponentTriples);
    }
  }

  void FindConnectivityComponent(
      ScTemplateTriple const * templateTriple,
      ScTemplateTriples & checkedTemplateTriples,
      ScTemplateTriples & connectivityComponentTemplateTriples)
  {
    // check if triple was passed in branch of sc-template
    if (checkedTemplateTriples.find(templateTriple->m_index) != checkedTemplateTriples.cend())
      return;

    connectivityComponentTemplateTriples.insert(templateTriple->m_index);

    FindConnectivityComponentByItem(
        (*templateTriple)[0], templateTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
    FindConnectivityComponentByItem(
        (*templateTriple)[1], templateTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
    FindConnectivityComponentByItem(
        (*templateTriple)[2], templateTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
  }

  void FindConnectivityComponentByItem(
      ScTemplateItem const & templateItem,
      ScTemplateTriple const * templateTriple,
      ScTemplateTriples & checkedTemplateTriples,
      ScTemplateTriples & connectivityComponentTemplateTriples)
  {
    ScTemplateTriples nextTriples;
    FindDependedTriple(templateItem, templateTriple, nextTriples);

    for (size_t const otherTripleIdx : nextTriples)
    {
      // check if triple was passed in branch of sc-template
      if (checkedTemplateTriples.find(otherTripleIdx) != checkedTemplateTriples.cend())
        continue;

      // iterate by all triple items
      {
        checkedTemplateTriples.insert(otherTripleIdx);
        connectivityComponentTemplateTriples.insert(otherTripleIdx);

        ScTemplateTriple const * otherTriple = m_template.m_templateTriples[otherTripleIdx];

        FindConnectivityComponentByItem(
            (*otherTriple)[0], otherTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
        FindConnectivityComponentByItem(
            (*otherTriple)[1], otherTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
        FindConnectivityComponentByItem(
            (*otherTriple)[2], otherTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
      }
    }
  }

  /*!
   * Finds all connectivity component triples among all triples that have the fixed first item, but not fixed
   * other items, for which the minimum number of arcs goes/incomes out of the first item compared to the other triples.
   */
  void FindTriplesWithMostMinimalArcsForFirstItem()
  {
    for (ScTemplateTriples const & connectivityComponentsTriples : m_connectivityComponentsTemplateTriples)
    {
      sc_int32 priorityTripleIdx = -1;
      auto & afaTriples = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::AFA];
      if (!afaTriples.empty())
        priorityTripleIdx = (sc_int32)*afaTriples.cbegin();

      if (priorityTripleIdx == -1)
      {
        priorityTripleIdx = FindTripleWithMostMinimalInputArcsForThirdItem(connectivityComponentsTriples);
        if (priorityTripleIdx == -1)
          priorityTripleIdx = FindTripleWithMostMinimalOutputArcsForFirstItem(connectivityComponentsTriples);
      }

      // save triple in which the first item address has the most minimal count of input/output arcs in vector
      // with more priority
      if (priorityTripleIdx != -1)
      {
        m_connectivityComponentPriorityTemplateTriples.insert(priorityTripleIdx);
      }
    }
  }

  sc_int32 FindTripleWithMostMinimalInputArcsForThirdItem(ScTemplateTriples const & connectivityComponentsTriples)
  {
    auto triplesWithConstEndElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAF];
    if (triplesWithConstEndElement.empty())
    {
      triplesWithConstEndElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::AAF];
    }

    // find triple in which the third item address has the most minimal count of input arcs
    sc_int32 priorityTripleIdx = -1;
    sc_int32 minInputArcsCount = -1;
    for (size_t const tripleIdx : triplesWithConstEndElement)
    {
      // check if triple in connectivity component
      if (connectivityComponentsTriples.find(tripleIdx) == connectivityComponentsTriples.cend())
        continue;

      ScTemplateTriple const * triple = m_template.m_templateTriples[tripleIdx];
      auto const count = (sc_int32)m_context.GetElementInputArcsCount(triple->GetValues()[2].m_addrValue);

      if (minInputArcsCount == -1 || count < minInputArcsCount)
      {
        priorityTripleIdx = (sc_int32)tripleIdx;
        minInputArcsCount = (sc_int32)count;
      }
    }

    return priorityTripleIdx;
  }

  sc_int32 FindTripleWithMostMinimalOutputArcsForFirstItem(ScTemplateTriples const & connectivityComponentsTriples)
  {
    auto triplesWithConstBeginElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAN];
    // if there are no triples with the no edge third item than sort triples with the edge third item
    if (triplesWithConstBeginElement.empty())
      triplesWithConstBeginElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAE];

    // find triple in which the first item address has the most minimal count of output arcs
    sc_int32 priorityTripleIdx = -1;
    sc_int32 minOutputArcsCount = -1;
    for (size_t const tripleIdx : triplesWithConstBeginElement)
    {
      // check if triple in connectivity component
      if (connectivityComponentsTriples.find(tripleIdx) == connectivityComponentsTriples.cend())
        continue;

      ScTemplateTriple const * triple = m_template.m_templateTriples[tripleIdx];
      auto const count = (sc_int32)m_context.GetElementOutputArcsCount(triple->GetValues()[0].m_addrValue);

      if (minOutputArcsCount == -1 || count < minOutputArcsCount)
      {
        priorityTripleIdx = (sc_int32)tripleIdx;
        minOutputArcsCount = (sc_int32)count;
      }
    }

    return priorityTripleIdx;
  }

  //! Returns key - "${item replacement name}${triple index}"
  static std::string GetKey(ScTemplateTriple const * triple, ScTemplateItem const & item)
  {
    std::ostringstream stream;
    stream << item.m_name << "_" << triple->m_index;
    return stream.str();
  }

  void FindDependedTriple(ScTemplateItem const & item, ScTemplateTriple const * triple, ScTemplateTriples & nextTriples)
  {
    if (item.m_name.empty())
      return;

    std::string const & key = GetKey(triple, item);
    auto const & found = m_templateItemsNamesToDependedTemplateTriples.find(key);
    if (found != m_templateItemsNamesToDependedTemplateTriples.cend())
      nextTriples = found->second;
  }

  bool IsTriplesEqual(
      ScTemplateTriple const * templateTriple,
      ScTemplateTriple const * otherTemplateTriple,
      std::string const & itemName = "")
  {
    if (templateTriple->m_index == otherTemplateTriple->m_index)
      return true;

    auto const & tripleValues = templateTriple->GetValues();
    auto const & otherTripleValues = otherTemplateTriple->GetValues();

    auto const & IsTriplesItemsEqual = [this](ScTemplateItem const & item, ScTemplateItem const & otherItem) -> bool {
      bool isEqual = item.m_typeValue == otherItem.m_typeValue;
      if (!isEqual)
      {
        auto found = m_template.m_templateItemsNamesToTypes.find(item.m_name);
        if (found == m_template.m_templateItemsNamesToTypes.cend())
        {
          found = m_template.m_templateItemsNamesToTypes.find(otherItem.m_name);
          if (found != m_template.m_templateItemsNamesToTypes.cend())
            isEqual = item.m_typeValue == found->second;
        }
        else
          isEqual = found->second == otherItem.m_typeValue;
      }

      if (isEqual)
        isEqual = item.m_addrValue == otherItem.m_addrValue;

      if (!isEqual)
      {
        auto found = m_template.m_templateItemsNamesToReplacementItemsAddrs.find(item.m_name);
        if (found == m_template.m_templateItemsNamesToReplacementItemsAddrs.cend())
        {
          found = m_template.m_templateItemsNamesToReplacementItemsAddrs.find(otherItem.m_name);
          if (found != m_template.m_templateItemsNamesToReplacementItemsAddrs.cend())
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
           ((tripleValues[0].m_name == otherTripleValues[0].m_name &&
             (itemName.empty() || otherTripleValues[0].m_name == itemName)) ||
            (tripleValues[2].m_name == otherTripleValues[2].m_name &&
             (itemName.empty() || otherTripleValues[0].m_name == itemName)));
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
      ScTemplateItem const & templateItem,
      ScAddrVector const & replacementConstruction,
      ScTemplateSearchResult & result) const
  {
    auto const & GetItemAddrInReplacements = [&replacementConstruction,
                                              &result](ScTemplateItem const & item) -> ScAddr const & {
      auto const & it = result.m_templateItemsNamesToReplacementItemsPositions.find(item.m_name);
      if (it != result.m_templateItemsNamesToReplacementItemsPositions.cend())
      {
        ScAddr const & addr = replacementConstruction[it->second];
        if (addr.IsValid())
          return addr;
      }

      return ScAddr::Empty;
    };

    switch (templateItem.m_itemType)
    {
    case ScTemplateItem::Type::Addr:
    {
      return templateItem.m_addrValue;
    }

    case ScTemplateItem::Type::Replace:
    {
      ScAddr const & replacementAddr = GetItemAddrInReplacements(templateItem);
      if (replacementAddr.IsValid())
        return replacementAddr;

      auto const & addrsIt = m_template.m_templateItemsNamesToReplacementItemsAddrs.find(templateItem.m_name);
      if (addrsIt != m_template.m_templateItemsNamesToReplacementItemsAddrs.cend())
        return addrsIt->second;

      return ScAddr::Empty;
    }

    case ScTemplateItem::Type::Type:
    {
      if (!templateItem.m_name.empty())
      {
        return GetItemAddrInReplacements(templateItem);
      }
    }

    default:
    {
      return ScAddr::Empty;
    }
    }
  }

  ScIterator3Ptr CreateIterator(
      ScTemplateTriple const * templateTriple,
      ScAddrVector const & replacementConstruction,
      ScTemplateSearchResult & result)
  {
    ScTemplateItem const & item1 = (*templateTriple)[0];
    ScTemplateItem const & item2 = (*templateTriple)[1];
    ScTemplateItem const & item3 = (*templateTriple)[2];

    ScAddr const & addr1 = ResolveAddr(item1, replacementConstruction, result);
    ScAddr const & addr2 = ResolveAddr(item2, replacementConstruction, result);
    ScAddr const & addr3 = ResolveAddr(item3, replacementConstruction, result);

    auto const & PrepareType = [this](ScTemplateItem const & item) -> ScType {
      ScType type = item.m_typeValue;
      if (!item.m_name.empty())
      {
        auto const & found = m_template.m_templateItemsNamesToTypes.find(item.m_name);
        if (found != m_template.m_templateItemsNamesToTypes.cend())
          type = found->second;
      }

      if (type.HasConstancyFlag())
        return type.UpConstType();

      return type;
    };

    if (addr1.IsValid())
    {
      if (!addr2.IsValid())
      {
        if (addr3.IsValid())  // F_A_F
          return m_context.Iterator3(addr1, PrepareType(item2), addr3);
        else  // F_A_A
          return m_context.Iterator3(addr1, PrepareType(item2), PrepareType(item3));
      }
      else
      {
        if (addr3.IsValid())  // F_F_F
          return m_context.Iterator3(addr1, addr2, addr3);
        else  // F_F_A
          return m_context.Iterator3(addr1, addr2, PrepareType(item3));
      }
    }
    else if (addr3.IsValid())
    {
      if (addr2.IsValid())  // A_F_F
        return m_context.Iterator3(PrepareType(item1), addr2, addr3);
      else  // A_A_F
        return m_context.Iterator3(PrepareType(item1), PrepareType(item2), addr3);
    }
    else if (addr2.IsValid() && !addr3.IsValid())  // A_F_A
      return m_context.Iterator3(PrepareType(item1), addr2, PrepareType(item3));

    return {};
  }

  using UsedEdges = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;

  void DoIterationOnNextEqualTriples(
      ScTemplateTriples const & templateTriples,
      std::string const & templateItemName,
      size_t const replacementConstructionIdx,
      ScTemplateTriples const & currentIterableTemplateTriples,
      ScTemplateTriples & childrenTemplateTriples,
      ScTemplateSearchResult & result,
      bool & isFinished,
      bool & isLast)
  {
    isLast = true;
    isFinished = true;

    auto const & checkedTemplateTriplesInCurrentReplacementConstruction =
        m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx];

    std::unordered_set<size_t> iteratedTemplateTriples;
    for (size_t const idx : templateTriples)
    {
      ScTemplateTriple * triple = m_template.m_templateTriples[idx];
      if (iteratedTemplateTriples.find(triple->m_index) != iteratedTemplateTriples.cend())
        continue;

      ScTemplateTriples equalTemplateTriples;
      for (ScTemplateTriple * otherTemplateTriple : m_template.m_templateTriples)
      {
        // check if iterable triple is equal to current, not checked and not iterable with previous
        if (checkedTemplateTriplesInCurrentReplacementConstruction.find(otherTemplateTriple->m_index) ==
                checkedTemplateTriplesInCurrentReplacementConstruction.cend() &&
            currentIterableTemplateTriples.find(idx) == currentIterableTemplateTriples.cend() &&
            IsTriplesEqual(triple, otherTemplateTriple, templateItemName))
        {
          equalTemplateTriples.insert(otherTemplateTriple->m_index);
          iteratedTemplateTriples.insert(otherTemplateTriple->m_index);
        }
      }

      if (!equalTemplateTriples.empty())
      {
        isLast = false;
        DoDependenceIteration(equalTemplateTriples, replacementConstructionIdx, childrenTemplateTriples, result);

        isFinished = std::all_of(
            equalTemplateTriples.begin(),
            equalTemplateTriples.end(),
            [this, replacementConstructionIdx](size_t const idx) {
              return m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].find(idx) !=
                     m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].cend();
            });

        if (!isFinished)
          break;
      }
    }
  }

  bool DoDependenceIterationByItem(
      ScTemplateTriple const * templateTriple,
      ScTemplateItem const & item,
      size_t replacementConstructionIdx,
      ScTemplateTriples const & templateTriples,
      ScTemplateTriples & childrenTemplateTriples,
      ScTemplateSearchResult & result,
      bool & isForLastTemplateTripleAllChildrenFinished,
      bool & isLastTemplateTripleHasNoChildren)
  {
    bool isChildFinished = false;
    bool isNoChild = false;
    ScTemplateTriples nextTemplateTriples;
    FindDependedTriple(item, templateTriple, nextTemplateTriples);

    DoIterationOnNextEqualTriples(
        nextTemplateTriples,
        item.m_name,
        replacementConstructionIdx,
        templateTriples,
        childrenTemplateTriples,
        result,
        isChildFinished,
        isNoChild);

    isForLastTemplateTripleAllChildrenFinished &= isChildFinished;
    isLastTemplateTripleHasNoChildren &= isNoChild;
    return !isForLastTemplateTripleAllChildrenFinished && !isLastTemplateTripleHasNoChildren;
  }

  void DoDependenceIteration(
      ScTemplateTriples const & templateTriples,
      size_t replacementConstructionIdx,
      ScTemplateTriples & childrenTemplateTriples,
      ScTemplateSearchResult & result)
  {
    size_t templateTripleIdx = *templateTriples.begin();
    ScTemplateTriple * templateTriple = m_template.m_templateTriples[templateTripleIdx];

    bool isForLastTemplateTripleAllChildrenFinished = true;
    bool isLastTemplateTripleHasNoChildren = false;

    ScIterator3Ptr it =
        CreateIterator(templateTriple, result.m_replacementConstructions[replacementConstructionIdx], result);
    if (!it || !it->IsValid())
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "During search procedure has been chosen var triple");

    size_t checkedCurrentResultEqualTemplateTriplesCount = 0;

    ScAddrVector nextResultReplacementTriples{result.m_replacementConstructions[replacementConstructionIdx]};
    ScTemplateTriples nextCheckedTemplateTriples{
        m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx]};
    UsedEdges nextUsedReplacementEdges{m_usedEdgesInReplacementConstructions[replacementConstructionIdx]};

    bool isTemplateTriplesIteratorNext = false;
    ScTemplateTriples::const_iterator templateTriplesIterator;
    do
    {
      ScReplacementTriple replacementTriple;
      if (it->Next())
      {
        replacementTriple = it->Get();
        auto copiedTemplateTriplesIterator = templateTriplesIterator;
        if (copiedTemplateTriplesIterator != templateTriples.cend())
        {
          ++copiedTemplateTriplesIterator;
          if (copiedTemplateTriplesIterator == templateTriples.cend())
            isTemplateTriplesIteratorNext = false;
        }
      }
      else
      {
        // it is not universal sc-template search
        break;
      }

      auto & notUsedEdgesInCurrentTemplateTriple = m_notUsedEdgesInTemplateTriples[templateTriple->m_index];
      if (notUsedEdgesInCurrentTemplateTriple.find(replacementTriple[1]) != notUsedEdgesInCurrentTemplateTriple.cend())
        continue;

      bool isFoundInOtherTemplateTriples = false;
      for (size_t const otherTemplateTripleIdx : templateTriples)
      {
        if (templateTripleIdx == otherTemplateTripleIdx)
          continue;

        if (m_usedEdgesInTemplateTriples[otherTemplateTripleIdx].find(replacementTriple[1]) !=
            m_usedEdgesInTemplateTriples[otherTemplateTripleIdx].cend())
        {
          isFoundInOtherTemplateTriples = true;
          break;
        }
      }
      if (isFoundInOtherTemplateTriples)
        continue;

      // check if edge is used for other equal triple
      auto & usedEdgesInCurrentReplacementConstruction =
          m_usedEdgesInReplacementConstructions[replacementConstructionIdx];
      if (usedEdgesInCurrentReplacementConstruction.find(replacementTriple[1]) !=
          usedEdgesInCurrentReplacementConstruction.cend())
        continue;

      // check triple elements by structure belonging or predicate callback
      if ((IsStructureValid() && (!IsInStructure(replacementTriple[0]) || !IsInStructure(replacementTriple[1]) ||
                                  !IsInStructure(replacementTriple[2]))) ||
          (m_checkCallback && (!m_checkCallback(replacementTriple[0]) || !m_checkCallback(replacementTriple[1]) ||
                               !m_checkCallback(replacementTriple[2]))))
      {
        m_usedEdgesInReplacementConstructions[replacementConstructionIdx].insert(replacementTriple[1]);
        continue;
      }

      do
      {
        if (!isTemplateTriplesIteratorNext)
        {
          templateTriplesIterator = templateTriples.cbegin();
          isTemplateTriplesIteratorNext = true;
        }
        else if (templateTriplesIterator != templateTriples.cend())
          ++templateTriplesIterator;

        // check if all equal triples found to make a new search result item
        if (checkedCurrentResultEqualTemplateTriplesCount == templateTriples.size())
        {
          replacementConstructionIdx = ++m_lastReplacementConstructionIdx;
          checkedCurrentResultEqualTemplateTriplesCount = 0;

          ReserveResult(replacementConstructionIdx, result);

          result.m_replacementConstructions.emplace_back(nextResultReplacementTriples);
          m_checkedTemplateTriplesInReplacementConstructions.emplace_back(nextCheckedTemplateTriples);
          m_usedEdgesInReplacementConstructions.emplace_back(DEFAULT_RESULT_RESERVE_SIZE);

          templateTriplesIterator = templateTriples.cbegin();
        }

        auto & checkedTemplateTriplesInCurrentReplacementConstruction =
            m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx];
        if (!isForLastTemplateTripleAllChildrenFinished)
        {
          result.m_replacementConstructions[replacementConstructionIdx].assign(
              nextResultReplacementTriples.cbegin(), nextResultReplacementTriples.cend());
          checkedTemplateTriplesInCurrentReplacementConstruction = nextCheckedTemplateTriples;
          m_usedEdgesInReplacementConstructions[replacementConstructionIdx] = nextUsedReplacementEdges;
        }

        if (templateTriplesIterator == templateTriples.cend())
        {
          isTemplateTriplesIteratorNext = false;
          break;
        }

        templateTripleIdx = *templateTriplesIterator;

        templateTriple = m_template.m_templateTriples[templateTripleIdx];

        if (checkedTemplateTriplesInCurrentReplacementConstruction.find(templateTripleIdx) !=
            checkedTemplateTriplesInCurrentReplacementConstruction.cend())
          continue;

        ScAddrVector & replacementConstruction = result.m_replacementConstructions[replacementConstructionIdx];

        bool isFinished = true;
        auto const & items = templateTriple->GetValues();
        for (size_t i = 0; i < items.size(); ++i)
        {
          ScAddr const & resolvedAddr = ResolveAddr(items[i], replacementConstruction, result);
          if (resolvedAddr.IsValid() && resolvedAddr != replacementTriple[i])
          {
            isForLastTemplateTripleAllChildrenFinished = false;
            isFinished = false;
            break;
          }
        }

        if (!isFinished)
          continue;

        // update data
        {
          UpdateResult(templateTriple, replacementConstructionIdx, replacementTriple, result);
        }

        // find next depended on triples and analyse result
        {
          isForLastTemplateTripleAllChildrenFinished = true;
          isLastTemplateTripleHasNoChildren = true;

          // first of all check triples by edge, it is more effectively
          if (DoDependenceIterationByItem(
                  templateTriple,
                  items[1],
                  replacementConstructionIdx,
                  templateTriples,
                  childrenTemplateTriples,
                  result,
                  isForLastTemplateTripleAllChildrenFinished,
                  isLastTemplateTripleHasNoChildren) ||
              DoDependenceIterationByItem(
                  templateTriple,
                  items[0],
                  replacementConstructionIdx,
                  templateTriples,
                  childrenTemplateTriples,
                  result,
                  isForLastTemplateTripleAllChildrenFinished,
                  isLastTemplateTripleHasNoChildren) ||
              DoDependenceIterationByItem(
                  templateTriple,
                  items[2],
                  replacementConstructionIdx,
                  templateTriples,
                  childrenTemplateTriples,
                  result,
                  isForLastTemplateTripleAllChildrenFinished,
                  isLastTemplateTripleHasNoChildren))
          {
            for (auto const & otherTemplateTripleIdx : childrenTemplateTriples)
            {
              m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].erase(
                  otherTemplateTripleIdx);
            }
            childrenTemplateTriples.clear();
            ClearResult(templateTripleIdx, replacementConstructionIdx, replacementConstruction);
            continue;
          }

          // all connected triples found
          if (isForLastTemplateTripleAllChildrenFinished)
          {
            ++checkedCurrentResultEqualTemplateTriplesCount;

            // current edge is busy for all equal triples
            childrenTemplateTriples.insert(templateTripleIdx);
            m_usedEdgesInTemplateTriples[templateTripleIdx].insert(replacementTriple[1]);
            m_usedEdgesInReplacementConstructions[replacementConstructionIdx].insert(replacementTriple[1]);

            break;
          }
        }
      } while (templateTriplesIterator != templateTriples.cend());

      // there are no next triples for current triple, it is last
      if (isLastTemplateTripleHasNoChildren && isForLastTemplateTripleAllChildrenFinished &&
          m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].size() ==
              m_template.m_templateTriples.size())
      {
        if (!m_filterCallback || m_filterCallback(ScTemplateSearchResultItem(
                                     &result.m_replacementConstructions[replacementConstructionIdx],
                                     &result.m_templateItemsNamesToReplacementItemsPositions)))
          AppendFoundReplacementConstruction(result, replacementConstructionIdx);
      }
    } while (!isStopped);
  }

  void UpdateResult(
      ScTemplateTriple const * templateTriple,
      size_t const replacementConstructionIdx,
      ScAddrTriple const & replacementTriple,
      ScTemplateSearchResult & result)
  {
    auto const & UpdateResultByItem =
        [&result](
            ScTemplateItem const & item, ScAddr const & addr, size_t const elementNum, ScAddrVector & resultAddrs) {
          resultAddrs[elementNum] = addr;

          if (item.m_name.empty())
            return;

          result.m_templateItemsNamesToReplacementItemsPositions[item.m_name] = elementNum;
        };

    m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].insert(templateTriple->m_index);
    m_usedEdgesInReplacementConstructions[replacementConstructionIdx].insert(replacementTriple[1]);

    size_t itemIdx = templateTriple->m_index * 3;
    for (size_t i = replacementConstructionIdx; i < result.Size(); ++i)
    {
      ScAddrVector & resultAddrs = result.m_replacementConstructions[i];

      UpdateResultByItem((*templateTriple)[0], replacementTriple[0], itemIdx, resultAddrs);
      UpdateResultByItem((*templateTriple)[1], replacementTriple[1], itemIdx + 1, resultAddrs);
      UpdateResultByItem((*templateTriple)[2], replacementTriple[2], itemIdx + 2, resultAddrs);
    }
  };

  void ClearResult(
      size_t const tripleIdx,
      size_t const replacementConstructionIdx,
      ScAddrVector & replacementConstruction)
  {
    m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].erase(tripleIdx);

    size_t itemIdx = tripleIdx * 3;

    replacementConstruction[itemIdx] = ScAddr::Empty;
    m_usedEdgesInReplacementConstructions[replacementConstructionIdx].erase(replacementConstruction[++itemIdx]);
    m_notUsedEdgesInTemplateTriples[tripleIdx].insert(replacementConstruction[itemIdx]);
    replacementConstruction[itemIdx] = ScAddr::Empty;
    replacementConstruction[++itemIdx] = ScAddr::Empty;
  };

  void AppendFoundReplacementConstruction(ScTemplateSearchResult & result, size_t & resultIdx)
  {
    if (m_callback)
    {
      m_callback(ScTemplateSearchResultItem(
          &result.m_replacementConstructions[resultIdx], &result.m_templateItemsNamesToReplacementItemsPositions));
    }
    else if (m_callbackWithRequest)
    {
      ScTemplateSearchRequest const & request = m_callbackWithRequest(ScTemplateSearchResultItem(
          &result.m_replacementConstructions[resultIdx], &result.m_templateItemsNamesToReplacementItemsPositions));
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
      m_foundReplacementConstructions.insert(resultIdx);
  }

  void ReserveResult(size_t const replacementConstructionIdx, ScTemplateSearchResult & result)
  {
    if (replacementConstructionIdx < DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount)
      return;

    ++m_resultReserveCount;
    result.m_replacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount);
    m_checkedTemplateTriplesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount);
    m_usedEdgesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount);
  }

  void DoIterations(ScTemplateSearchResult & result)
  {
    if (m_template.m_templateTriples.empty())
      return;

    ScAddrVector newResult;
    newResult.resize(CalculateOneResultSize());
    result.m_replacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE);
    result.m_replacementConstructions.emplace_back(newResult);

    m_notUsedEdgesInTemplateTriples.resize(m_template.m_templateTriples.size());
    m_usedEdgesInTemplateTriples.resize(m_template.m_templateTriples.size());
    m_usedEdgesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE);
    m_usedEdgesInReplacementConstructions.emplace_back();
    m_checkedTemplateTriplesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE);
    m_checkedTemplateTriplesInReplacementConstructions.emplace_back();

    ScTemplateTriples childrenTemplateTriples;

    bool isFinished = false;
    bool isLast = false;

    DoIterationOnNextEqualTriples(
        m_connectivityComponentPriorityTemplateTriples, "", 0, {}, childrenTemplateTriples, result, isFinished, isLast);
  }

public:
  ScTemplate::Result operator()(ScTemplateSearchResult & result)
  {
    result.Clear();
    DoIterations(result);

    std::vector<ScAddrVector> checkedResults;
    checkedResults.reserve(result.Size());
    for (size_t const foundIdx : m_foundReplacementConstructions)
    {
      checkedResults.emplace_back(result.m_replacementConstructions[foundIdx]);
    }
    result.m_replacementConstructions.assign(checkedResults.cbegin(), checkedResults.cend());

    return ScTemplate::Result(result.Size() > 0);
  }

  void operator()()
  {
    ScTemplateSearchResult result;
    DoIterations(result);
  }

  size_t CalculateOneResultSize() const
  {
    return m_template.m_templateTriples.size() * 3;
  }

private:
  ScTemplate & m_template;
  ScMemoryContext & m_context;

  // fields for template preprocessing
  std::map<std::string, ScTemplateTriples> m_templateItemsNamesToDependedTemplateTriples;
  ScTemplateTriples m_cycledTemplateTriples;
  std::vector<ScTemplateTriples> m_connectivityComponentsTemplateTriples;
  ScTemplateTriples m_connectivityComponentPriorityTemplateTriples;

  // fields search by template
  std::vector<UsedEdges> m_notUsedEdgesInTemplateTriples;
  std::vector<UsedEdges> m_usedEdgesInTemplateTriples;
  std::vector<UsedEdges> m_usedEdgesInReplacementConstructions;
  std::vector<ScTemplateTriples> m_checkedTemplateTriplesInReplacementConstructions;

  size_t const DEFAULT_RESULT_RESERVE_SIZE = 512;
  size_t m_resultReserveCount = 1;
  size_t m_lastReplacementConstructionIdx = 0;
  std::unordered_set<size_t> m_foundReplacementConstructions;

  // fields for append result handling
  bool isStopped = false;

  ScAddr const m_structure;
  ScTemplateSearchResultCallback m_callback;
  ScTemplateSearchResultCallbackWithRequest m_callbackWithRequest;
  ScTemplateSearchResultFilterCallback m_filterCallback;
  ScTemplateSearchResultCheckCallback m_checkCallback;
};

ScTemplate::Result ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty);
  return search(result);
}

void ScTemplate::Search(
    ScMemoryContext & ctx,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty);
  search.SetCallback(callback);
  search.SetFilterCallback(filterCallback);
  search.SetCheckCallback(checkCallback);
  search();
}

void ScTemplate::Search(
    ScMemoryContext & ctx,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty);
  search.SetCallbackWithRequest(callback);
  search.SetFilterCallback(filterCallback);
  search.SetCheckCallback(checkCallback);
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
