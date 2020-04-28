#include "../test_unit.hpp"

#include "sc-memory/sc_link.hpp"

UNIT_TEST_CUSTOM(Builder_Contents_Link, BuilderTestUnit)
{
  ScMemoryContext ctx("Builder_Contents_Link");

  SUBTEST_START(file_relative)
  {
    /*
      "file://file.txt" => nrel_system_identifier:
        [test_content_file];;
     */

    ScAddr const linkAddr = ctx.HelperResolveSystemIdtf("test_content_file");
    SC_CHECK(linkAddr.IsValid(), ());

    ScLink link(ctx, linkAddr);
    std::string const content = link.GetAsString();
    SC_CHECK_EQUAL(content, "file", ());
  }
  SUBTEST_END()

  SUBTEST_START(file_relative_folder)
  {
    ScAddr const linkAddr = ctx.HelperResolveSystemIdtf("test_content_file_2");
    SC_CHECK(linkAddr.IsValid(), ());

    ScLink link(ctx, linkAddr);
    std::string const content = link.GetAsString();
    SC_CHECK_EQUAL(content, "contents/file", ());
  }
  SUBTEST_END()
}

template <typename ValueT>
void CheckBinaryContent(ScMemoryContext & ctx, std::string const & sysIdtf, ValueT value)
{
  ScAddr const linkAddr = ctx.HelperResolveSystemIdtf(sysIdtf);
  SC_CHECK(linkAddr.IsValid(), ());

  ScLink link(ctx, linkAddr);
  SC_CHECK(link.IsType<ValueT>(), ());

  SC_CHECK_EQUAL(link.Get<ValueT>(), value, ());
}

UNIT_TEST_CUSTOM(Builder_Contents_Binary, BuilderTestUnit)
{
  ScMemoryContext ctx("Builder_Contents_Binary");

  SUBTEST_START(string)
  {
    CheckBinaryContent<std::string>(ctx, "test_content_string", "string");
  }
  SUBTEST_END()

  SUBTEST_START(float)
  {
    CheckBinaryContent<float>(ctx, "test_content_float", 43.567f);
  }
  SUBTEST_END()

  SUBTEST_START(double)
  {
    CheckBinaryContent<double>(ctx, "test_content_double", 543.345);
  }
  SUBTEST_END()

  SUBTEST_START(int8)
  {
    CheckBinaryContent<int8_t>(ctx, "test_content_int8", 8);
  }
  SUBTEST_END()

  SUBTEST_START(int16)
  {
    CheckBinaryContent<int16_t>(ctx, "test_content_int16", 16);
  }
  SUBTEST_END()

  SUBTEST_START(int32)
  {
    CheckBinaryContent<int32_t>(ctx, "test_content_int32", 32);
  }
  SUBTEST_END()

  SUBTEST_START(int64)
  {
    CheckBinaryContent<int64_t>(ctx, "test_content_int64", 64);
  }
  SUBTEST_END()

  SUBTEST_START(uint8)
  {
    CheckBinaryContent<uint8_t>(ctx, "test_content_uint8", 108);
  }
  SUBTEST_END()

  SUBTEST_START(uint16)
  {
    CheckBinaryContent<uint16_t>(ctx, "test_content_uint16", 116);
  }
  SUBTEST_END()

  SUBTEST_START(uint32)
  {
    CheckBinaryContent<uint32_t>(ctx, "test_content_uint32", 132);
  }
  SUBTEST_END()

  SUBTEST_START(uint64)
  {
    CheckBinaryContent<uint64_t>(ctx, "test_content_uint64", 164);
  }
  SUBTEST_END()
}