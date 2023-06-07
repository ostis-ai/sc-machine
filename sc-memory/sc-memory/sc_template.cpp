/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <algorithm>

extern "C"
{
#include "sc-core/sc_helper.h"
}

#include "sc_template.hpp"
#include "sc_memory.hpp"

SC_DEPRECATED(0.8.0, "Don't use alias for fixed sc-address")
ScTemplateItem operator>>(ScAddr const & value, char const * replName)
{
  return {value, replName};
}

SC_DEPRECATED(0.8.0, "Don't use alias for fixed sc-address")
ScTemplateItem operator>>(ScAddr const & value, std::string const & replName)
{
  return {value, replName.c_str()};
}

ScTemplateItem operator>>(ScType const & value, char const * replName)
{
  return {value, replName};
}

ScTemplateItem operator>>(ScType const & value, std::string const & replName)
{
  return {value, replName.c_str()};
}

// --------------------------------

ScTemplate::ScTemplate()
{
  m_templateTriples.reserve(16);

  auto const tripleTypeCount = (size_t)ScTemplateTripleType::ScConstr3TypeCount;
  m_priorityOrderedTemplateTriples.resize(tripleTypeCount);
}

ScTemplate::ScTemplate(bool forceOrder)
  : ScTemplate()
{
  SC_UNUSED(forceOrder);
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3)
{
  return Triple(param1, param2, param3);
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3,
    ScTemplateItem const & param4,
    ScTemplateItem const & param5)
{
  return TripleWithRelation(param1, param2, param3, param4, param5);
}

void ScTemplate::Clear()
{
  for (auto * triple : m_templateTriples)
    delete triple;
  m_templateTriples.clear();

  m_templateItemsNamesToReplacementItemsAddrs.clear();
  m_priorityOrderedTemplateTriples.clear();
  m_priorityOrderedTemplateTriples.resize((size_t)ScTemplateTripleType::ScConstr3TypeCount);
}

bool ScTemplate::IsEmpty() const
{
  return m_templateTriples.empty();
}

size_t ScTemplate::Size() const
{
  return m_templateTriples.size();
}

bool ScTemplate::HasReplacement(std::string const & repl) const
{
  return m_templateItemsNamesToReplacementItemsPositions.find(repl) !=
         m_templateItemsNamesToReplacementItemsPositions.cend();
}

ScTemplate & ScTemplate::Triple(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3)
{
  size_t const replPos = m_templateTriples.size() * 3;
  m_templateTriples.emplace_back(new ScTemplateTriple(param1, param2, param3, m_templateTriples.size()));

  if (!param2.m_name.empty() && (param2.m_name == param1.m_name || param2.m_name == param3.m_name))
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use equal replacement for an edge and source/target");

  ScTemplateTriple * triple = m_templateTriples.back();

  for (size_t i = 0; i < 3; ++i)
  {
    ScTemplateItem & item = triple->m_values[i];

    if (item.IsAssign() && item.m_typeValue.HasConstancyFlag() && !item.m_typeValue.IsVar())
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You should to use variable types in template");

    if (item.IsAddr() && !item.m_addrValue.IsValid())
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use empty ScAddr");

    if (!item.m_name.empty())
    {
      if (item.IsAddr())
        m_templateItemsNamesToReplacementItemsAddrs[item.m_name] = item.m_addrValue;
      else
      {
        auto const & found = m_templateItemsNamesToReplacementItemsAddrs.find(item.m_name);
        if (found != m_templateItemsNamesToReplacementItemsAddrs.cend())
          item.SetAddr(found->second);
      }

      if (item.IsType())
        m_templateItemsNamesToTypes[item.m_name] = item.m_typeValue;

      if (!item.IsReplacement())
      {
        if (!HasReplacement(item.m_name))
          m_templateItemsNamesToReplacementItemsPositions.insert({item.m_name, replPos + i});
      }

      /* Store type there, if replacement for any type.
       * That allows to use it before original type will be processed
       */
      size_t const tripleIdx = replPos / 3;
      ScTemplateItem const & valueType = m_templateTriples[tripleIdx]->m_values[i];

      if (valueType.IsType())
        item.m_typeValue = valueType.m_typeValue;
    }
  }

  ScTemplateTripleType const priority = GetPriority(triple);
  auto const pr = (size_t)priority;
  m_priorityOrderedTemplateTriples[pr].insert(triple->m_index);

  return *this;
}

