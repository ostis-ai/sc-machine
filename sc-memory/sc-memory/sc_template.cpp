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
  m_constructions.reserve(16);

  auto const constr3TypeCount = (size_t)ScConstr3Type::ScConstr3TypeCount;
  m_orderedConstructions.reserve(constr3TypeCount);

  for (size_t i = 0; i < constr3TypeCount; ++i)
  {
    m_orderedConstructions.emplace_back();
  }
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3,
    ScTemplate::TripleFlag isRequired /* = ScTemplate::TripleFlag::Required */)
{
  return Triple(param1, param2, param3, isRequired);
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3,
    ScTemplateItemValue const & param4,
    ScTemplateItemValue const & param5,
    ScTemplate::TripleFlag isRequired /* = ScTemplate::TripleFlag::Required */)
{
  return TripleWithRelation(param1, param2, param3, param4, param5, isRequired);
}

void ScTemplate::Clear()
{
  for (auto * construct : m_constructions)
    delete construct;
  m_constructions.clear();

  m_namesToAddrs.clear();
  m_orderedConstructions.clear();
}

bool ScTemplate::IsEmpty() const
{
  return m_constructions.empty();
}

bool ScTemplate::HasReplacement(std::string const & repl) const
{
  return (m_replacements.find(repl) != m_replacements.end());
}

ScTemplate & ScTemplate::Triple(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3,
    ScTemplate::TripleFlag isRequired /* = ScTemplate::TripleFlag::Required */)
{
  size_t const replPos = m_constructions.size() * 3;
  m_constructions.emplace_back(new ScTemplateConstr3(param1, param2, param3, m_constructions.size(), isRequired));

  if (!param2.m_replacementName.empty() &&
      (param2.m_replacementName == param1.m_replacementName || param2.m_replacementName == param3.m_replacementName))
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use equal replacement for an edge and source/target");

  ScTemplateConstr3 * constr3 = m_constructions.back();
  for (size_t i = 0; i < 3; ++i)
  {
    ScTemplateItemValue & value = constr3->m_values[i];

    if (value.IsAssign() && value.m_typeValue.HasConstancyFlag() && !value.m_typeValue.IsVar())
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You should to use variable types in template");

    if (value.IsAddr() && !value.m_addrValue.IsValid())
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use empty ScAddr");

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

      m_itemsNames.insert(value.m_replacementName);

      /* Store type there, if replacement for any type.
       * That allows to use it before original type will be processed
       */
      size_t const constrIdx = replPos / 3;
      SC_ASSERT(constrIdx < m_constructions.size(), ());
      ScTemplateItemValue const & valueType = m_constructions[constrIdx]->m_values[i];

      if (valueType.IsType())
        value.m_typeValue = valueType.m_typeValue;
    }
  }

  ScConstr3Type const priority = GetPriority(constr3);
  size_t const pr = int(priority);
  m_orderedConstructions[pr].insert(constr3->m_index);

  return *this;
}

ScTemplate & ScTemplate::TripleWithRelation(
    ScTemplateItemValue const & param1,
    ScTemplateItemValue const & param2,
    ScTemplateItemValue const & param3,
    ScTemplateItemValue const & param4,
    ScTemplateItemValue const & param5,
    ScTemplate::TripleFlag isRequired /* = ScTemplate::TripleFlag::Required */)
{
  size_t const replPos = m_constructions.size() * 3;

  ScTemplateItemValue edgeCommonItem = param2;

  // check if relation edge has replacement
  if (edgeCommonItem.m_replacementName.empty())
  {
    std::stringstream ss;
    ss << "_repl_" << replPos + 1;
    edgeCommonItem.m_replacementName = ss.str();
  }

  Triple(param1, edgeCommonItem, param3, isRequired);
  Triple(param5, param4, edgeCommonItem.m_replacementName.c_str(), isRequired);

  return *this;
}

inline ScConstr3Type ScTemplate::GetPriority(ScTemplateConstr3 * constr)
{
  ScTemplateItemValue const & item1 = constr->m_values[0];
  ScTemplateItemValue const & item2 = constr->m_values[1];
  ScTemplateItemValue const & item3 = constr->m_values[2];

  if (item2.IsFixed())
    return ScConstr3Type::AFA;

  else if (item1.IsFixed() && item3.IsFixed())
    return ScConstr3Type::FAF;

  else if (item3.IsFixed())
    return ScConstr3Type::AAF;

  else if (item1.IsFixed() && (!item3.m_typeValue.IsEdge() && !item3.m_typeValue.IsUnknown()))
  {
    auto const & it = m_replacements.find(item3.m_replacementName);
    if (it != m_replacements.cend())
    {
      size_t const constrIdx = it->second / 3;
      if (constrIdx == constr->m_index ||
          (constrIdx >= m_constructions.size() && !m_constructions[constrIdx]->m_values[1].m_typeValue.IsEdge()))
      {
        return ScConstr3Type::FAE;
      }
    }
  }

  if (item1.IsFixed())
    return ScConstr3Type::FAN;

  else
    return ScConstr3Type::AAA;
}
