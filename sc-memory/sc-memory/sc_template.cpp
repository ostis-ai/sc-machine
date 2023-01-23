/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

ScTemplateItemValue operator>>(ScAddr const & value, char const * replName)
{
  return {value, replName};
}

ScTemplateItemValue operator>>(ScAddr const & value, std::string const & replName)
{
  return {value, replName.c_str()};
}

ScTemplateItemValue operator>>(ScType const & value, char const * replName)
{
  return {value, replName};
}

ScTemplateItemValue operator>>(ScType const & value, std::string const & replName)
{
  return {value, replName.c_str()};
}

// --------------------------------

ScTemplate::ScTemplate(bool forceOrder /* = false */)
{
  m_triples.reserve(16);

  auto const tripleTypeCount = (size_t)ScTemplateTripleType::ScConstr3TypeCount;
  m_orderedTriples.resize(tripleTypeCount);
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3)
{
  return Triple(param1, param2, param3);
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3,
    ScTemplateItemValue const & param4,
    ScTemplateItemValue const & param5)
{
  return TripleWithRelation(param1, param2, param3, param4, param5);
}

void ScTemplate::Clear()
{
  for (auto * triple : m_triples)
    delete triple;
  m_triples.clear();

  m_namesToAddrs.clear();
  m_orderedTriples.clear();
  m_orderedTriples.resize((size_t)ScTemplateTripleType::ScConstr3TypeCount);
}

bool ScTemplate::IsEmpty() const
{
  return m_triples.empty();
}

bool ScTemplate::HasReplacement(std::string const & repl) const
{
  return (m_replacements.find(repl) != m_replacements.end());
}

ScTemplate & ScTemplate::Triple(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3)
{
  size_t const replPos = m_triples.size() * 3;
  m_triples.emplace_back(new ScTemplateTriple(param1, param2, param3, m_triples.size()));

  if (!param2.m_replacementName.empty() &&
      (param2.m_replacementName == param1.m_replacementName || param2.m_replacementName == param3.m_replacementName))
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use equal replacement for an edge and source/target");
  }

  ScTemplateTriple * triple = m_triples.back();

  for (size_t i = 0; i < 3; ++i)
  {
    ScTemplateItemValue & value = triple->m_values[i];

    if (value.IsAssign() && value.m_typeValue.HasConstancyFlag() && !value.m_typeValue.IsVar())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You should to use variable types in template");
    }

    if (value.IsAddr() && !value.m_addrValue.IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use empty ScAddr");
    }

    if (!value.m_replacementName.empty())
    {
      if (value.IsAddr())
      {
        m_namesToAddrs[value.m_replacementName] = value.m_addrValue;
      }
      else
      {
        auto const & found = m_namesToAddrs.find(value.m_replacementName);
        if (found != m_namesToAddrs.cend())
        {
          value.SetAddr(found->second);
        }
      }

      if (value.IsType())
      {
        m_namesToTypes[value.m_replacementName] = value.m_typeValue;
      }

      if (value.m_itemType != ScTemplateItemValue::Type::Replace)
      {
        if (m_replacements.find(value.m_replacementName) == m_replacements.end())
          m_replacements.insert({value.m_replacementName, replPos + i});
      }

      /* Store type there, if replacement for any type.
       * That allows to use it before original type will be processed
       */
      size_t const tripleIdx = replPos / 3;
      ScTemplateItemValue const & valueType = m_triples[tripleIdx]->m_values[i];

      if (valueType.IsType())
        value.m_typeValue = valueType.m_typeValue;
    }
  }

  ScTemplateTripleType const priority = GetPriority(triple);
  auto const pr = (size_t)priority;
  m_orderedTriples[pr].insert(triple->m_index);

  return *this;
}

ScTemplate & ScTemplate::TripleWithRelation(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3,
    ScTemplateItemValue const & param4,
    ScTemplateItemValue const & param5)
{
  size_t const replPos = m_triples.size() * 3;

  ScTemplateItemValue edgeCommonItem = param2;

  // check if relation edge has replacement
  if (edgeCommonItem.m_replacementName.empty())
  {
    std::stringstream ss;
    ss << "_repl_" << replPos + 1;
    edgeCommonItem.m_replacementName = ss.str();
  }

  Triple(param1, edgeCommonItem, param3);
  Triple(param5, param4, edgeCommonItem.m_replacementName.c_str());

  return *this;
}

inline ScTemplateTripleType ScTemplate::GetPriority(ScTemplateTriple * triple)
{
  ScTemplateItemValue const & item1 = triple->m_values[0];
  ScTemplateItemValue const & item2 = triple->m_values[1];
  ScTemplateItemValue const & item3 = triple->m_values[2];

  if (item2.IsFixed())
    return ScTemplateTripleType::AFA;

  else if (item1.IsFixed() && item3.IsFixed())
    return ScTemplateTripleType::FAF;

  else if (item3.IsFixed())
    return ScTemplateTripleType::AAF;

  else if (item1.IsFixed() && (!item3.m_typeValue.IsEdge() && !item3.m_typeValue.IsUnknown()))
  {
    auto const & it = m_replacements.find(item3.m_replacementName);
    if (it != m_replacements.cend())
    {
      size_t const tripleIdx = it->second / 3;
      if (tripleIdx == triple->m_index ||
          (tripleIdx >= m_triples.size() && !m_triples[tripleIdx]->m_values[1].m_typeValue.IsEdge()))
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
