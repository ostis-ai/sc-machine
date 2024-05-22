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
            "You can't generate sc-element with unknown sc-type as the first item of triple "
                << (sourceItem.HasName() ? ("`" + sourceItem.m_name + "` ") : "") << ".");

      ScAddr sourceAddr = TryFindElementReplacement(sourceItem, result.m_replacementConstruction);
      if (sourceItem.IsType() && sourceItem.m_typeValue.IsEdge() && !sourceAddr.IsValid())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-connector as the first item of triple "
                << (sourceItem.HasName() ? ("`" + sourceItem.m_name + "` ") : "")
                << "without specifying source and target "
                   "sc-elements of this sc-connector.");

      if (targetItem.IsType() && targetItem.m_typeValue.IsUnknown())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-element with unknown sc-type as the third item of triple "
                << (targetItem.HasName() ? ("`" + targetItem.m_name + "` ") : "") << ".");

      ScAddr targetAddr = TryFindElementReplacement(targetItem, result.m_replacementConstruction);
      if (targetItem.IsType() && targetItem.m_typeValue.IsEdge() && !targetAddr.IsValid())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-connector as the third item of triple "
                << (targetItem.HasName() ? ("`" + targetItem.m_name + "` ") : "")
                << "without specifying source and target "
                   "sc-elements of this sc-connector.");

      if (targetItem.IsType() && targetItem.m_typeValue.IsUnknown())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "You can't generate sc-element with unknown sc-type as the second item of triple "
                << (connectorItem.HasName() ? ("`" + connectorItem.m_name + "` ") : "") << ".");

      ScAddr connectorAddr = TryFindElementReplacement(connectorItem, result.m_replacementConstruction);
      if (connectorAddr.IsValid())
        CheckIncidenceBetweenConnectorAndIncidentElements(connectorItem, connectorAddr, sourceItem, targetItem);

      if (connectorAddr.IsValid())
        m_context.GetEdgeInfo(connectorAddr, sourceAddr, targetAddr);

      if (!sourceAddr.IsValid())
        sourceAddr = CreateNodeOrLink(sourceItem.m_typeValue.UpConstType());
      if (!targetAddr.IsValid())
        targetAddr = CreateNodeOrLink(targetItem.m_typeValue.UpConstType());

      if (!connectorAddr.IsValid())
        connectorAddr = m_context.CreateEdge(connectorItem.m_typeValue.UpConstType(), sourceAddr, targetAddr);

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

  ScAddr TryFindElementReplacement(ScTemplateItem const & item, ScAddrVector const & resultAddrs) const
  {
    // replace by value from params
    if (!m_params.IsEmpty() && item.HasName())
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

  void CheckIncidenceBetweenConnectorAndIncidentElements(
      ScTemplateItem const & connectorItem,
      ScAddr const & connectorAddr,
      ScTemplateItem const & sourceItem,
      ScTemplateItem const & targetItem) const
  {
    ScAddr foundSourceAddr, foundTargetAddr;
    m_context.GetEdgeInfo(connectorAddr, foundSourceAddr, foundTargetAddr);

    if (sourceItem.IsAddr() && sourceItem.m_addrValue != foundSourceAddr)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-connector `" << std::to_string(connectorAddr.Hash())
                                     << "` as parameter for the second item in sc-template "
                                     << (connectorItem.HasName() ? ("`" + connectorItem.m_name + "` ") : "")
                                     << "is not incident to specified source sc-element `"
                                     << std::to_string(sourceItem.m_addrValue.Hash())
                                     << "` as fixed first item in sc-template "
                                     << (sourceItem.HasName() ? ("`" + sourceItem.m_name + "` ") : "")
                                     << ". This sc-connector is incident to sc-element `"
                                     << std::to_string(foundSourceAddr.Hash()) << "`.");

    if (sourceItem.HasName())
    {
      auto const & itemIt = m_params.m_templateItemsToParams.find(sourceItem.m_name);
      if (itemIt != m_params.m_templateItemsToParams.cend() && itemIt->second != foundSourceAddr)
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Specified sc-connector `" << std::to_string(connectorAddr.Hash())
                                       << "` as parameter for the second item in sc-template "
                                       << (connectorItem.HasName() ? ("`" + connectorItem.m_name + "` ") : "")
                                       << "is not incident to specified source sc-element `"
                                       << std::to_string(itemIt->second.Hash())
                                       << "` as parameter for the first item in sc-template "
                                       << (sourceItem.HasName() ? ("`" + sourceItem.m_name + "` ") : "")
                                       << ". This sc-connector is incident to sc-element `"
                                       << std::to_string(foundSourceAddr.Hash()) << "`.");
    }

    if (targetItem.IsAddr() && targetItem.m_addrValue != foundTargetAddr)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-connector `" << std::to_string(connectorAddr.Hash())
                                     << "` as parameter for the second item in sc-template "
                                     << (connectorItem.HasName() ? ("`" + connectorItem.m_name + "` ") : "")
                                     << "is not incident to specified target sc-element `"
                                     << std::to_string(targetItem.m_addrValue.Hash())
                                     << "` as fixed third item in sc-template "
                                     << (targetItem.HasName() ? ("`" + targetItem.m_name + "` ") : "")
                                     << ". This sc-connector is incident to sc-element `"
                                     << std::to_string(foundTargetAddr.Hash()) << "`.");

    if (targetItem.HasName())
    {
      auto const & itemIt = m_params.m_templateItemsToParams.find(targetItem.m_name);
      if (itemIt != m_params.m_templateItemsToParams.cend() && itemIt->second != foundTargetAddr)
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Specified sc-connector `" << std::to_string(connectorAddr.Hash())
                                       << "` as parameter for the second item in sc-template "
                                       << (connectorItem.HasName() ? ("`" + connectorItem.m_name + "` ") : "")
                                       << "is not incident to specified target sc-element `"
                                       << std::to_string(itemIt->second.Hash())
                                       << "` as fixed third item in sc-template "
                                       << (targetItem.HasName() ? ("`" + targetItem.m_name + "` ") : "")
                                       << ". This sc-connector is incident to sc-element `"
                                       << std::to_string(foundTargetAddr.Hash()) << "`.");
    }
  };

  void PreCheckTemplateAndParams() const
  {
    auto const & CheckCorrespondenceBetweenTemplateParamReplacementNameAndTemplateItemReplacementName =
        [&](std::string const & templateParamReplacementName, size_t & templateItemPosition)
    {
      ScAddr varAddr;
      std::string addrHashStr;

      auto replacementIt = m_replacements.find(templateParamReplacementName);
      if (replacementIt != m_replacements.cend())
        goto end;

      varAddr = m_context.HelperFindBySystemIdtf(templateParamReplacementName);
      if (!varAddr.IsValid())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "The given sc-template hasn't item with name `"
                << templateParamReplacementName
                << "` given in parameters, for which you want to perform substitution from these parameters.");

      addrHashStr = std::to_string(varAddr.Hash());
      replacementIt = m_replacements.find(addrHashStr);
      if (replacementIt == m_replacements.cend())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "The given sc-template hasn't item with variable `"
                << addrHashStr
                << "` given in parameters, for which you want to perform substitution from these parameters.");

    end:
      templateItemPosition = replacementIt->second;
    };

    auto const & CheckTemplateItemTypeAndTemplateParamType =
        [&](ScTemplateItem const & templateItem, ScAddr const & templateParamAddr)
    {
      ScType const & templateItemType = templateItem.m_typeValue;
      ScType const & templateParamType = m_context.GetElementType(templateParamAddr);

      if (!templateItemType.UpConstType().CanExtendTo(templateParamType))
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Template item " << (templateItem.HasName() ? ("`" + templateItem.m_name + "` ") : "") << "has type `"
                             << templateItemType << "`. This item can't be replaced by template parameter `"
                             << std::to_string(templateParamAddr.Hash()) << "`, because it has type `"
                             << templateParamType
                             << "` and up-constant template item type can't be extended to template parameter type.");
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

        ScAddr const & foundItemAddr = item.second;
        CheckIncidenceBetweenConnectorAndIncidentElements(foundItem, foundItemAddr, sourceItem, targetItem);
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

  return ScTemplate::Result(true);
}
