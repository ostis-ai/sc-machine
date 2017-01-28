/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

ScTemplate::ScTemplate(size_t BufferedNum)
  : m_isCacheValid(false)
{
  m_constructions.reserve(BufferedNum);
  m_currentReplPos = 0;
}

ScTemplate & ScTemplate::operator() (ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3)
{
  return Triple(param1, param2, param3);
}

ScTemplate & ScTemplate::operator() (ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3,
                                     ScTemplateItemValue const & param4, ScTemplateItemValue const & param5)
{
  return TripleWithRelation(param1, param2, param3, param4, param5);
}

void ScTemplate::Clear()
{
  m_constructions.clear();
  m_replacements.clear();
  m_currentReplPos = 0;

  m_isCacheValid = false;
}

bool ScTemplate::IsSearchCacheValid() const
{
  return (m_isCacheValid && (m_searchCachedOrder.size() == m_constructions.size()));
}

bool ScTemplate::IsGenerateCacheValid() const
{
  return (m_isCacheValid && (m_generateCachedOrder.size() == m_constructions.size()));
}

bool ScTemplate::HasReplacement(std::string const & repl) const
{
  return (m_replacements.find(repl) != m_replacements.end());
}

ScTemplate & ScTemplate::Triple(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3)
{
  size_t const replPos = m_constructions.size() * 3;
  m_constructions.emplace_back(ScTemplateConstr3(param1, param2, param3, m_constructions.size()));

  ScTemplateConstr3 & constr3 = m_constructions.back();
  for (size_t i = 0; i < 3; ++i)
  {
    ScTemplateItemValue & value = constr3.m_values[i];
    if (value.m_itemType == ScTemplateItemValue::Type::Type && !value.m_typeValue.IsVar())
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You should to use variable types in template");

    if (!value.m_replacementName.empty())
    {
      if (value.m_itemType != ScTemplateItemValue::Type::Replace)
        m_replacements[value.m_replacementName] = replPos + i;

      /* Store type there, if replacement for any type.
      * That allows to use it before original type will processed
      */
      size_t const constrIdx = replPos / 3;
      SC_ASSERT(constrIdx < m_constructions.size(), ());
      ScTemplateItemValue const & valueType = m_constructions[constrIdx].m_values[i];

      if (valueType.IsType())
        value.m_typeValue = valueType.m_typeValue;
    }
  }

  m_isCacheValid = false;

  return *this;
}

ScTemplate & ScTemplate::TripleWithRelation(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3,
                                            ScTemplateItemValue const & param4, ScTemplateItemValue const & param5)
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

  Triple(param1, edgeCommonItem, param3);
  Triple(param5, param4, edgeCommonItem.m_replacementName.c_str());

  return *this;
}
