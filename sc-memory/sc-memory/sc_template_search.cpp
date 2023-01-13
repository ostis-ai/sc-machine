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
  ScTemplateSearch(ScTemplate & templ, ScMemoryContext & context, ScAddr const & scStruct)
    : m_template(templ)
    , m_context(context)
    , m_struct(scStruct)
  {
    PrepareSearch();
  }

  using ScTemplateGroupedConstructions = ScTemplate::ScTemplateGroupedConstructions;

  void PrepareSearch()
  {
    for (auto const * construct : m_template.m_constructions)
    {
      auto & values = construct->GetValues();
      ScTemplateItemValue const & item1 = values[0];
      ScTemplateItemValue const & item2 = values[1];
      ScTemplateItemValue const & item3 = values[2];

      for (auto * otherConstruct : m_template.m_constructions)
      {
        auto & otherValues = otherConstruct->GetValues();
        ScTemplateItemValue const & otherItem1 = otherValues[0];
        ScTemplateItemValue const & otherItem2 = otherValues[1];
        ScTemplateItemValue const & otherItem3 = otherValues[2];

        if (construct->m_index == otherConstruct->m_index)
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
            std::stringstream stream;
            stream << item.m_replacementName << construct->m_index;

            std::string const key = stream.str();
            auto const & found = m_itemsToConstructs.find(key);
            if (found == m_itemsToConstructs.cend())
              m_itemsToConstructs.insert({key, {otherConstruct}});
            else
              found->second.insert(otherConstruct);
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
    return m_struct.IsValid();
  }

  inline bool IsInStructure(ScAddr const & addr)
  {
    return m_context.HelperCheckEdge(m_struct, addr, ScType::EdgeAccessConstPosPerm);
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

  void FindDependedConstruction(
      ScTemplateItemValue const & value,
      ScTemplateConstr3 const * construct,
      ScTemplateGroupedConstructions & nextConstructs)
  {
    if (value.m_replacementName.empty())
      return;

    std::stringstream stream;
    stream << value.m_replacementName << construct->m_index;

    auto const & found = m_itemsToConstructs.find(stream.str());
    if (found != m_itemsToConstructs.cend())
    {
      nextConstructs = found->second;
    }
  }

  bool IsConstructsEqual(
      ScTemplateConstr3 const * construct,
      ScTemplateConstr3 const * otherConstruct,
      ScAddrVector const & resultAddrs,
      ScTemplateSearchResult & result)
  {
    auto const & constructValues = construct->GetValues();
    auto const & otherConstructValues = otherConstruct->GetValues();

    auto const & IsConstructsItemsEqual =
        [&resultAddrs, &result](ScTemplateItemValue const & value, ScTemplateItemValue const & otherValue) -> bool {
      bool const isEqual =
          (value.m_typeValue == otherValue.m_typeValue || value.m_typeValue.IsUnknown() ||
           otherValue.m_typeValue.IsUnknown() || value.m_typeValue.CanExtendTo(otherValue.m_typeValue) ||
           otherValue.m_typeValue.CanExtendTo(value.m_typeValue)) &&
          value.m_addrValue == otherValue.m_addrValue;

      if (!resultAddrs.empty())
      {
        auto const & valueFound = result.m_replacements.find(value.m_replacementName);
        if (valueFound != result.m_replacements.cend())
        {
          auto const & otherValueFound = result.m_replacements.find(value.m_replacementName);
          return otherValueFound != result.m_replacements.cend() && isEqual &&
                 resultAddrs[valueFound->second] == resultAddrs[otherValueFound->second];
        }
      }

      return isEqual;
    };

    return IsConstructsItemsEqual(constructValues[0], otherConstructValues[0]) &&
           IsConstructsItemsEqual(constructValues[1], otherConstructValues[1]) &&
           IsConstructsItemsEqual(constructValues[2], otherConstructValues[2]) &&
           (constructValues[0].m_replacementName == otherConstructValues[0].m_replacementName ||
            constructValues[2].m_replacementName == otherConstructValues[2].m_replacementName);
  };

  using UsedEdges = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;

  void DoIterationOnNextEqualConstructions(
      ScTemplateGroupedConstructions const & constructs,
      std::unordered_set<size_t> & checkedConstructs,
      ScTemplateGroupedConstructions const & currentIterableConstructs,
      UsedEdges & checkedEdges,
      ScAddrVector & resultAddrs,
      ScTemplateSearchResult & result,
      bool & isFinished,
      bool & isLast)
  {
    isLast = true;
    isFinished = true;

    std::unordered_set<size_t> iteratedConstructs;

    for (ScTemplateConstr3 const * construct : constructs)
    {
      if (iteratedConstructs.find(construct->m_index) != iteratedConstructs.cend())
        continue;

      ScTemplateGroupedConstructions equalConstructs;
      for (ScTemplateConstr3 * otherConstruct : m_template.m_constructions)
      {
        if (checkedConstructs.find(otherConstruct->m_index) == checkedConstructs.cend() &&
            std::find_if(
                currentIterableConstructs.cbegin(),
                currentIterableConstructs.cend(),
                [&](ScTemplateConstr3 const * other) {
                  return *other == *otherConstruct;
                }) == currentIterableConstructs.cend() &&
            IsConstructsEqual(construct, otherConstruct, resultAddrs, result))
        {
          equalConstructs.insert(otherConstruct);
          iteratedConstructs.insert(otherConstruct->m_index);
        }
      }

      if (!equalConstructs.empty())
      {
        isLast = false;
        DoDependenceIteration(equalConstructs, checkedConstructs, checkedEdges, resultAddrs, result, isFinished);
      }
    }
  }

  void DoDependenceIteration(
      ScTemplateGroupedConstructions const & constructs,
      std::unordered_set<size_t> & checkedConstructs,
      UsedEdges & checkedEdges,
      ScAddrVector & resultAddrs,
      ScTemplateSearchResult & result,
      bool & isFinished)
  {
    auto const & TryDoNextDependenceIteration = [this, &result](
                                                    ScTemplateConstr3 const * construct,
                                                    ScTemplateItemValue const & item,
                                                    ScAddr const & itemAddr,
                                                    std::unordered_set<size_t> & checkedConstructs,
                                                    UsedEdges & checkedEdges,
                                                    ScAddrVector & resultAddrs,
                                                    ScTemplateGroupedConstructions const & currentIterableConstructs,
                                                    bool & isFinished,
                                                    bool & isLast) {
      ScTemplateGroupedConstructions nextConstructs;
      FindDependedConstruction(item, construct, nextConstructs);

      DoIterationOnNextEqualConstructions(
          nextConstructs,
          checkedConstructs,
          currentIterableConstructs,
          checkedEdges,
          resultAddrs,
          result,
          isFinished,
          isLast);
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
        [](size_t constructIdx, ScAddrVector & resultAddrs, std::unordered_set<size_t> & checkedConstructs) {
          checkedConstructs.erase(constructIdx);

          constructIdx *= 3;
          resultAddrs[constructIdx] = ScAddr::Empty;
          resultAddrs[++constructIdx] = ScAddr::Empty;
          resultAddrs[++constructIdx] = ScAddr::Empty;
        };

    ScTemplateConstr3 * construct = *constructs.begin();

    bool isAllChildrenFinished = false;
    bool isLast = false;

    auto values = construct->GetValues();

    ScTemplateItemValue value1 = values[0];
    ScTemplateItemValue value2 = values[1];
    ScTemplateItemValue value3 = values[2];

    ScIterator3Ptr const it = CreateIterator(value1, value2, value3, resultAddrs, result);

    size_t count = 0;

    std::unordered_set<size_t> const currentCheckedConstructs{checkedConstructs};
    ScAddrVector const currentResultAddrs{resultAddrs};
    while (it->Next())
    {
      ScAddr const & addr2 = it->Get(1);
      if (checkedEdges.find(addr2) != checkedEdges.cend())
      {
        isLast = true;
        isAllChildrenFinished = true;
        continue;
      }

      if (isAllChildrenFinished && count == constructs.size())
      {
        count = 0;
        checkedConstructs = std::unordered_set<size_t>{currentCheckedConstructs};
        resultAddrs.assign(currentResultAddrs.cbegin(), currentResultAddrs.cend());
      }

      ScAddr const & addr1 = it->Get(0);
      ScAddr const & addr3 = it->Get(2);

      // check construction for that it is in structure
      if (IsStructureValid() && (!IsInStructure(addr1) || !IsInStructure(addr2) || !IsInStructure(addr3)))
        return;

      for (ScTemplateConstr3 * otherConstruct : constructs)
      {
        construct = otherConstruct;

        if (checkedConstructs.find(construct->m_index) != checkedConstructs.cend())
        {
          continue;
        }

        values = construct->GetValues();
        value1 = values[0];
        value2 = values[1];
        value3 = values[2];

        //        std::cout << construct->m_index << " = {" << value1.m_replacementName << "} ---{" <<
        //        value2.m_replacementName
        //                  << "}---> {" << value3.m_replacementName << "}" << std::endl;

        {
          // don't use cycle to call this function
          size_t const idx = construct->m_index * 3;
          UpdateResults(value1, addr1, idx, resultAddrs);
          UpdateResults(value2, addr2, idx + 1, resultAddrs);
          UpdateResults(value3, addr3, idx + 2, resultAddrs);

          checkedConstructs.insert(construct->m_index);
        }

        // find next depended on constructions and analyse result
        {
          bool isChildFinished = false;
          bool isNoChild = false;
          TryDoNextDependenceIteration(
              construct,
              value2,
              addr2,
              checkedConstructs,
              checkedEdges,
              resultAddrs,
              constructs,
              isChildFinished,
              isNoChild);
          isAllChildrenFinished = isChildFinished;
          isLast = isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(construct->m_index, resultAddrs, checkedConstructs);
            continue;
          }

          TryDoNextDependenceIteration(
              construct,
              value1,
              addr1,
              checkedConstructs,
              checkedEdges,
              resultAddrs,
              constructs,
              isChildFinished,
              isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(construct->m_index, resultAddrs, checkedConstructs);
            continue;
          }

          TryDoNextDependenceIteration(
              construct,
              value3,
              addr3,
              checkedConstructs,
              checkedEdges,
              resultAddrs,
              constructs,
              isChildFinished,
              isNoChild);
          isAllChildrenFinished &= isChildFinished;
          isLast &= isNoChild;
          if (!isChildFinished && !isLast)
          {
            ClearResults(construct->m_index, resultAddrs, checkedConstructs);
            continue;
          }

          if (isAllChildrenFinished)
          {
            //            std::cout << "succeed " << construct->m_index << " = {" << value1.m_replacementName << "}---{"
            //                      << value2.m_replacementName << "}---> {" << value3.m_replacementName << "}" <<
            //                      std::endl;
            checkedEdges.insert(addr2);
            ++count;
            break;
          }
        }
      }

      if (isLast & isAllChildrenFinished)
      {
        //        std::cout << "Found " << checkedConstructs.size() << " to achieve " <<
        //        m_template.m_constructions.size()
        //                  << std::endl;

        if (checkedConstructs.size() == m_template.m_constructions.size())
        {
          result.m_results.emplace_back(resultAddrs);
        }
      }
    }

    isFinished = isAllChildrenFinished;
  }

  void DoIterations(ScTemplateSearchResult & result)
  {
    if (m_template.m_constructions.empty())
      return;

    auto const & DoStartIteration = [this, &result](ScTemplateGroupedConstructions const & constructions) {
      std::vector<ScAddr> resultAddrs;
      resultAddrs.resize(CalculateOneResultSize());
      std::unordered_set<size_t> checkedConstructs;
      checkedConstructs.reserve(CalculateOneResultSize());
      UsedEdges checkedEdges;
      checkedEdges.reserve(CalculateOneResultSize());

      bool isFinished = false;
      bool isLast = false;

      DoIterationOnNextEqualConstructions(
          constructions, checkedConstructs, {}, checkedEdges, resultAddrs, result, isFinished, isLast);
    };

    auto const & constructs = m_template.m_orderedConstructions;
    for (ScTemplateGroupedConstructions const & equalConstructs : constructs)
    {
      if (!equalConstructs.empty())
      {
        DoStartIteration(equalConstructs);

        break;
      }
    }
  }

  ScTemplate::Result operator()(ScTemplateSearchResult & result)
  {
    result.Clear();

    DoIterations(result);

    return ScTemplate::Result(result.Size() > 0);
  }

  size_t CalculateOneResultSize() const
  {
    return m_template.m_constructions.size() * 3;
  }

private:
  ScTemplate & m_template;
  ScMemoryContext & m_context;
  ScAddr const m_struct;

  std::map<std::string, ScTemplateGroupedConstructions> m_itemsToConstructs;
};

ScTemplate::Result ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result)
{
  ScTemplateSearch search(*this, ctx, ScAddr());
  return search(result);
}

ScTemplate::Result ScTemplate::SearchInStruct(
    ScMemoryContext & ctx,
    ScAddr const & scStruct,
    ScTemplateSearchResult & result)
{
  ScTemplateSearch search(*this, ctx, scStruct);
  return search(result);
}
