/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"

#include "sc-memory/cpp/sc_struct.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

TEST_CASE("struct_common", "[test struct]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "struct_common");

  ScAddr structAddr = ctx.CreateNode(ScType::NodeConstStruct);
  REQUIRE(structAddr.IsValid());

  ScStruct st(&ctx, structAddr);

  ScAddr const addr1 = ctx.CreateNode(ScType::NodeConstClass);
  REQUIRE(addr1.IsValid());

  ScAddr const addr2 = ctx.CreateNode(ScType::NodeConstMaterial);
  REQUIRE(addr2.IsValid());

  st << addr1 << addr2;
  REQUIRE(st.HasElement(addr1));
  REQUIRE(st.HasElement(addr2));

  st >> addr1;

  REQUIRE(!st.HasElement(addr1));
  REQUIRE(st.HasElement(addr2));

  st >> addr2;

  REQUIRE(!st.HasElement(addr1));
  REQUIRE(!st.HasElement(addr2));
  REQUIRE(st.IsEmpty());

  // attributes
  ScAddr const attrAddr = ctx.CreateNode(ScType::NodeConstRole);
  REQUIRE(attrAddr.IsValid());

  REQUIRE(st.Append(addr1, attrAddr));
  ScIterator5Ptr iter5 = ctx.Iterator5(
        structAddr,
        SC_TYPE(sc_type_arc_pos_const_perm),
        SC_TYPE(0),
        SC_TYPE(sc_type_arc_pos_const_perm),
        attrAddr);

  bool found = false;
  while (iter5->Next())
  {
    REQUIRE_FALSE(found);  // one time
    REQUIRE(iter5->Get(0) == structAddr);
    REQUIRE(iter5->Get(2) == addr1);
    REQUIRE(iter5->Get(4) == attrAddr);
    found = true;
  }
  SC_CHECK(found, ());
}