ScTemplate & ScTemplate::Quintuple(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3,
    ScTemplateItem const & param4,
    ScTemplateItem const & param5)
{
  size_t const replPos = m_templateTriples.size() * 3;

  ScTemplateItem edgeCommonItem = param2;

  // check if relation edge has replacement
  if (edgeCommonItem.m_name.empty())
  {
    std::stringstream ss;
    ss << "_repl_" << replPos + 1;
    edgeCommonItem.m_name = ss.str();
  }

  Triple(param1, edgeCommonItem, param3);
  Triple(param5, param4, edgeCommonItem.m_name);

  return *this;
}

ScTemplate & ScTemplate::TripleWithRelation(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3,
    ScTemplateItem const & param4,
    ScTemplateItem const & param5)
{
  return Quintuple(param1, param2, param3, param4, param5);
}

inline ScTemplateTripleType ScTemplate::GetPriority(ScTemplateTriple * triple)
{
  ScTemplateItem const & item1 = triple->m_values[0];
  ScTemplateItem const & item2 = triple->m_values[1];
  ScTemplateItem const & item3 = triple->m_values[2];

  if (item2.IsFixed())
    return ScTemplateTripleType::AFA;

  if (item1.IsFixed() && item3.IsFixed())
    return ScTemplateTripleType::FAF;

  if (item3.IsFixed())
    return ScTemplateTripleType::AAF;

  if (item1.IsFixed() && (!item3.m_typeValue.IsEdge() || item3.m_typeValue.IsUnknown()))
  {
    auto const & it = m_templateItemsNamesToTypes.find(item3.m_name);
    if (it != m_templateItemsNamesToTypes.cend() && !it->second.IsEdge() && !it->second.IsUnknown())
      return ScTemplateTripleType::FAN;
  }

  if (item1.IsFixed())
    return ScTemplateTripleType::FAE;

  return ScTemplateTripleType::AAA;
}

ScAddr ScTemplateResultItem::GetAddrBySystemIdtf(std::string const & name) const
{
  sc_addr _addr;
  sc_helper_find_element_by_system_identifier(m_context, name.c_str(), name.size(), &_addr);
  return {_addr};
}

std::string ScTemplateResultItem::GetSystemIdtfByAddr(ScAddr const & addr) const
{
  sc_addr _link_addr;
  sc_helper_get_system_identifier_link(m_context, addr.GetRealAddr(), &_link_addr);
  ScAddr linkAddr{_link_addr};
  if (!linkAddr.IsValid())
    return "";

  sc_stream * stream;
  sc_memory_get_link_content(m_context, _link_addr, &stream);

  std::string idtf;
  ScStreamConverter::StreamToString(std::make_shared<ScStream>(stream), idtf);

  return idtf;
}

ScTemplate::ScTemplateItemsToReplacementsItemsPositions ScTemplateSearchResult::GetReplacements() const noexcept
{
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacementsItemsPositions;

  for (auto const & item : m_templateItemsNamesToReplacementItemsPositions)
  {
    replacementsItemsPositions.insert(item);

    std::stringstream ss(item.first);
    sc_addr_hash hash;
    ss >> hash;
    if (ss.fail() || !ss.eof())
      continue;

    ScAddr const & varAddr = ScAddr(hash);
    sc_addr _link_addr;
    sc_helper_get_system_identifier_link(m_context, varAddr.GetRealAddr(), &_link_addr);
    if (SC_ADDR_IS_EMPTY(_link_addr) || !sc_memory_is_element(m_context, _link_addr))
      continue;

    sc_stream * stream;
    sc_memory_get_link_content(m_context, _link_addr, &stream);

    std::string idtf;
    ScStreamConverter::StreamToString(std::make_shared<ScStream>(stream), idtf);

    replacementsItemsPositions.insert({idtf, item.second});
  }

  return replacementsItemsPositions;
}
