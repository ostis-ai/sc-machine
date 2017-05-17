/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/sc_link.hpp"

template <typename Type> void TestType(ScMemoryContext & ctx, Type const & value)
{
  ScAddr const linkAddr = ctx.CreateLink();
  SC_CHECK(linkAddr.IsValid(), ());

  ScLink link(ctx, linkAddr);
  SC_CHECK_NOT(link.IsType<Type>(), ());
  
  SC_CHECK(link.Set(value), ());
  SC_CHECK_EQUAL(link.Get<Type>(), value, ());
  SC_CHECK(link.IsType<Type>(), ());
}

UNIT_TEST(sc_link_common)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_common");

  TestType<std::string>(ctx, "test_string_value");
  TestType<double>(ctx, 5.0);
  TestType<float>(ctx, 10.f);
  TestType<int8_t>(ctx, -57);
  TestType<int16_t>(ctx, -6757);
  TestType<int32_t>(ctx, -672357);
  TestType<int64_t>(ctx, -672423457);
  TestType<uint8_t>(ctx, 57);
  TestType<uint16_t>(ctx, 6757);
  TestType<uint32_t>(ctx, 672357);
  TestType<uint64_t>(ctx, 672423457);
}

UNIT_TEST(sc_link_reuse)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_reuse");

  std::string const str = "test_string_value_2";
  ScAddr const linkAddr = ctx.CreateLink();
  SC_CHECK(linkAddr.IsValid(), ());

  ScLink link(ctx, linkAddr);
  SC_CHECK_NOT(link.IsType<std::string>(), ());

  SC_CHECK(link.Set(str), ());
  SC_CHECK(link.IsType<std::string>(), ());
  SC_CHECK_EQUAL(link.Get<std::string>(), str, ());
  
  uint64_t value = 32049809;
  SC_CHECK(link.Set(value), ());
  SC_CHECK(link.IsType<uint64_t>(), ());
  SC_CHECK_NOT(link.IsType<std::string>(), ());
  SC_CHECK_EQUAL(link.Get<uint64_t>(), value, ());
}
