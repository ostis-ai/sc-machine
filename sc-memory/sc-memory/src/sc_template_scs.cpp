/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_template.hpp"

#include "sc-memory/sc_memory.hpp"

#include "sc-memory/scs/scs_parser.hpp"

#include "sc-memory/utils/sc_keynode_cache.hpp"

class ScTemplateBuilderFromScs
{
public:
  ScTemplateBuilderFromScs(std::string const & translatableSCsTemplate, ScMemoryContext & ctx)
    : m_translatableSCsTemplate(translatableSCsTemplate)
    , m_ctx(ctx)
    , m_parser()
  {
  }

  void operator()(ScTemplate * templ)
  {
    if (!m_parser.Parse(m_translatableSCsTemplate))
      SC_THROW_EXCEPTION(utils::ExceptionParseError, m_parser.GetParseError());

    BuildImpl(templ);
  }

protected:
  void BuildImpl(ScTemplate * templ) const
  {
    utils::ScKeynodeCache keynodes(m_ctx);
    std::unordered_set<std::string> passed;

    auto const MakeTemplItem = [&passed, &keynodes](scs::ParsedElement const & el, ScTemplateItem & outValue) -> bool
    {
      std::string const & idtf = el.GetIdtf();
      bool const isUnnamed = scs::TypeResolver::IsUnnamed(idtf);
      bool const isPassed = passed.find(idtf) != passed.cend();

      if (!isUnnamed && isPassed)
      {
        outValue.SetReplacement(idtf.c_str());
      }
      else
      {
        sc_char const * alias = (isUnnamed ? nullptr : idtf.c_str());
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
            SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't find element " << idtf);
            return false;
          }
        }
      }

      passed.insert(idtf);

      return true;
    };

    for (scs::ParsedTriple const & t : m_parser.GetParsedTriples())
    {
      scs::ParsedElement const & src = m_parser.GetParsedElement(t.m_source);
      scs::ParsedElement const & connector = m_parser.GetParsedElement(t.m_connector);
      scs::ParsedElement const & trg = m_parser.GetParsedElement(t.m_target);

      ScTemplateItem srcItem, connectorItem, trgItem;

      if (!MakeTemplItem(src, srcItem) || !MakeTemplItem(connector, connectorItem) || !MakeTemplItem(trg, trgItem))
        break;

      templ->Triple(srcItem, connectorItem, trgItem);
    }
  }

private:
  std::string const & m_translatableSCsTemplate;
  ScMemoryContext & m_ctx;

  scs::Parser m_parser;
};

void ScTemplate::TranslateFrom(ScMemoryContext & ctx, std::string const & translatableSCsTemplate)
{
  ScTemplateBuilderFromScs builder(translatableSCsTemplate, ctx);
  builder(this);
}
