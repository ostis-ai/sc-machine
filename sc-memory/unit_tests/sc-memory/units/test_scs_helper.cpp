/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "dummy_file_interface.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"

TEST_CASE("SCsHelper_GenerateBySCs_Smoke", "[test scs helper]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  std::vector<std::pair<std::string, std::string>> tests =
        {
              { "x -> y;;",                            "x _-> _y;;" },
              { "x1 => nrel_x1: [test_content*];;",    "x1 _=> nrel_x1:: _[];;" },
              { "x2 ~> y2 (* <- z2;; *);;",            "x2 _~> _y2 (* _<- _z2;; *);;" },
              { "x3 <- y3 (* <- sc_node_class;; *);;", "sc_node_class -> _y3;; _y3 _-> x3;;" },
        };

  try
  {
    for (auto const & t : tests)
    {
      ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_Smoke");

      SCsHelper helper(ctx, std::make_shared<DummyFileInterface>());

      REQUIRE(helper.GenerateBySCsText(t.first));

      ScTemplate templ;
      REQUIRE(ctx.HelperBuildTemplate(templ, t.second));

      ScTemplateSearchResult result;
      REQUIRE(ctx.HelperSearchTemplate(templ, result));
      REQUIRE(result.Size() == 1);

      ctx.Destroy();
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"SCsHelper_GenerateBySCs_Smoke\" failed")
  }

  test::ScTestUnit::ShutdownMemory(false);
}

namespace
{

class TestFileInterface : public SCsFileInterface
{
public:
  ~TestFileInterface() override
  {}

  ScStreamPtr GetFileContent(std::string const & fileURL) override
  {
    std::string const content = "content: " + fileURL;
    return ScStreamConverter::StreamFromString(content);
  }
};

} // namespace

