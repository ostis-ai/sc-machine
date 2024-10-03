/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_template.hpp"

#include <vector>
#include <unordered_map>
#include <iostream>

#include "sc_template_private.hpp"
#include "sc-memory/sc_memory.hpp"

#include "sc-memory/sc_structure.hpp"

class ScTemplateLoader
{
  friend class ScTemplate;

protected:
  ScTemplateLoader(ScMemoryContext & ctx, ScTemplateParams const & params)
    : m_context(ctx)
    , m_params(params)
  {
  }

  void operator()(ScTemplate * inTemplate, ScAddr & resultTemplateAddr)
  {
    resultTemplateAddr = m_context.GenerateNode(ScType::ConstNodeStructure);
    ScStructure templateStruct(m_context, resultTemplateAddr);

    std::unordered_map<std::string, ScAddr> itemNamesToTemplateElements;
    auto const & ResolveAddr = [this, &templateStruct, &inTemplate, &itemNamesToTemplateElements](
                                   ScTemplateItem const & item,
                                   ScAddr const & sourceAddr = ScAddr::Empty,
                                   ScAddr const & targetAddr = ScAddr::Empty) -> ScAddr
    {
      ScAddr itemAddr;

      if (item.HasName())
      {
        ScAddr replacementAddr;
        if (m_params.Get(item.m_name, replacementAddr))
          itemAddr = replacementAddr;
        else
        {
          auto const & replacementItemsAddrsIt =
              inTemplate->m_templateItemsNamesToReplacementItemsAddrs.find(item.m_name);
          if (replacementItemsAddrsIt != inTemplate->m_templateItemsNamesToReplacementItemsAddrs.cend())
            itemAddr = replacementItemsAddrsIt->second;
          else
          {
            auto const & templateElementsIt = itemNamesToTemplateElements.find(item.m_name);
            if (templateElementsIt != itemNamesToTemplateElements.cend())
              itemAddr = templateElementsIt->second;
          }
        }
      }

      if (!m_context.IsElement(itemAddr))
      {
        if (item.IsAddr())
          itemAddr = item.m_addrValue;
        else if (item.IsType())
        {
          if (m_context.IsElement(sourceAddr) && m_context.IsElement(targetAddr))
            itemAddr = m_context.GenerateConnector(item.m_typeValue, sourceAddr, targetAddr);
          else if (item.m_typeValue.IsLink())
            itemAddr = m_context.GenerateLink(item.m_typeValue);
          else
            itemAddr = m_context.GenerateNode(item.m_typeValue);
        }

        if (item.HasName())
          itemNamesToTemplateElements.insert({item.m_name, itemAddr});
      }

      templateStruct << itemAddr;

      return itemAddr;
    };

    for (ScTemplateTriple * triple : inTemplate->m_templateTriples)
    {
      auto const & values = triple->GetValues();

      ScTemplateItem const & sourceItem = values[0];
      ScTemplateItem const & targetItem = values[2];
      ScTemplateItem const & connectorItem = values[1];

      ScAddr const & sourceAddr = ResolveAddr(sourceItem);
      ScAddr const & targetAddr = ResolveAddr(targetItem);
      ResolveAddr(connectorItem, sourceAddr, targetAddr);
    }
  }

protected:
  ScAddr m_templateAddr;
  ScMemoryContext & m_context;
  ScTemplateParams m_params;
};

void ScTemplate::TranslateTo(ScMemoryContext & ctx, ScAddr & resultTemplateAddr, ScTemplateParams const & params)
{
  ScTemplateLoader loader(ctx, params);
  loader(this, resultTemplateAddr);
}
