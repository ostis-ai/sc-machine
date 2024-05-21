/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

ScTemplateParams const ScTemplateParams::Empty;

class ScTemplateGenerator
{
public:
  ScTemplateGenerator(
      ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & replacements,
      ScTemplate::ScTemplateTriplesVector const & triples,
      ScTemplateParams const & params,
      ScMemoryContext & context)
    : m_replacements(replacements)
    , m_triples(triples)
    , m_params(params)
    , m_context(context)
  {
  }

  ScTemplateResultCode operator()(ScTemplateGenResult & result)
  {
    ScMemoryContextEventsPendingGuard guard(m_context);

    PreCheckTemplateAndParams();

    result = ScTemplateResultItem{*m_context, m_replacements};
    result.m_replacementConstruction.resize(m_triples.size() * 3);

    ScAddrVector createdElements;
    size_t resultIdx = 0;

    for (auto const & triple : m_triples)
    {
      auto const & items = triple->GetValues();
      ScTemplateItem const & sourceItem = items[0];
      ScTemplateItem const & connectorItem = items[1];
      ScTemplateItem const & targetItem = items[2];

      if (sourceItem.IsType() && sourceItem.m_typeValue.IsUnknown())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-element with unknown sc-type as the first element of triple.");

      ScAddr sourceAddr = TryFindElementReplacement(sourceItem, result.m_replacementConstruction);
      if (sourceItem.IsType() && sourceItem.m_typeValue.IsEdge() && !sourceAddr.IsValid())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-connector as the first item of triple without specifying source and target "
            "sc-elements of this sc-connector.");

      if (targetItem.IsType() && targetItem.m_typeValue.IsUnknown())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-element with unknown sc-type as the third element of triple.");

      ScAddr targetAddr = TryFindElementReplacement(targetItem, result.m_replacementConstruction);
      if (targetItem.IsType() && targetItem.m_typeValue.IsEdge() && !targetAddr.IsValid())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-connector as the third item of triple without specifying source and target "
            "sc-elements of this sc-connector.");

      if (targetItem.IsType() && targetItem.m_typeValue.IsUnknown())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-element with unknown sc-type as the second element of triple.");

      if (!(sourceItem.IsAddr() && targetItem.IsAddr())
          && !(connectorItem.IsType() && connectorItem.m_typeValue.IsEdge()))
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "The second element of triple must have variable type to generate sc-connector, because the first and "
            "third elements of triple have variable types. You can't generate source and target for constant "
            "sc-connector because this sc-connector already has source and target sc-elements.");

      if (!sourceAddr.IsValid())
        sourceAddr = CreateNodeOrLink(sourceItem.m_typeValue.UpConstType());
      if (!targetAddr.IsValid())
        targetAddr = CreateNodeOrLink(targetItem.m_typeValue.UpConstType());

      ScAddr connectorAddr = TryFindElementReplacement(connectorItem, result.m_replacementConstruction);
      if (!connectorAddr.IsValid() && !connectorItem.IsReplacement())
        connectorAddr = m_context.CreateEdge(connectorItem.m_typeValue.UpConstType(), sourceAddr, targetAddr);
      else
        m_context.GetEdgeInfo(connectorAddr, sourceAddr, targetAddr);

      result.m_replacementConstruction[resultIdx++] = sourceAddr;
      result.m_replacementConstruction[resultIdx++] = connectorAddr;
      result.m_replacementConstruction[resultIdx++] = targetAddr;
    }

    return ScTemplateResultCode::Success;
  }

  void CleanupCreatedElements()
  {
    for (auto & m_createdElement : m_createdElements)
      m_context.EraseElement(m_createdElement);
    m_createdElements.clear();
  }

