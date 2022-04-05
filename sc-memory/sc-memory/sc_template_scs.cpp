/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"
#include "sc_debug.hpp"

#include "scs/scs_parser.hpp"

#include "utils/sc_keynode_cache.hpp"

class ScTemplateBuilderFromScs
{
public:
  ScTemplateBuilderFromScs(std::string const & scsText, ScMemoryContext & ctx)
    : m_scsText(scsText)
    , m_ctx(ctx)
    , m_parser()
  {
  }

  ScTemplate::Result operator()(ScTemplate * templ)
  {
    // mark template to don't force order of triples
    templ->m_isForceOrder = false;

    if (!m_parser.Parse(m_scsText))
      return ScTemplate::Result(false, m_parser.GetParseError());

    return BuildImpl(templ);
  }

protected:
  ScTemplate::Result BuildImpl(ScTemplate * templ) const
  {
    utils::ScKeynodeCache keynodes(m_ctx);
    std::unordered_set<std::string> passed;

    ScTemplate::Result result(true);

    auto const MakeTemplItem = [&passed, &keynodes, &result](
                                   scs::ParsedElement const & el, ScTemplateItemValue & outValue) -> bool {
      std::string const & idtf = el.GetIdtf();
      bool const isUnnamed = scs::TypeResolver::IsUnnamed(idtf);
      bool const isPassed = (passed.find(idtf) != passed.end());

      if (!isUnnamed && isPassed)
      {
        outValue.SetReplacement(idtf.c_str());
      }
      else
      {
        char const * alias = (isUnnamed ? nullptr : idtf.c_str());
        ScAddr const addr = keynodes.GetKeynode(idtf);
        if (addr.IsValid())
        {
          outValue.SetAddr(addr, alias);
        }
        else
        {
          if (el.GetType().IsVar())
          {
            outValue.SetType(el.GetType(), alias);
          }
          else
          {
            result = ScTemplate::Result(false, "Can't find element " + idtf);
            return false;
          }
        }
      }

      return true;
    };

    for (scs::ParsedTriple const & t : m_parser.GetParsedTriples())
    {
      scs::ParsedElement const & src = m_parser.GetParsedElement(t.m_source);
      scs::ParsedElement const & edge = m_parser.GetParsedElement(t.m_edge);
      scs::ParsedElement const & trg = m_parser.GetParsedElement(t.m_target);

      ScTemplateItemValue srcItem, edgeItem, trgItem;

      if (!MakeTemplItem(src, srcItem) || !MakeTemplItem(edge, edgeItem) || !MakeTemplItem(trg, trgItem))
      {
        break;
      }

      templ->Triple(srcItem, edgeItem, trgItem);
    }

    return result;
  }

private:
  std::string const & m_scsText;
  ScMemoryContext & m_ctx;

  scs::Parser m_parser;
};

ScTemplate::Result ScTemplate::FromScs(ScMemoryContext & ctx, std::string const & scsText)
{
  ScTemplateBuilderFromScs builder(scsText, ctx);
  return builder(this);
}
