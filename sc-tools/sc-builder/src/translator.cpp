/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "translator.hpp"
#include "keynodes.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_link.hpp"

Translator::Translator(ScMemoryContext & ctx)
  : m_ctx(ctx)
{
}

bool Translator::Translate(Params const & params)
{
  return TranslateImpl(params);
}

void Translator::GenerateFormatInfo(ScAddr const & addr, std::string const & ext)
{
  std::string const fmtStr = "format_" + ext;

  ScAddr const formatAddr = m_ctx.HelperResolveSystemIdtf(fmtStr, ScType::NodeConstClass);
  
  ScTemplate templ;
  templ.TripleWithRelation(
    addr,
    ScType::EdgeDCommonVar,
    formatAddr,
    ScType::EdgeAccessVarPosPerm,
    Keynodes::kNrelFormat()
  );

  ScTemplateGenResult genResult;
  auto const res = m_ctx.HelperGenTemplate(templ, genResult);
  if (!res)
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Error to generate format for sc-link: " << res.Msg());
}

void Translator::GetFileContent(std::string const & fileName, std::string & outContent)
{
  std::ifstream ifs(fileName);
  if (!ifs.is_open())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't open file " << fileName);
  }
    
  outContent.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  ifs.close();
}

void Translator::Clean(ScMemoryContext & ctx)
{
  // remove global identifiers
  ScAddr const nrelSCsGlobalIdtf = ctx.HelperResolveSystemIdtf("nrel_scs_global_idtf", ScType::NodeConstNoRole);
  if (!nrelSCsGlobalIdtf.IsValid())
  {
    ScConsole::PrintLine() << ScConsole::Color::Red << "Can't resolve keynode 'nrel_scs_global_idtf'";
    return;
  }

  ScTemplate templ;
  templ.TripleWithRelation(
    ScType::Unknown,
    ScType::EdgeDCommonVar,
    ScType::Link >> "_link",
    ScType::EdgeAccessVarPosPerm,
    nrelSCsGlobalIdtf);

  ScTemplateSearchResult res;
  if (ctx.HelperSearchTemplate(templ, res))
  {
    res.ForEach([&ctx](ScTemplateSearchResultItem const & item)
    {
      ctx.EraseElement(item["_link"]);
    });
  }
}



