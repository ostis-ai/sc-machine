#pragma once

#include "sc_link.hpp"
#include "sc_memory.hpp"
#include "sc_template.hpp"

namespace sc
{
/* Create construction:
 * elAddr <= relAddr: {};;
 * If construction exist, then returns exist tuple addr; otherwise create new one.
 * Returns ScAddr of tuple
 */
_SC_EXTERN ScAddr ResolveRelationTuple(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const & relAddr);

/* Generates construction:
 * elAddr => relAddr: [];;
 * If link with relation exist, then changes it value; otherwise create new one.
 * Returns ScAddr of sc-link
 */
template <typename ValueType>
ScAddr SetRelationValue(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const & relAddr, ValueType const & value)
{
  ScTemplate templ;

  templ.TripleWithRelation(
      elAddr, ScType::EdgeDCommonVar, ScType::Link >> "_link", ScType::EdgeAccessVarPosPerm, relAddr);

  ScAddr linkAddr;
  ScTemplateSearchResult res;
  if (ctx.HelperSearchTemplate(templ, res))
    linkAddr = res[0]["_link"];

  if (!linkAddr.IsValid())
  {
    ScTemplateGenResult genRes;
    if (ctx.HelperGenTemplate(templ, genRes))
      linkAddr = genRes["_link"];

    if (!linkAddr.IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't create value relation");
    }
  }

  ScLink link(ctx, linkAddr);
  link.Set(value);

  return linkAddr;
}

}  // namespace sc