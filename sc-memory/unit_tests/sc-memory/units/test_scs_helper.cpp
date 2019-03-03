/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "dummy_file_interface.hpp"

#include "sc-memory/utils/sc_test.hpp"

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"



UNIT_TEST(SCsHelper_GenerateBySCs_Smoke)
{
  std::vector<std::pair<std::string, std::string>> tests =
  {
    { "x -> y;;", "x _-> _y;;" },
    { "x1 => nrel_x1: [test_content*];;", "x1 _=> nrel_x1:: _[];;" },
    { "x2 ~> y2 (* <- z2;; *);;", "x2 _~> _y2 (* _<- _z2;; *);;" },
    { "x3 <- y3 (* <- sc_node_class;; *);;", "sc_node_class -> _y3;; _y3 _-> x3;;" },
  };

  for (auto const & t : tests)
  {
    ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_Smoke");

    SCsHelper helper(&ctx, std::make_shared<DummyFileInterface>());

    SC_CHECK(helper.GenerateBySCsText(t.first), ());

    ScTemplate templ;
    SC_CHECK(ctx.HelperBuildTemplate(templ, t.second), ());

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
    SC_CHECK_EQUAL(result.Size(), 1, ());
  }
}

namespace
{

class TestFileInterface : public SCsFileInterface
{
public:
  ~TestFileInterface() override {}

  ScStreamPtr GetFileContent(std::string const & fileURL) override
  {
    std::string const content = "content: " + fileURL;
    return ScStreamConverter::StreamFromString(content);
  }
};

} // namespace

UNIT_TEST(SCsHelper_GenerateBySCs_FileURL)
{
  std::string const data = "x -> \"file://test.scs\";;";

  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_FileURL");

  SCsHelper helper(&ctx, std::make_shared<TestFileInterface>());
  SC_CHECK(helper.GenerateBySCsText(data), ());

  ScTemplate templ;
  SC_CHECK(ctx.HelperBuildTemplate(templ, "x _-> _[];;"), ());

  ScTemplateSearchResult result;
  SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());

  SC_CHECK_EQUAL(result.Size(), 1, ());

  ScAddr const linkAddr = result[0][2];
  SC_CHECK(linkAddr.IsValid(), ());

  ScType const linkType = ctx.GetElementType(linkAddr);
  SC_CHECK_EQUAL(linkType, ScType::LinkConst, ());

  ScLink const link(ctx, linkAddr);
  SC_CHECK_EQUAL(link.DetermineType(), ScLink::Type::Custom, ());
  SC_CHECK_EQUAL(link.GetAsString(), "content: file://test.scs", ());
}

UNIT_TEST(SCsHelper_GenerateBySCs_Aliases)
{
  std::string const content = "SCsHelper_GenerateBySCs_Aliases";
  std::string const data = "@alias = [" + content + "];;";

  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_Aliases");

  SCsHelper helper(&ctx, std::make_shared<TestFileInterface>());
  SC_CHECK(helper.GenerateBySCsText(data), ());

  ScStreamPtr const stream = ScStreamMakeRead(content);

  ScAddrVector const links = ctx.FindLinksByContent(stream);
  SC_CHECK_EQUAL(links.size(), 1, ());

  ScLink const link(ctx, links[0]);
  std::string const content2 = link.GetAsString();
  SC_CHECK_EQUAL(content, content2, ());
}

