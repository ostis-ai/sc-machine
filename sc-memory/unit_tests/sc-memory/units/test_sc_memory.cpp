/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/utils/sc_test.hpp"

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"

UNIT_TEST(ScMemoryContext_FindLinksByContent)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ScMemoryContext_FindLinksByContent");

  {
    ScAddr const linkAddr1 = ctx.CreateLink();
    SC_CHECK(linkAddr1.IsValid(), ());

    std::string const linkContent1 = "ScMemoryContext_FindLinksByContent_content_1";
    ScLink link1(ctx, linkAddr1);
    SC_CHECK(link1.Set(linkContent1), ());

    ScAddrVector const result = ctx.FindLinksByContent(linkContent1);
    SC_CHECK_EQUAL(result.size(), 1, ());
  }
}