TEST_CASE("SCsHelper_GenerateBySCs_FileURL", "[test scs helper]")
{
  std::string const data = "x -> \"file://test.scs\";;";

  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_FileURL");

  try
  {
    SCsHelper helper(ctx, std::make_shared<TestFileInterface>());
    REQUIRE(helper.GenerateBySCsText(data));

    ScTemplate templ;
    REQUIRE(ctx.HelperBuildTemplate(templ, "x _-> _[];;"));

    ScTemplateSearchResult result;
    REQUIRE(ctx.HelperSearchTemplate(templ, result));

    REQUIRE(result.Size() == 1);

    ScAddr const linkAddr = result[0][2];
    REQUIRE(linkAddr.IsValid());

    ScType const linkType = ctx.GetElementType(linkAddr);
    REQUIRE(linkType == ScType::LinkConst);

    ScLink const link(ctx, linkAddr);
    REQUIRE(link.DetermineType() == ScLink::Type::Custom);
    REQUIRE_THAT(link.GetAsString(), Catch::Equals("content: file://test.scs"));
  } catch (...)
  {
    SC_LOG_ERROR("Test \"SCsHelper_GenerateBySCs_FileURL\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("SCsHelper_ChangeElementType", "[test scs helper]")
{
    std::string const data = "sc_node_struct -> a;;";

    test::ScTestUnit::InitMemory("sc-memory.ini", "");
    ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_ChangeElementType");

    try
    {
        ScAddr node = ctx.CreateNode(ScType::Node);
        ctx.HelperSetSystemIdtf("a", node);

        SCsHelper helper(ctx, std::make_shared<TestFileInterface>());
        REQUIRE(helper.GenerateBySCsText(data));

        ScAddr element = ctx.HelperFindBySystemIdtf("a");

        ScType const linkType = ctx.GetElementType(element);
        REQUIRE(linkType == ScType::NodeConstStruct);
    } catch (...)
    {
        SC_LOG_ERROR("Test \"SCsHelper_ChangeElementType\" failed")
    }

    ctx.Destroy();
    test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("SCsHelper_GenerateBySCs_Aliases", "[test scs helper]")
{
  std::string const content = "SCsHelper_GenerateBySCs_Aliases";
  std::string const data = "@alias = [" + content + "];;";

  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_Aliases");

  try
  {
    SCsHelper helper(ctx, std::make_shared<TestFileInterface>());
    REQUIRE(helper.GenerateBySCsText(data));

    ScStreamPtr const stream = ScStreamMakeRead(content);

    ScAddrVector const links = ctx.FindLinksByContent(stream);
    REQUIRE(links.size() == 1);

    ScLink const link(ctx, links[0]);
    std::string const content2 = link.GetAsString();
    REQUIRE_THAT(content, Catch::Equals(content2));
  } catch (...)
  {
    SC_LOG_ERROR("Test \"SCsHelper_GenerateBySCs_Aliases\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("SCsHelper_ProcessEdgeAlias", "[test scs helper]")
{
    std::string const data = "@edge_alias = (c -> b);;"
                             "a -> @edge_alias;;";

    test::ScTestUnit::InitMemory("sc-memory.ini", "");
    ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_ProcessEdgeAlias");

    try
    {
        SCsHelper helper(ctx, std::make_shared<TestFileInterface>());
        REQUIRE(helper.GenerateBySCsText(data));

        ScAddr const aAddr = ctx.HelperResolveSystemIdtf("a");
        REQUIRE(aAddr.IsValid());

        ScAddr const bAddr = ctx.HelperResolveSystemIdtf("b");
        REQUIRE(bAddr.IsValid());

        ScAddr const cAddr = ctx.HelperResolveSystemIdtf("c");
        REQUIRE(cAddr.IsValid());

        ScTemplate templ;
        templ.TripleWithRelation(
                cAddr,
                ScType::EdgeAccessVarPosPerm,
                bAddr,
                ScType::EdgeAccessVarPosPerm,
                aAddr);

        ScTemplateSearchResult result;
        REQUIRE(ctx.HelperSearchTemplate(templ, result));
        REQUIRE(result.Size() == 1);

    } catch (...)
    {
        SC_LOG_ERROR("Test \"SCsHelper_ProcessEdgeAlias\" failed")
    }

    ctx.Destroy();
    test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("SCsHelper_GenerateBySCs_Contents", "[test scs helper]")
{
  std::string const dataString = "v_string -> [string];;";
  std::string const dataFloat = "v_float -> [^\"float:7\"];;";
  std::string const dataDouble = "v_double -> [^\"double:8\"];;";
  std::string const dataInt8 = "v_int8 -> [^\"int8:9\"];;";
  std::string const dataInt16 = "v_int16 -> [^\"int16:10\"];;";
  std::string const dataInt32 = "v_int32 -> [^\"int32:11\"];;";
  std::string const dataInt64 = "v_int64 -> [^\"int64:12\"];;";
  std::string const dataUint8 = "v_uint8 -> [^\"uint8:13\"];;";
  std::string const dataUint16 = "v_uint16 -> [^\"uint16:14\"];;";
  std::string const dataUint32 = "v_uint32 -> [^\"uint32:15\"];;";
  std::string const dataUint64 = "v_uint64 -> [^\"uint64:16\"];;";

  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_NumberContents");

  try
  {
    auto const testLink = [&ctx](std::string const & data, std::string const & keynode)
    {
      SCsHelper helper(ctx, std::make_shared<TestFileInterface>());
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
      ScLink const linkFloat(ctx, testLink(dataString, "v_string"));
      SC_CHECK_EQUAL(linkFloat.DetermineType(), ScLink::Type::String, ());
      SC_CHECK_EQUAL(linkFloat.Get<std::string>(), "string", ());
    }

    {
      ScLink const linkFloat(ctx, testLink(dataFloat, "v_float"));
      REQUIRE(linkFloat.DetermineType() == ScLink::Type::Float);
      REQUIRE(linkFloat.Get<float>() == 7.f);
    }

    {
      ScLink const linkDouble(ctx, testLink(dataDouble, "v_double"));
      REQUIRE(linkDouble.DetermineType() == ScLink::Type::Double);
      REQUIRE(linkDouble.Get<double>() == 8.0);
    }

    {
      ScLink const linkInt8(ctx, testLink(dataInt8, "v_int8"));
      REQUIRE(linkInt8.DetermineType() == ScLink::Type::Int8);
      REQUIRE(linkInt8.Get<int8_t>() == 9);
    }

    {
      ScLink const linkInt16(ctx, testLink(dataInt16, "v_int16"));
      REQUIRE(linkInt16.DetermineType() == ScLink::Type::Int16);
      REQUIRE(linkInt16.Get<int16_t>() == 10);
    }

    {
      ScLink const linkInt32(ctx, testLink(dataInt32, "v_int32"));
      REQUIRE(linkInt32.DetermineType() == ScLink::Type::Int32);
      REQUIRE(linkInt32.Get<int32_t>() == 11);
    }

    {
      ScLink const linkInt64(ctx, testLink(dataInt64, "v_int64"));
      REQUIRE(linkInt64.DetermineType() == ScLink::Type::Int64);
      REQUIRE(linkInt64.Get<int64_t>() == 12);
    }

    {
      ScLink const linkUint8(ctx, testLink(dataUint8, "v_uint8"));
      REQUIRE(linkUint8.DetermineType() == ScLink::Type::UInt8);
      REQUIRE(linkUint8.Get<uint8_t>() == 13);
    }

    {
      ScLink const linkUint16(ctx, testLink(dataUint16, "v_uint16"));
      REQUIRE(linkUint16.DetermineType() == ScLink::Type::UInt16);
      REQUIRE(linkUint16.Get<uint16_t>() == 14);
    }

    {
      ScLink const linkUint32(ctx, testLink(dataUint32, "v_uint32"));
      REQUIRE(linkUint32.DetermineType() == ScLink::Type::UInt32);
      REQUIRE(linkUint32.Get<uint32_t>() == 15);
    }

    {
      ScLink const linkUint64(ctx, testLink(dataUint64, "v_uint64"));
      REQUIRE(linkUint64.DetermineType() == ScLink::Type::UInt64);
      REQUIRE(linkUint64.Get<uint64_t>() == 16);
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"SCsHelper_GenerateBySCs_Contents\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("SCsHelper_GenerateBySCs_NotUsedElement", "[test scs helper]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_NotUsedElement");

  try
  {
    SCsHelper helper(ctx, std::make_shared<DummyFileInterface>());
    std::string const scsData = "node <- sc_node_class;;";

    REQUIRE(helper.GenerateBySCsText(scsData));

    ScAddr const node = ctx.HelperFindBySystemIdtf("node");
    REQUIRE(node.IsValid());
    REQUIRE(ctx.GetElementType(node) == ScType::NodeConstClass);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"SCsHelper_GenerateBySCs_NotUsedElement\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("SCsHelper_GenerateBySCs_Visibility", "[test scs helper]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_max, "SCsHelper_GenerateBySCs_Visibility");

  SECTION("system_idtf")
  {
    SUBTEST_START("system_idtf")
    {
      try
      {
        SCsHelper helper(ctx, std::make_shared<DummyFileInterface>());

        REQUIRE(helper.GenerateBySCsText("x -> y;;"));
        REQUIRE(helper.GenerateBySCsText("x ~> z;;"));

        ScAddr const xAddr = ctx.HelperResolveSystemIdtf("x");
        REQUIRE(xAddr.IsValid());

        ScAddr const yAddr = ctx.HelperResolveSystemIdtf("y");
        REQUIRE(yAddr.IsValid());

        ScAddr const zAddr = ctx.HelperResolveSystemIdtf("z");
        REQUIRE(zAddr.IsValid());

        {
          ScTemplate templ;
          templ.Triple(xAddr, ScType::EdgeAccessVarPosPerm, yAddr);

          ScTemplateSearchResult res;
          REQUIRE(ctx.HelperSearchTemplate(templ, res));
        }

        {
          ScTemplate templ;
          templ.Triple(xAddr, ScType::EdgeAccessVarPosTemp, zAddr);

          ScTemplateSearchResult res;
          REQUIRE(ctx.HelperSearchTemplate(templ, res));
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"system_idtf\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("global")
  {
    SUBTEST_START("global")
    {
      try
      {
        SCsHelper helper(ctx, std::make_shared<DummyFileInterface>());

        {
          ScAddr const xAddr = ctx.HelperResolveSystemIdtf("x_global");
          REQUIRE_FALSE(xAddr.IsValid());
        }

        REQUIRE(helper.GenerateBySCsText("x_global -> .y;;"));
        REQUIRE(helper.GenerateBySCsText("x_global -> .y;;"));

        ScAddr const xAddr = ctx.HelperResolveSystemIdtf("x_global");
        REQUIRE(xAddr.IsValid());

        ScTemplate templ;
        templ.Triple(xAddr, ScType::EdgeAccessVarPosPerm >> "_edge", ScType::Unknown >> "_trg");

        ScTemplateSearchResult res;
        REQUIRE(ctx.HelperSearchTemplate(templ, res));
        REQUIRE(res.Size() == 2);
        REQUIRE(res[0]["_trg"] == res[1]["_trg"]);
        REQUIRE_FALSE(res[0]["_edge"] == res[1]["_edge"]);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"global\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("local")
  {
    SUBTEST_START("local")
    {
      try
      {
        SCsHelper helper(ctx, std::make_shared<DummyFileInterface>());

        {
          ScAddr const xAddr = ctx.HelperResolveSystemIdtf("x_local");
          REQUIRE_FALSE(xAddr.IsValid());
        }

        REQUIRE(helper.GenerateBySCsText("x_local -> ..y;;"));
        REQUIRE(helper.GenerateBySCsText("x_local -> ..z;;"));

        ScAddr const xAddr = ctx.HelperResolveSystemIdtf("x_local");
        REQUIRE(xAddr.IsValid());

        ScTemplate templ;
        templ.Triple(xAddr, ScType::EdgeAccessVarPosPerm, ScType::Unknown >> "_trg");

        ScTemplateSearchResult res;
        REQUIRE(ctx.HelperSearchTemplate(templ, res));
        REQUIRE(res.Size() == 2);
        REQUIRE_FALSE(res[0]["_trg"] == res[1]["_trg"]);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"local\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
