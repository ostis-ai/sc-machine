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
  ScTemplateSearch(ScTemplate & templ,
                   ScMemoryContext & context,
                   ScAddr const & scStruct)
      : m_template(templ)
      , m_context(context)
      , m_struct(scStruct)
  {
    PrepareSearch();
  }

  void PrepareSearch()
  {
    for (auto const & construct : m_template.m_constructions)
    {
      auto & values = construct.GetValues();
      ScTemplateItemValue const & item1 = values[0];
      ScTemplateItemValue const & item2 = values[1];
      ScTemplateItemValue const & item3 = values[2];

      for (auto const & otherConstruct : m_template.m_constructions)
      {
        auto & otherValues = otherConstruct.GetValues();
        ScTemplateItemValue const & otherItem1 = otherValues[0];
        ScTemplateItemValue const & otherItem2 = otherValues[1];
        ScTemplateItemValue const & otherItem3 = otherValues[2];

        if (construct.m_index == otherConstruct.m_index)
          continue;

        auto const TryAddDependence = [&](
            ScTemplateItemValue const & item,
            ScTemplateItemValue const & otherItem1,
            ScTemplateItemValue const & otherItem2,
            ScTemplateItemValue const & otherItem3)
        {
          if (item.m_replacementName.empty())
            return;

          sc_bool withItem1Equal = item.m_replacementName == otherItem1.m_replacementName;
          sc_bool withItem2Equal = item.m_replacementName == otherItem2.m_replacementName;
          sc_bool withItem3Equal = item.m_replacementName == otherItem3.m_replacementName;

          if (withItem1Equal || withItem2Equal || withItem3Equal)
          {
            std::stringstream stream;
            stream << item.m_replacementName << construct.m_index;

            std::string const key = stream.str();
            auto * ptr = (ScTemplateConstr3 *)(&otherConstruct);
            auto const & found = m_itemsToConstructs.find(key);
            if (found == m_itemsToConstructs.end())
              m_itemsToConstructs.insert({ key, { ptr } });
            else
              found->second.insert(ptr);
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

  ScAddr const & ResolveAddr(ScTemplateItemValue const & value, ScAddrVector const & resultAddrs, ScTemplateSearchResult & result) const
  {
    static ScAddr empty;

    switch (value.m_itemType)
    {
      case ScTemplateItemValue::Type::Addr:
        return value.m_addrValue;

      case ScTemplateItemValue::Type::Replace:
      {
        auto it = result.m_replacements.find(value.m_replacementName);
        if (it == result.m_replacements.end())
          return empty;
        return resultAddrs[it->second];
      }

      case ScTemplateItemValue::Type::Type:
      {
        if (!value.m_replacementName.empty())
        {
          auto it = result.m_replacements.find(value.m_replacementName);
          if (it == result.m_replacements.end())
            return empty;
          return resultAddrs[it->second];
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

    auto const PrepareType = [](ScType const & type)
    {
      if (type.HasConstancyFlag())
        return type.UpConstType();

      return type;
    };

    if (addr1.IsValid())
    {
      if (!addr2.IsValid())
      {
        if (addr3.IsValid()) // F_A_F
        {
          return m_context.Iterator3(addr1, PrepareType(value2.m_typeValue), addr3);
        }
        else // F_A_A
        {
          return m_context.Iterator3(addr1, PrepareType(value2.m_typeValue), PrepareType(value3.m_typeValue));
        }
      }
      else
      {
        if (addr3.IsValid()) // F_F_F
        {
          return m_context.Iterator3(addr1, addr2, addr3);
        }
        else // F_F_A
        {
          return m_context.Iterator3(addr1, addr2, PrepareType(value3.m_typeValue));
        }
      }
    }
    else if (addr3.IsValid())
    {
      if (addr2.IsValid()) // A_F_F
      {
        return m_context.Iterator3(PrepareType(value1.m_typeValue), addr2, addr3);
      }
      else // A_A_F
      {
        return m_context.Iterator3(PrepareType(value1.m_typeValue), PrepareType(value2.m_typeValue), addr3);
      }
    }
    else if (addr2.IsValid() && !addr3.IsValid()) // A_F_A
    {
      return m_context.Iterator3(PrepareType(value1.m_typeValue), addr2, PrepareType(value3.m_typeValue));
    }

    return {};
  }

  std::set<ScTemplateConstr3 *> FindDependedConstruction(ScTemplateItemValue const & value, ScTemplateConstr3 const & construct)
  {
    if (value.m_replacementName.empty())
      return {};

    std::stringstream stream;
    stream << value.m_replacementName << construct.m_index;

    auto const & found = m_itemsToConstructs.find(stream.str());
    if (found != m_itemsToConstructs.cend())
      return found->second;

    return {};
  }

  using UsedEdges = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;

  bool DoDependenceIteration(
      ScTemplateConstr3 const & construct,
      ScTemplateSearchResult & result,
      ScAddrVector & resultAddrs,
      UsedEdges & resultFoundEdges,
      std::unordered_set<size_t> & foundConstructs,
      size_t & itNum,
      size_t & elementNum)
  {
    auto const & values = construct.GetValues();

    ScTemplateItemValue const & value1 = values[0];
    ScTemplateItemValue const & value2 = values[1];
    ScTemplateItemValue const & value3 = values[2];

    ScIterator3Ptr const it = CreateIterator(value1, value2, value3, resultAddrs, result);

    auto const & TryDoNextDependenceIteration = [this, &result, &construct](
        ScTemplateItemValue const & item,
        ScAddr const & itemAddr,
        ScAddrVector & resultAddrs,
        UsedEdges & foundEdges,
        std::unordered_set<size_t> & foundConstructs,
        size_t & itNum,
        size_t & elementNum) {

      std::set<ScTemplateConstr3 *> nextConstructs = FindDependedConstruction(item, construct);

      for (ScTemplateConstr3 * nextConstruct : nextConstructs)
      {
        if (foundConstructs.find(nextConstruct->m_index) == foundConstructs.cend())
        {
          DoDependenceIteration(*nextConstruct, result, resultAddrs, foundEdges, foundConstructs, itNum, elementNum);
        }
      }
    };

    auto const & UpdateResults = [&result](ScTemplateItemValue const & value, ScAddr const & addr, size_t const elementNum, ScAddrVector & resultAddrs) {
      resultAddrs[elementNum] = addr;

      if (value.m_replacementName.empty())
        return;

      if (result.m_replacements.find(value.m_replacementName) == result.m_replacements.end())
        result.m_replacements[value.m_replacementName] = elementNum;
    };

    bool isStart = true;

    std::unordered_set<size_t> copiedConstructs{foundConstructs};
    UsedEdges copiedEdges{resultFoundEdges};
    for (; it->IsValid() && it->Next(); copiedConstructs = foundConstructs, copiedEdges = resultFoundEdges)
    {
      if (isStart)
      {
        isStart = false;
      }
      else
      {
        --itNum;
        elementNum -= 3;
      }

      ScAddr const & addr1 = it->Get(0);
      ScAddr const & addr2 = it->Get(1);

      if (copiedEdges.find(addr2) != copiedEdges.cend())
        continue;

      ScAddr const & addr3 = it->Get(2);

      ScAddrVector copiedAddrs{resultAddrs};

      UpdateResults(value1, addr1, elementNum, copiedAddrs);
      UpdateResults(value2, addr2, elementNum + 1, copiedAddrs);
      UpdateResults(value3, addr3, elementNum + 2, copiedAddrs);

      ++itNum;
      elementNum += 3;

      copiedConstructs.insert(construct.m_index);
      copiedEdges.insert(addr2);

      if (itNum == m_template.m_constructions.size())
      {
        result.m_results.push_back(copiedAddrs);
      }
      else
      {
        // check construction for that it is in structure
        if (IsStructureValid() && (!IsInStructure(addr1) || !IsInStructure(addr2) || !IsInStructure(addr3)))
          continue;

        TryDoNextDependenceIteration(value2, addr2, copiedAddrs, copiedEdges, copiedConstructs, itNum, elementNum);
        TryDoNextDependenceIteration(value1, addr1, copiedAddrs, copiedEdges, copiedConstructs, itNum, elementNum);
        TryDoNextDependenceIteration(value3, addr3, copiedAddrs, copiedEdges, copiedConstructs, itNum, elementNum);
      }
    }

    return true;
  }

  void DoIterations(ScTemplateSearchResult & result)
  {
    if (m_template.m_constructions.empty())
      return;

    auto const & DoStartIteration = [this, &result](ScTemplateConstr3 const & construct) {
      size_t itNum = 0;
      size_t elementNum = 0;

      std::vector<ScAddr> resultAddrs;
      resultAddrs.resize(CalculateOneResultSize());
      std::unordered_set<size_t> resultConstructs;
      UsedEdges foundEdges;

      DoDependenceIteration(construct, result, resultAddrs, foundEdges, resultConstructs, itNum, elementNum);
    };

    auto & constructs = m_template.m_orderedConstructions;
    for (auto const & equalConstructs : constructs)
    {
      if (!equalConstructs.empty())
      {
        ScTemplateConstr3 const & construct = equalConstructs.front();

        DoStartIteration(construct);

        break;
      }
    }
  }

  ScTemplate::Result operator () (ScTemplateSearchResult & result)
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

  std::map<std::string, std::set<ScTemplateConstr3 *>> m_itemsToConstructs;
};

ScTemplate::Result ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result)
{
  ScTemplateSearch search(*this, ctx, ScAddr());
  return search(result);
}

ScTemplate::Result ScTemplate::SearchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result)
{
  ScTemplateSearch search(*this, ctx, scStruct);
  return search(result);
}
