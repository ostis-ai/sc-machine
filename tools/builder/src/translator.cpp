/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "translator.hpp"

#include "sc-memory/sc_memory.hpp"

ScAddr Translator::ms_kNrelFormat;

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
    ms_kNrelFormat
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