UNIT_TEST(SCsHelper_GenerateBySCs_NumberContents)
{
  std::string const dataFloat  = "v_float -> [^\"float:7\"];;";
  std::string const dataDouble = "v_double -> [^\"double:8\"];;";
  std::string const dataInt8   = "v_int8 -> [^\"int8:9\"];;";
  std::string const dataInt16  = "v_int16 -> [^\"int16:10\"];;";
  std::string const dataInt32  = "v_int32 -> [^\"int32:11\"];;";
  std::string const dataInt64  = "v_int64 -> [^\"int64:12\"];;";
  std::string const dataUint8  = "v_uint8 -> [^\"uint8:13\"];;";
  std::string const dataUint16 = "v_uint16 -> [^\"uint16:14\"];;";
  std::string const dataUint32 = "v_uint32 -> [^\"uint32:15\"];;";
  std::string const dataUint64 = "v_uint64 -> [^\"uint64:16\"];;";

  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_NumberContents");

  auto const testLink = [&ctx](std::string const & data, std::string const & keynode)
  {
    SCsHelper helper(&ctx, std::make_shared<TestFileInterface>());
    SC_CHECK(helper.GenerateBySCsText(data), ());

    ScTemplate templ;
    SC_CHECK(ctx.HelperBuildTemplate(templ, keynode + " _-> _[];;"), ());

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());

    SC_CHECK_EQUAL(result.Size(), 1, ());

    ScAddr const linkAddr = result[0][2];
    SC_CHECK(linkAddr.IsValid(), ());

    return linkAddr;
  };

  {
    ScLink const linkFloat(ctx, testLink(dataFloat, "v_float"));
    SC_CHECK_EQUAL(linkFloat.DetermineType(), ScLink::Type::Float, ());
    SC_CHECK_EQUAL(linkFloat.Get<float>(), 7.f, ());
  }

  {
    ScLink const linkDouble(ctx, testLink(dataDouble, "v_double"));
    SC_CHECK_EQUAL(linkDouble.DetermineType(), ScLink::Type::Double, ());
    SC_CHECK_EQUAL(linkDouble.Get<double>(), 8.0, ());
  }

  {
    ScLink const linkInt8(ctx, testLink(dataInt8, "v_int8"));
    SC_CHECK_EQUAL(linkInt8.DetermineType(), ScLink::Type::Int8, ());
    SC_CHECK_EQUAL(linkInt8.Get<int8_t>(), 9, ());
  }

  {
    ScLink const linkInt16(ctx, testLink(dataInt16, "v_int16"));
    SC_CHECK_EQUAL(linkInt16.DetermineType(), ScLink::Type::Int16, ());
    SC_CHECK_EQUAL(linkInt16.Get<int16_t>(), 10, ());
  }

  {
    ScLink const linkInt32(ctx, testLink(dataInt32, "v_int32"));
    SC_CHECK_EQUAL(linkInt32.DetermineType(), ScLink::Type::Int32, ());
    SC_CHECK_EQUAL(linkInt32.Get<int32_t>(), 11, ());
  }

  {
    ScLink const linkInt64(ctx, testLink(dataInt64, "v_int64"));
    SC_CHECK_EQUAL(linkInt64.DetermineType(), ScLink::Type::Int64, ());
    SC_CHECK_EQUAL(linkInt64.Get<int64_t>(), 12, ());
  }

  {
    ScLink const linkUint8(ctx, testLink(dataUint8, "v_uint8"));
    SC_CHECK_EQUAL(linkUint8.DetermineType(), ScLink::Type::UInt8, ());
    SC_CHECK_EQUAL(linkUint8.Get<uint8_t>(), 13, ());
  }

  {
    ScLink const linkUint16(ctx, testLink(dataUint16, "v_uint16"));
    SC_CHECK_EQUAL(linkUint16.DetermineType(), ScLink::Type::UInt16, ());
    SC_CHECK_EQUAL(linkUint16.Get<uint16_t>(), 14, ());
  }

  {
    ScLink const linkUint32(ctx, testLink(dataUint32, "v_uint32"));
    SC_CHECK_EQUAL(linkUint32.DetermineType(), ScLink::Type::UInt32, ());
    SC_CHECK_EQUAL(linkUint32.Get<uint32_t>(), 15, ());
  }

  {
    ScLink const linkUint64(ctx, testLink(dataUint64, "v_uint64"));
    SC_CHECK_EQUAL(linkUint64.DetermineType(), ScLink::Type::UInt64, ());
    SC_CHECK_EQUAL(linkUint64.Get<uint64_t>(), 16, ());
  }
}

UNIT_TEST(SCsHelper_GenerateBySCs_NotUsedElement)
{
  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_NotUsedElement");

  SCsHelper helper(&ctx, std::make_shared<DummyFileInterface>());
  std::string const scsData = "node <- sc_node_class";

  SC_CHECK(helper.GenerateBySCsText(scsData), ());

  ScAddr const node = ctx.HelperFindBySystemIdtf("node");
  SC_CHECK(node.IsValid(), ());
  SC_CHECK_EQUAL(ctx.GetElementType(node), ScType::NodeConstClass, ());
}
