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
          sc_bool withItem1Equal = item.m_replacementName == otherItem1.m_replacementName;
          sc_bool withItem2Equal = item.m_replacementName == otherItem2.m_replacementName;
          sc_bool withItem3Equal = item.m_replacementName == otherItem3.m_replacementName;

          if (!item.m_replacementName.empty() && (withItem1Equal || withItem2Equal || withItem3Equal))
          {
            std::stringstream stream;
            stream << item.m_replacementName << construct.m_index;

            if (m_itemsToConstructs.find(stream.str()) == m_itemsToConstructs.end())
              m_itemsToConstructs.insert({ stream.str(), &otherConstruct });

            if (withItem1Equal)
            {
              m_itemsToItems.insert({ stream.str(), 0u });
            }
            if (withItem2Equal)
            {
              m_itemsToItems.insert({ stream.str(), 1u });
            }
            if (withItem3Equal)
            {
              m_itemsToItems.insert({ stream.str(), 2u });
            }
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

  ScAddr const & ResolveAddr(ScTemplateItemValue const & value, ScAddrVector const & resultAddrs) const
  {
    switch (value.m_itemType)
    {
      case ScTemplateItemValue::Type::Addr:
        return value.m_addrValue;

      case ScTemplateItemValue::Type::Replace:
      {
        auto it = m_template.m_replacements.find(value.m_replacementName);
        SC_ASSERT(it != m_template.m_replacements.end(), ());
        SC_ASSERT(it->second < resultAddrs.size(), ());
        return resultAddrs[it->second];
      }

      case ScTemplateItemValue::Type::Type:
      {
        if (!value.m_replacementName.empty())
        {
          auto it = m_template.m_replacements.find(value.m_replacementName);
          SC_ASSERT(it != m_template.m_replacements.end(), ());
          SC_ASSERT(it->second < resultAddrs.size(), ());
          return resultAddrs[it->second];
        }
        break;
      }

      default:
        break;
    }

    static ScAddr empty;
    return empty;
  }

  ScIterator3Ptr CreateIterator(
      ScTemplateItemValue const & value1,
      ScTemplateItemValue const & value2,
      ScTemplateItemValue const & value3,
      ScAddrVector const & resultAddrs)
  {
    ScAddr const addr1 = ResolveAddr(value1, resultAddrs);
    ScAddr const addr2 = ResolveAddr(value2, resultAddrs);
    ScAddr const addr3 = ResolveAddr(value3, resultAddrs);

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

  ScTemplateConstr3 * FindDependedConstruction(ScTemplateItemValue const & value, ScTemplateConstr3 const & construct)
  {
    std::stringstream stream;
    stream << value.m_replacementName << construct.m_index;

    auto const & found = m_itemsToConstructs.find(stream.str());
    if (found != m_itemsToConstructs.cend())
      return &found->second;

    return nullptr;
  }

  void ReplaceInDependedConstruction(ScTemplateItemValue const & value, ScTemplateConstr3 const & construct, ScAddr replAddr)
  {
    std::stringstream stream;
    stream << value.m_replacementName << construct.m_index;

    auto const & found = m_itemsToItems.find(stream.str());
    if (found != m_itemsToItems.cend())
    {
      auto const & foundConstruct = m_itemsToConstructs.find(stream.str());
      if (foundConstruct != m_itemsToConstructs.cend())
      {
        foundConstruct->second.SetAddr(found->second, replAddr);
      }
    }
  }

  bool DoDependenceIteration(
      ScTemplateConstr3 const & construct,
      ScTemplateSearchResult & result,
      ScAddrVector & resultAddrs,
      std::list<ScTemplateConstr3> & foundConstructs,
      size_t & itNum,
      size_t & elementNum)
  {
    auto const & values = construct.GetValues();

    ScTemplateItemValue const & value1 = values[0];
    ScTemplateItemValue const & value2 = values[1];
    ScTemplateItemValue const & value3 = values[2];

    ScIterator3Ptr it = CreateIterator(value1, value2, value3, resultAddrs);

    auto const & TryDoNextDependenceIteration = [this, &result](
        ScTemplateItemValue const & item,
        ScAddr const & itemAddr,
        ScTemplateConstr3 const & construct,
        ScAddrVector & resultAddrs,
        std::list<ScTemplateConstr3> & foundConstructs,
        size_t & itNum,
        size_t & elementNum) {
      ReplaceInDependedConstruction(item, construct, itemAddr);

      auto * nextConstruct = FindDependedConstruction(item, construct);
      if (nextConstruct == nullptr)
      {
        return;
      }

      if (itNum == m_template.m_constructions.size() - 1)
      {
        result.m_results.push_back(resultAddrs);
      }

      std::cout << "Before " << itNum << std::endl;

      if (std::find(foundConstructs.cbegin(), foundConstructs.cend(), *nextConstruct) == foundConstructs.cend())
      {
        ++itNum;
        elementNum += 3;

        foundConstructs.push_back(*nextConstruct);
        DoDependenceIteration(*nextConstruct, result, resultAddrs, foundConstructs, itNum, elementNum);
      }

      std::cout << "After " << itNum << std::endl;

      if (itNum == m_template.m_constructions.size() - 1)
      {
        result.m_results.push_back(resultAddrs);
      }
    };

    while (it.get() && it->IsValid() && it->Next())
    {
      ScAddr const & addr1 = it->Get(0);
      ScAddr const & addr2 = it->Get(1);
      ScAddr const & addr3 = it->Get(2);

      resultAddrs[elementNum] = addr1;
      resultAddrs[elementNum + 1] = addr2;
      resultAddrs[elementNum + 2] = addr3;

      ScAddrVector & copiedAddrs{resultAddrs};
      std::list<ScTemplateConstr3> & copiedConstructs{foundConstructs};

      // check construction for that it is in structure
      if (IsStructureValid() && (!IsInStructure(addr1) || !IsInStructure(addr2) || !IsInStructure(addr3)))
        continue;

      TryDoNextDependenceIteration(value1, addr1, construct, copiedAddrs, copiedConstructs, itNum, elementNum);
      TryDoNextDependenceIteration(value2, addr2, construct, copiedAddrs, copiedConstructs, itNum, elementNum);
      TryDoNextDependenceIteration(value3, addr3, construct, copiedAddrs, copiedConstructs, itNum, elementNum);
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

      std::list<ScTemplateConstr3> resultConstructs;
      DoDependenceIteration(construct, result, resultAddrs, resultConstructs, itNum, elementNum);
    };

    auto & constructs = m_template.m_orderedConstructions;
    for (auto const & equalConstructs : constructs)
    {
      if (!equalConstructs.empty())
      {
        ScTemplateConstr3 const & construct = equalConstructs.front();

        if (IsStructureValid())
        {
          DoStartIteration(construct);
        }
        else
        {
          DoStartIteration(construct);
        }

        break;
      }
    }
  }

  ScTemplate::Result operator () (ScTemplateSearchResult & result)
  {
    result.Clear();

    result.m_replacements = m_template.m_replacements;

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

  using ScAddrSet = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;
  ScAddrSet m_structCache;

  using UsedEdges = std::set<ScAddr, ScAddLessFunc>;
  UsedEdges m_usedEdges;

  std::map<std::string, ScTemplateConstr3 *> m_itemsToConstructs;
  std::map<std::string, size_t> m_itemsToItems;

  using ReplRefs = std::vector<uint32_t>;
  ReplRefs m_replRefs;
};

ScTemplate::Result ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(*this, ctx, ScAddr());
  return search(result);
}

ScTemplate::Result ScTemplate::SearchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(*this, ctx, scStruct);
  return search(result);
}