private:
  ScAddr CreateNodeOrLink(ScType const & type)
  {
    ScAddr addr;
    if (type.IsLink())
      addr = m_context.CreateLink();
    else
      addr = m_context.CreateNode(type);

    m_createdElements.push_back(addr);

    return addr;
  }

  [[nodiscard]] ScAddr GetAddrFromParams(ScTemplateItem const & itemValue) const
  {
    ScAddr result;
    if (m_params.Get(itemValue.m_name, result))
      return result;

    std::stringstream stream(itemValue.m_name);
    sc_addr_hash hash;
    stream >> hash;
    if (stream.fail() || !stream.eof())
      return ScAddr::Empty;

    ScAddr const & varAddr = ScAddr(hash);
    if (!varAddr.IsValid() || !m_context.IsElement(varAddr))
      return ScAddr::Empty;

    std::string const & name = m_context.HelperGetSystemIdtf(varAddr);
    m_params.Get(name, result);

    return result;
  }

  ScAddr TryFindElementReplacement(ScTemplateItem const & item, ScAddrVector const & resultAddrs)
  {
    // replace by value from params
    if (!m_params.IsEmpty() && !item.m_name.empty())
    {
      ScAddr const & addr = GetAddrFromParams(item);
      if (addr.IsValid())
        return addr;
    }

    if (item.IsAddr())
      return item.m_addrValue;

    if (item.IsReplacement())
    {
      auto it = m_replacements.find(item.m_name);
      if (it != m_replacements.cend())
        return resultAddrs[it->second];
    }

    return ScAddr::Empty;
  }

  void PreCheckTemplateAndParams() const
  {
    auto const & CheckCorrespondenceBetweenTemplateParamReplacementNameAndTemplateItemReplacementName =
        [&](std::string const & templateParamReplacementName, size_t & templateItemPosition)
    {
      ScAddr varAddr;

      auto replacementIt = m_replacements.find(templateParamReplacementName);
      if (replacementIt != m_replacements.cend())
        goto end;

      varAddr = m_context.HelperFindBySystemIdtf(templateParamReplacementName);
      if (!varAddr.IsValid())
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");

      replacementIt = m_replacements.find(std::to_string(varAddr.Hash()));
      if (replacementIt == m_replacements.cend())
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");

    end:
      templateItemPosition = replacementIt->second;
    };

    auto const & CheckIncidenceBetweenConnectorAndIncidentElements =
        [&](ScTemplateItem const & connectorItem, ScTemplateItem const & sourceItem, ScTemplateItem const & targetItem)
    {
      ScAddr const & connectorAddr = connectorItem.m_addrValue;

      ScAddr foundSourceAddr, foundTargetAddr;
      m_context.GetEdgeInfo(connectorAddr, foundSourceAddr, foundTargetAddr);

      if (sourceItem.IsAddr() && sourceItem.m_addrValue != foundSourceAddr)
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");

      if (sourceItem.HasName())
      {
        auto const & itemIt = m_params.m_templateItemsToParams.find(sourceItem.m_name);
        if (itemIt != m_params.m_templateItemsToParams.cend() && sourceItem.m_addrValue != itemIt->second)
          SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");
      }

      if (targetItem.IsAddr() && targetItem.m_addrValue != foundTargetAddr)
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");

      if (targetItem.HasName())
      {
        auto const & itemIt = m_params.m_templateItemsToParams.find(targetItem.m_name);
        if (itemIt != m_params.m_templateItemsToParams.cend() && targetItem.m_addrValue != itemIt->second)
          SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");
      }
    };

    auto const & CheckTemplateItemTypeAndTemplateParamType =
        [&](ScTemplateItem const & templateItem, ScAddr const & templateParamAddr)
    {
      ScType const & templateItemType = templateItem.m_typeValue;
      if (templateItemType.HasConstancyFlag() && !templateItemType.IsVar())
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");

      ScType const & templateParamType = m_context.GetElementType(templateParamAddr);

      if (templateItemType.UpConstType().CanExtendTo(templateParamType))
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "");
    };

    for (auto const & item : m_params.m_templateItemsToParams)
    {
      std::string const & templateParamReplacementName = item.first;

      size_t templateItemPosition;
      CheckCorrespondenceBetweenTemplateParamReplacementNameAndTemplateItemReplacementName(
          templateParamReplacementName, templateItemPosition);

      ScTemplateTriple * triple = m_triples[templateItemPosition / 3];
      auto const & items = triple->GetValues();

      size_t const templateItemPositionInTriple = templateItemPosition % 3;
      ScTemplateItem const & foundItem = items[templateItemPositionInTriple];

      if (templateItemPositionInTriple == 1)
      {
        ScTemplateItem const & sourceItem = items[0];
        ScTemplateItem const & targetItem = items[2];

        CheckIncidenceBetweenConnectorAndIncidentElements(foundItem, sourceItem, targetItem);
      }

      ScAddr const & templateParamAddr = item.second;

      CheckTemplateItemTypeAndTemplateParamType(foundItem, templateParamAddr);
    }
  }

  ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & m_replacements;
  ScTemplate::ScTemplateTriplesVector const & m_triples;
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
  ScTemplateGenerator gen(m_templateItemsNamesToReplacementItemsPositions, m_templateTriples, params, ctx);
  ScTemplateResultCode resultCode;

  try
  {
    resultCode = gen(result);
  }
  catch (utils::ExceptionInvalidParams const & exception)
  {
    gen.CleanupCreatedElements();
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, exception.Message());
  }

  if (errorCode)
    *errorCode = resultCode;

  return ScTemplate::Result(resultCode == ScTemplateResultCode::Success);
}
