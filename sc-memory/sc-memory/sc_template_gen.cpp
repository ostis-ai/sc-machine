/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

const ScTemplateParams ScTemplateParams::Empty;

class ScTemplateGenerator
{
public:
  ScTemplateGenerator(
      ScTemplate::ReplacementsMap const & replacements,
      ScTemplate::TemplateConstr3Vector const & constructions,
      ScTemplateParams const & params,
      ScMemoryContext & context)
    : m_replacements(replacements)
    , m_constructions(constructions)
    , m_params(params)
    , m_context(context)
  {
    // check if it is valid
    for (const auto & constr : m_constructions)
    {
      auto values = constr.GetValues();
      if (values[1].IsFixed())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams, "You can't use fixed value for edge in triple for template generation");

      auto checkEdge = [](ScTemplateItemValue const & value) {
        if (value.IsAssign() && (!value.IsType() || value.m_typeValue.IsEdge()))
          SC_THROW_EXCEPTION(
              utils::ExceptionInvalidParams, "You can't use edges as a source/target element in triple for generation");
      };

      checkEdge(values[0]);
      checkEdge(values[2]);
    }
  }

  ScTemplateResultCode operator()(ScTemplateGenResult & result)
  {
    if (!checkParams())
      return ScTemplateResultCode::InvalidParams;  /// TODO: Provide error

    ScMemoryContextEventsPendingGuard guard(m_context);

    result.m_result.resize(m_constructions.size() * 3);
    result.m_replacements = m_replacements;

    ScAddrVector createdElements;
    size_t resultIdx = 0;
    bool isError = false;

    for (const auto & item : m_constructions)
    {
      auto const & values = item.GetValues();

      // check that the third argument isn't a command to generate edge
      SC_ASSERT(!(values[2].m_itemType == ScTemplateItemValue::Type::Type && values[2].m_typeValue.IsEdge()), ());
      // check that second command is to generate edge
      SC_ASSERT(values[1].m_itemType == ScTemplateItemValue::Type::Type && values[1].m_typeValue.IsEdge(), ());
      // the second item couldn't be a replacement
      SC_ASSERT(values[1].m_itemType != ScTemplateItemValue::Type::Replace, ());

      ScAddr const addr1 = resolveAddr(values[0], result.m_result);
      SC_ASSERT(addr1.IsValid(), ());
      ScAddr const addr2 = resolveAddr(values[2], result.m_result);
      SC_ASSERT(addr2.IsValid(), ());

      if (!addr1.IsValid() || !addr2.IsValid())
      {
        isError = true;
        break;
      }

      ScAddr const edge = m_context.CreateEdge(values[1].m_typeValue.UpConstType(), addr1, addr2);
      if (!edge.IsValid())
      {
        isError = true;
        break;
      }

      result.m_result[resultIdx++] = addr1;
      result.m_result[resultIdx++] = edge;
      result.m_result[resultIdx++] = addr2;
    }

    if (isError)
    {
      cleanupCreated();
      return ScTemplateResultCode::InternalError;
    }

    return ScTemplateResultCode::Success;
  }

  ScAddr createNodeLink(ScType const & type)
  {
    ScAddr addr;
    if (type.IsNode())
    {
      addr = m_context.CreateNode(type);
    }
    else if (type.IsLink())
    {
      addr = m_context.CreateLink();
    }

    if (addr.IsValid())
      m_createdElements.push_back(addr);

    return addr;
  }

  ScAddr resolveAddr(ScTemplateItemValue const & itemValue, ScAddrVector const & resultAddrs)
  {
    /// TODO: improve speed, because not all time we need to replace by params
    // replace by value from params
    if (!m_params.IsEmpty() && !itemValue.m_replacementName.empty())
    {
      ScAddr result;
      if (m_params.Get(itemValue.m_replacementName, result))
        return result;
    }

    switch (itemValue.m_itemType)
    {
    case ScTemplateItemValue::Type::Addr:
      return itemValue.m_addrValue;
    case ScTemplateItemValue::Type::Type:
      return createNodeLink(itemValue.m_typeValue.UpConstType());
    case ScTemplateItemValue::Type::Replace:
    {
      auto it = m_replacements.find(itemValue.m_replacementName);
      if (it != m_replacements.end())
      {
        SC_ASSERT(it->second < resultAddrs.size(), ());
        return resultAddrs[it->second];
      }
    }
    default:
      break;
    }

    return {};
  }

  void cleanupCreated()
  {
    for (auto & m_createdElement : m_createdElements)
      m_context.EraseElement(m_createdElement);
    m_createdElements.clear();
  }

  bool checkParams() const
  {
    for (auto const & it : m_params.m_values)
    {
      ScTemplate::ReplacementsMap::const_iterator const itRepl = m_replacements.find(it.first);

      if (itRepl == m_replacements.end())
        return false;

      ScTemplateConstr3 const & constr = m_constructions[itRepl->second / 3];
      ScType const & itemType = constr.GetValues()[itRepl->second % 3].m_typeValue;
      /// TODO: check subtype of objects. Can't replace tuple with no tuple object
      if (itemType.HasConstancyFlag() && (!itemType.IsVar() || itemType.IsEdge()))
        return false;
    }

    return true;
  }

private:
  ScTemplate::ReplacementsMap const & m_replacements;
  ScTemplate::TemplateConstr3Vector const & m_constructions;
  ScTemplateParams const & m_params;
  ScMemoryContext & m_context;
  ScAddrVector m_createdElements;
};

ScTemplate::Result ScTemplate::Generate(
    ScMemoryContext & ctx,
    ScTemplateGenResult & result,
    ScTemplateParams const & params,
    ScTemplateResultCode * errorCode) const
{
  ScTemplateGenerator gen(m_replacements, m_constructions, params, ctx);
  ScTemplateResultCode resultCode = gen(result);

  if (errorCode)
    *errorCode = resultCode;

  return ScTemplate::Result(resultCode == ScTemplateResultCode::Success);
}
