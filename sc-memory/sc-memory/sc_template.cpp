/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

ScTemplateItem operator>>(ScAddr const & value, char const * replName)
{
  return {value, replName};
}

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

ScTemplate::ScTemplate(bool forceOrder /* = false */)
{
  m_templateTriples.reserve(16);

  auto const tripleTypeCount = (size_t)ScTemplateTripleType::ScConstr3TypeCount;
  m_priorityOrderedTemplateTriples.resize(tripleTypeCount);
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

bool ScTemplate::HasReplacement(std::string const & repl) const
{
  return (
      m_templateItemsNamesToReplacementItemsPositions.find(repl) !=
      m_templateItemsNamesToReplacementItemsPositions.end());
}

ScTemplate & ScTemplate::Triple(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3)
{
  size_t const replPos = m_templateTriples.size() * 3;
  m_templateTriples.emplace_back(new ScTemplateTriple(param1, param2, param3, m_templateTriples.size()));

  if (!param2.m_name.empty() && (param2.m_name == param1.m_name || param2.m_name == param3.m_name))
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use equal replacement for an edge and source/target");
  }

  ScTemplateTriple * triple = m_templateTriples.back();

  for (size_t i = 0; i < 3; ++i)
  {
    ScTemplateItem & value = triple->m_values[i];

    if (value.IsAssign() && value.m_typeValue.HasConstancyFlag() && !value.m_typeValue.IsVar())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You should to use variable types in template");
    }

    if (value.IsAddr() && !value.m_addrValue.IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use empty ScAddr");
    }

    if (!value.m_name.empty())
    {
      if (value.IsAddr())
      {
        m_templateItemsNamesToReplacementItemsAddrs[value.m_name] = value.m_addrValue;
      }
      else
      {
        auto const & found = m_templateItemsNamesToReplacementItemsAddrs.find(value.m_name);
        if (found != m_templateItemsNamesToReplacementItemsAddrs.cend())
        {
          value.SetAddr(found->second);
        }
      }

      if (value.IsType())
      {
        m_templateItemsNamesToTypes[value.m_name] = value.m_typeValue;
      }

      if (value.m_itemType != ScTemplateItem::Type::Replace)
      {
        if (m_templateItemsNamesToReplacementItemsPositions.find(value.m_name) ==
            m_templateItemsNamesToReplacementItemsPositions.end())
          m_templateItemsNamesToReplacementItemsPositions.insert({value.m_name, replPos + i});
      }

      /* Store type there, if replacement for any type.
       * That allows to use it before original type will be processed
       */
      size_t const tripleIdx = replPos / 3;
      ScTemplateItem const & valueType = m_templateTriples[tripleIdx]->m_values[i];

      if (valueType.IsType())
        value.m_typeValue = valueType.m_typeValue;
    }
  }

  ScTemplateTripleType const priority = GetPriority(triple);
  auto const pr = (size_t)priority;
  m_priorityOrderedTemplateTriples[pr].insert(triple->m_index);

  return *this;
}

ScTemplate & ScTemplate::TripleWithRelation(
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
  Triple(param5, param4, edgeCommonItem.m_name.c_str());

  return *this;
}

inline ScTemplateTripleType ScTemplate::GetPriority(ScTemplateTriple * triple)
{
  ScTemplateItem const & item1 = triple->m_values[0];
  ScTemplateItem const & item2 = triple->m_values[1];
  ScTemplateItem const & item3 = triple->m_values[2];

  if (item2.IsFixed())
    return ScTemplateTripleType::AFA;

  else if (item1.IsFixed() && item3.IsFixed())
    return ScTemplateTripleType::FAF;

  else if (item3.IsFixed())
    return ScTemplateTripleType::AAF;

  else if (item1.IsFixed() && (!item3.m_typeValue.IsEdge() && !item3.m_typeValue.IsUnknown()))
  {
    auto const & it = m_templateItemsNamesToReplacementItemsPositions.find(item3.m_name);
    if (it != m_templateItemsNamesToReplacementItemsPositions.cend())
    {
      size_t const tripleIdx = it->second / 3;
      if (tripleIdx == triple->m_index ||
          (tripleIdx >= m_templateTriples.size() && !m_templateTriples[tripleIdx]->m_values[1].m_typeValue.IsEdge()))
      {
        return ScTemplateTripleType::FAE;
      }
    }
  }

  if (item1.IsFixed())
    return ScTemplateTripleType::FAN;

  else
    return ScTemplateTripleType::AAA;
}
