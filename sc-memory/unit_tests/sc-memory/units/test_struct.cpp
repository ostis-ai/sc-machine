/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/sc_struct.hpp"
#include "sc-memory/utils/sc_test.hpp"

UNIT_TEST(struct_common)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "struct_common");

  ScAddr structAddr = ctx.CreateNode(ScType::NodeConstStruct);
  SC_CHECK(structAddr.IsValid(), ());

  ScStruct st(&ctx, structAddr);

  ScAddr const addr1 = ctx.CreateNode(ScType::NodeConstClass);
  SC_CHECK(addr1.IsValid(), ());

  ScAddr const addr2 = ctx.CreateNode(ScType::NodeConstMaterial);
  SC_CHECK(addr2.IsValid(), ());

  st << addr1 << addr2;
  SC_CHECK(st.HasElement(addr1), ());
  SC_CHECK(st.HasElement(addr2), ());

  st >> addr1;

  SC_CHECK(!st.HasElement(addr1), ());
  SC_CHECK(st.HasElement(addr2), ());

  st >> addr2;

  SC_CHECK(!st.HasElement(addr1), ());
  SC_CHECK(!st.HasElement(addr2), ());
  SC_CHECK(st.IsEmpty(), ());

  // attributes
  ScAddr const attrAddr = ctx.CreateNode(ScType::NodeConstRole);
  SC_CHECK(attrAddr.IsValid(), ());

  SC_CHECK(st.Append(addr1, attrAddr), ());
  ScIterator5Ptr iter5 = ctx.Iterator5(
        structAddr,
        SC_TYPE(sc_type_arc_pos_const_perm),
        SC_TYPE(0),
        SC_TYPE(sc_type_arc_pos_const_perm),
        attrAddr);

  bool found = false;
  while (iter5->Next())
  {
    SC_CHECK(!found, ());	// one time
    SC_CHECK_EQUAL(iter5->Get(0), structAddr, ());
    SC_CHECK_EQUAL(iter5->Get(2), addr1, ());
    SC_CHECK_EQUAL(iter5->Get(4), attrAddr, ());
    found = true;
  }
  SC_CHECK(found, ());
}
