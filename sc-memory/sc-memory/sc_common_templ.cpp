#include "sc_common_templ.hpp"

namespace sc
{
ScAddr ResolveRelationTuple(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const & relAddr)
{
  ScTemplate templ;

  templ.TripleWithRelation(
      ScType::NodeVarTuple >> "_tuple", ScType::EdgeDCommonVar, elAddr, ScType::EdgeAccessVarPosPerm, relAddr);

  ScTemplateSearchResult searchRes;
  if (ctx.HelperSearchTemplate(templ, searchRes))
  {
    return searchRes[0]["_tuple"];
  }

  ScTemplateGenResult genRes;
  if (!ctx.HelperGenTemplate(templ, genRes))
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't create tuple");
  }

  return genRes["_tuple"];
}

}  // namespace sc