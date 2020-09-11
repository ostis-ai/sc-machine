/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/sc_link.hpp"

template<typename Type>
void TestType(ScMemoryContext & ctx, Type const & value)
{
  ScAddr const linkAddr = ctx.CreateLink();
  REQUIRE(linkAddr.IsValid());

  ScLink link(ctx, linkAddr);
  REQUIRE_FALSE(link.IsType<Type>());

  REQUIRE(link.Set(value));
  REQUIRE(link.Get<Type>() == value);
  REQUIRE(link.IsType<Type>());
}

TEST_CASE("sc_link_common", "[test sc link]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_common");
  try
  {
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
  } catch (...)
  {
    SC_LOG_ERROR("Test \"sc_link_common\" failed")
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("sc_link_reuse", "[test sc link]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_reuse");
  try
  {
    std::string const str = "test_string_value_2";
    ScAddr const linkAddr = ctx.CreateLink();
    REQUIRE(linkAddr.IsValid());

    ScLink link(ctx, linkAddr);
    REQUIRE_FALSE(link.IsType<std::string>());

    REQUIRE(link.Set(str));
    REQUIRE(link.IsType<std::string>());
    REQUIRE_THAT(link.Get<std::string>(), Catch::Equals(str));

    uint64_t value = 32049809;
    REQUIRE(link.Set(value));
    REQUIRE(link.IsType<uint64_t>());
    REQUIRE_FALSE(link.IsType<std::string>());
    REQUIRE(link.Get<uint64_t>() == value);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"sc_link_reuse\" failed")
  }
  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("sc_link_as_string", "[test sc link]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_link_as_string");
  try
  {
    ScAddr const linkAddr = ctx.CreateLink();
    REQUIRE(linkAddr.IsValid());

    ScLink link(ctx, linkAddr);

    REQUIRE(link.Set<std::string>("76"));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("76"));

    REQUIRE(link.Set<int8_t>(-76));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("-76"));

    REQUIRE(link.Set<uint8_t>(76));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("76"));

    REQUIRE(link.Set<int16_t>(-760));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("-760"));

    REQUIRE(link.Set<uint16_t>(760));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("760"));

    REQUIRE(link.Set<int32_t>(-76000));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("-76000"));

    REQUIRE(link.Set<uint32_t>(76000));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("76000"));

    REQUIRE(link.Set<int64_t>(-7600000000));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("-7600000000"));

    REQUIRE(link.Set<uint64_t>(7600000000));
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("7600000000"));
  } catch (...)
  {
    SC_LOG_ERROR("Test \"sc_link_as_string\" failed")
  }
  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}
