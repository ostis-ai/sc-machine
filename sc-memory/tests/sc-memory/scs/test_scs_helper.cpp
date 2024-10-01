#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"

#include "dummy_file_interface.hpp"
#include "sc_test.hpp"

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

using SCsHelperTest = ScMemoryTest;

SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN

TEST_F(SCsHelperTest, GenerateBySCs)
{
  std::vector<std::pair<std::string, std::string>> tests =
  {
    { "x -> y;;", "x _-> _y;;" },
    { "x1 => nrel_x1: [test_content*];;", "x1 _=> nrel_x1:: _[];;" },
    { "x2 ~> y2 (* <- z2;; *);;", "x2 _~> _y2 (* <-_ _z2;; *);;" },
    { "x3 <- y3 (* <- sc_node_class;; *);;", "sc_node_class -> _y3;; _y3 _-> x3;;" },
  };

  for (auto const & t : tests)
  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

    EXPECT_TRUE(helper.GenerateBySCsText(t.first));

    ScTemplate templ;
    m_ctx->BuildTemplate(templ, t.second);

    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
    EXPECT_EQ(result.Size(), 1u);
  }
}

TEST_F(SCsHelperTest, GenerateBySCs_FileURL)
{
  std::string const data = "x -> \"file://test.scs\";;";

  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScTemplate templ;
  m_ctx->BuildTemplate(templ, "x _-> _[];;");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));

  EXPECT_EQ(result.Size(), 1u);

  ScAddr const linkAddr = result[0][2];
  EXPECT_TRUE(linkAddr.IsValid());

  ScType const linkType = m_ctx->GetElementType(linkAddr);
  EXPECT_EQ(linkType, ScType::ConstNodeLink);

  ScLink const link(*m_ctx, linkAddr);
  EXPECT_EQ(link.DetermineType(), ScLink::Type::Custom);
  EXPECT_EQ(link.GetAsString(), "content: file://test.scs");
}

TEST_F(SCsHelperTest, GenerateBySCs_Aliases)
{
  std::string const content = "SCsHelper_GenerateBySCs_Aliases";
  std::string const data = "@alias = [" + content + "];;";

  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScStreamPtr const stream = ScStreamMakeRead(content);

  ScAddrSet const links = m_ctx->SearchLinksByContent(stream);
  EXPECT_EQ(links.size(), 1u);
  auto it = std::next(links.begin(), 0u);

  ScLink const link(*m_ctx, *it);
  std::string const content2 = link.GetAsString();
  EXPECT_EQ(content, content2);
}

TEST_F(SCsHelperTest, GenerateBySCs_Contents)
{
  std::string const dataString = "v_string -> [string];;";
  std::string const dataFloat  = "v_float -> [^\"float:7\"];;";
  std::string const dataDouble = "v_double -> [^\"double:8\"];;";
  std::string const dataInt16  = "v_int16 -> [^\"int16:10\"];;";
  std::string const dataInt32  = "v_int32 -> [^\"int32:11\"];;";
  std::string const dataInt64  = "v_int64 -> [^\"int64:12\"];;";
  std::string const dataUint16 = "v_uint16 -> [^\"uint16:14\"];;";
  std::string const dataUint32 = "v_uint32 -> [^\"uint32:15\"];;";
  std::string const dataUint64 = "v_uint64 -> [^\"uint64:16\"];;";

  auto const testLink = [this](std::string const & data, std::string const & keynode)
  {
    SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
    EXPECT_TRUE(helper.GenerateBySCsText(data));

    ScTemplate templ;
    m_ctx->BuildTemplate(templ, keynode + " _-> _[];;");

    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));

    EXPECT_EQ(result.Size(), 1u);

    ScAddr const linkAddr = result[0][2];
    EXPECT_TRUE(linkAddr.IsValid());

    return linkAddr;
  };

  {
    ScLink const linkFloat(*m_ctx, testLink(dataString, "v_string"));
    EXPECT_EQ(linkFloat.DetermineType(), ScLink::Type::String);
    EXPECT_EQ(linkFloat.Get<std::string>(), "string");
  }

  {
    ScLink const linkFloat(*m_ctx, testLink(dataFloat, "v_float"));
    EXPECT_EQ(linkFloat.DetermineType(), ScLink::Type::Float);
    EXPECT_EQ(linkFloat.Get<float>(), 7.f);
  }

  {
    ScLink const linkDouble(*m_ctx, testLink(dataDouble, "v_double"));
    EXPECT_EQ(linkDouble.DetermineType(), ScLink::Type::Double);
    EXPECT_EQ(linkDouble.Get<double>(), 8.0);
  }

  {
    ScLink const linkInt16(*m_ctx, testLink(dataInt16, "v_int16"));
    EXPECT_EQ(linkInt16.DetermineType(), ScLink::Type::Int16);
    EXPECT_EQ(linkInt16.Get<int16_t>(), 10);
  }

  {
    ScLink const linkInt32(*m_ctx, testLink(dataInt32, "v_int32"));
    EXPECT_EQ(linkInt32.DetermineType(), ScLink::Type::Int32);
    EXPECT_EQ(linkInt32.Get<int32_t>(), 11);
  }

  {
    ScLink const linkInt64(*m_ctx, testLink(dataInt64, "v_int64"));
    EXPECT_EQ(linkInt64.DetermineType(), ScLink::Type::Int64);
    EXPECT_EQ(linkInt64.Get<int64_t>(), 12);
  }

  {
    ScLink const linkUint16(*m_ctx, testLink(dataUint16, "v_uint16"));
    EXPECT_EQ(linkUint16.DetermineType(), ScLink::Type::UInt16);
    EXPECT_EQ(linkUint16.Get<uint16_t>(), 14u);
  }

  {
    ScLink const linkUint32(*m_ctx, testLink(dataUint32, "v_uint32"));
    EXPECT_EQ(linkUint32.DetermineType(), ScLink::Type::UInt32);
    EXPECT_EQ(linkUint32.Get<uint32_t>(), 15u);
  }

  {
    ScLink const linkUint64(*m_ctx, testLink(dataUint64, "v_uint64"));
    EXPECT_EQ(linkUint64.DetermineType(), ScLink::Type::UInt64);
    EXPECT_EQ(linkUint64.Get<uint64_t>(), 16u);
  }
}

TEST_F(SCsHelperTest, GenerateBySCs_SingleNode)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string const scsData = "node <- sc_node_class;;";

  EXPECT_TRUE(helper.GenerateBySCsText(scsData));

  ScAddr const node = m_ctx->SearchElementBySystemIdentifier("node");
  EXPECT_TRUE(node.IsValid());
  EXPECT_EQ(m_ctx->GetElementType(node), ScType::ConstNodeClass);
}

TEST_F(SCsHelperTest, GenerateBySCs_Visibility_System)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  EXPECT_TRUE(helper.GenerateBySCsText("x -> y;;"));
  EXPECT_TRUE(helper.GenerateBySCsText("x ~> z;;"));

  ScAddr const xAddr = m_ctx->ResolveElementSystemIdentifier("x");
  EXPECT_TRUE(xAddr.IsValid());

  ScAddr const yAddr = m_ctx->ResolveElementSystemIdentifier("y");
  EXPECT_TRUE(yAddr.IsValid());

  ScAddr const zAddr = m_ctx->ResolveElementSystemIdentifier("z");
  EXPECT_TRUE(zAddr.IsValid());

  {
    ScTemplate templ;
    templ.Triple(xAddr, ScType::VarPermPosArc, yAddr);

    ScTemplateSearchResult res;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, res));
  }

  {
    ScTemplate templ;
    templ.Triple(xAddr, ScType::VarTempPosArc, zAddr);

    ScTemplateSearchResult res;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, res));
  }
}


TEST_F(SCsHelperTest, GenerateBySCs_Visibility_Global)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  {
    ScAddr const xAddr = m_ctx->ResolveElementSystemIdentifier("x_global");
    EXPECT_FALSE(xAddr.IsValid());
  }

  EXPECT_TRUE(helper.GenerateBySCsText("x_global -> .y;;"));
  EXPECT_TRUE(helper.GenerateBySCsText("x_global -> .y;;"));

  ScAddr const xAddr = m_ctx->ResolveElementSystemIdentifier("x_global");
  EXPECT_TRUE(xAddr.IsValid());

  ScTemplate templ;
  templ.Triple(xAddr, ScType::VarPermPosArc >> "_arc", ScType::Unknown >> "_trg");

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, res));
  EXPECT_EQ(res.Size(), 2u);
  EXPECT_EQ(res[0]["_trg"], res[1]["_trg"]);
  EXPECT_NE(res[0]["_arc"], res[1]["_arc"]);
}

TEST_F(SCsHelperTest, GenerateBySCs_Visibility_Local)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  {
    ScAddr const xAddr = m_ctx->ResolveElementSystemIdentifier("x_local");
    EXPECT_FALSE(xAddr.IsValid());
  }

  EXPECT_TRUE(helper.GenerateBySCsText("x_local -> ..y;;"));
  EXPECT_TRUE(helper.GenerateBySCsText("x_local -> ..z;;"));

  ScAddr const xAddr = m_ctx->ResolveElementSystemIdentifier("x_local");
  EXPECT_TRUE(xAddr.IsValid());

  ScTemplate templ;
  templ.Triple(xAddr, ScType::VarPermPosArc, ScType::Unknown >> "_trg");

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, res));
  EXPECT_EQ(res.Size(), 2u);
  EXPECT_NE(res[0]["_trg"], res[1]["_trg"]);
}

TEST_F(SCsHelperTest, GenerateAppendToStructure)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  ScAddr const outputStructure = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(outputStructure.IsValid());

  EXPECT_TRUE(helper.GenerateBySCsText(
    "class_1 -> class_1_instance_1;;"
    "class_1 -> class_1_instance_2;;"
    "class_1_instance_1 => rel_1: class_1_instance_2;;"
    "class_1_instance_2 => rel_2: class_1_instance_1;;",
    outputStructure
  ));

  ScTemplate templ;
  m_ctx->BuildTemplate(
    templ,
    "class_1 _-> _class_1_instance_1;;"
    "class_1 _-> _class_1_instance_2;;"
    "_class_1_instance_1 _=> rel_1:: _class_1_instance_2;;"
    "_class_1_instance_2 _=> rel_2:: _class_1_instance_1;;"
  );

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  result.ForEach([this, &outputStructure](ScTemplateSearchResultItem const & item) {
    for (size_t i = 0; i < item.Size(); ++i)
    {
      EXPECT_TRUE(m_ctx->CheckConnector(outputStructure, item[i], ScType::ConstPermPosArc));
    }
  });
}

TEST_F(SCsHelperTest, GenerateStructureAppendToStructure)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  ScAddr const outputStructure = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(outputStructure.IsValid());

  EXPECT_TRUE(helper.GenerateBySCsText(
    "example_structure = [*"
    "class_1 -> class_1_instance_1;;" // 3 new sc-elements
    "class_1 -> class_1_instance_2;;" // 2 new sc-elements
    "class_1_instance_1 => rel_1: class_1_instance_2;;" // 3 new sc-elements
    "class_1_instance_2 => rel_2: class_1_instance_1;;" // 3 new sc-elements
    "*];;", // example_structure must contains 11 sc-elements
    outputStructure
  ));

  ScAddr const & exampleStructure = m_ctx->SearchElementBySystemIdentifier("example_structure");
  EXPECT_EQ(m_ctx->GetElementType(exampleStructure), ScType::ConstNodeStructure);

  auto const checkInStruct = [this, outputStructure](std::string const & scsText, size_t const expectedStructNum) {
    ScTemplate templ;
   m_ctx->BuildTemplate(templ, scsText);

    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
    EXPECT_EQ(result.Size(), expectedStructNum);

    result.ForEach([this, &outputStructure](ScTemplateSearchResultItem const & item) {
      for (size_t i = 0; i < item.Size(); ++i)
      {
        EXPECT_TRUE(m_ctx->CheckConnector(outputStructure, item[i], ScType::ConstPermPosArc));
      }
    });
  };

  checkInStruct(
      "class_1 _-> _class_1_instance_1;;"
      "class_1 _-> _class_1_instance_2;;"
      "_class_1_instance_1 _=> rel_1:: _class_1_instance_2;;"
      "_class_1_instance_2 _=> rel_2:: _class_1_instance_1;;",
      1u
  );
  checkInStruct(
      "example_structure _-> _...;;",
      5u
  );
  checkInStruct(
      "example_structure _-> (_... _-> _...);;",
      4u
  );
  checkInStruct(
      "example_structure _-> (_... _=> _...);;",
      2u // total 5 + 4 + 2 = 11 sc-elements
  );
}

TEST_F(SCsHelperTest, FindTriplesSmoke)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  ScAddr const & outputStructureWithRuMainIdtf = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(outputStructureWithRuMainIdtf.IsValid());
  EXPECT_TRUE(helper.GenerateBySCsText(
      "test_node => nrel_main_idtf: [] (* <- lang_ru;; *);;",
      outputStructureWithRuMainIdtf
  ));

  ScAddr const & outputStructureWithEnMainIdtf = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(outputStructureWithEnMainIdtf.IsValid());
  EXPECT_TRUE(helper.GenerateBySCsText(
      "test_node => nrel_main_idtf: [] (* <- lang_en;; *);;",
      outputStructureWithEnMainIdtf
  ));

  ScAddr const & outputStructureWithRuIdtf = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(outputStructureWithRuIdtf.IsValid());
  EXPECT_TRUE(helper.GenerateBySCsText(
      "test_node => nrel_idtf: [] (* <- lang_ru;; *);;",
      outputStructureWithRuIdtf
  ));

  ScAddr const & outputStructureWithEnIdtf = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(outputStructureWithEnIdtf.IsValid());
  EXPECT_TRUE(helper.GenerateBySCsText(
      "test_node => nrel_idtf: [] (* <- lang_en;; *);;",
      outputStructureWithEnIdtf
  ));

  EXPECT_TRUE(helper.GenerateBySCsText(
      "lang_ru -> [];;"
      "lang_en -> [];;"
      "test_node => identification: [] (* <- lang_ru;; *);;"
      "test_node => identification: [] (* <- lang_en;; *);;"
  ));

  ScTemplate templ;
  m_ctx->BuildTemplate(
      templ,
      "test_node _=> nrel_main_idtf:: _[] (* <-_ lang_ru;; *);;"
  );

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  result.ForEach([this, &outputStructureWithRuMainIdtf](ScTemplateSearchResultItem const & item) {
    for (size_t i = 0; i < item.Size(); ++i)
    {
      EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithRuMainIdtf, item[i], ScType::ConstPermPosArc));
    }
  });

  result.Clear();
  {
    bool isFound = false;
    m_ctx->SearchByTemplate(templ, [this, &isFound, &outputStructureWithRuMainIdtf](ScTemplateSearchResultItem const & item) {
      isFound = true;
      for (size_t i = 0; i < item.Size(); ++i)
      {
        EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithRuMainIdtf, item[i], ScType::ConstPermPosArc));
      }
    }, [this, outputStructureWithRuMainIdtf](ScAddr const & elementAddr) -> bool {
      return m_ctx->CheckConnector(outputStructureWithRuMainIdtf, elementAddr, ScType::ConstPermPosArc);
    });
    EXPECT_TRUE(isFound);
  }

  templ.Clear();
  m_ctx->BuildTemplate(
      templ,
      "test_node _=> nrel_main_idtf:: _[] (* <-_ lang_en;; *);;"
  );

  result.Clear();
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);
  result.ForEach([this, &outputStructureWithEnMainIdtf](ScTemplateSearchResultItem const & item) {
    for (size_t i = 0; i < item.Size(); ++i)
    {
      EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithEnMainIdtf, item[i], ScType::ConstPermPosArc));
    }
  });

  result.Clear();
  {
    bool isFound = false;
    m_ctx->SearchByTemplate(templ, [this, &isFound, &outputStructureWithEnMainIdtf](ScTemplateSearchResultItem const & item) {
      isFound = true;
      for (size_t i = 0; i < item.Size(); ++i)
      {
        EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithEnMainIdtf, item[i], ScType::ConstPermPosArc));
      }
    }, [this, outputStructureWithEnMainIdtf](ScAddr const & elementAddr) -> bool {
      return m_ctx->CheckConnector(outputStructureWithEnMainIdtf, elementAddr, ScType::ConstPermPosArc);
    });
    EXPECT_TRUE(isFound);
  }

  templ.Clear();
  m_ctx->BuildTemplate(
      templ,
      "test_node _=> nrel_idtf:: _[] (* <-_ lang_ru;; *);;"
  );

  result.Clear();
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);
  result.ForEach([this, &outputStructureWithRuIdtf](ScTemplateSearchResultItem const & item) {
    for (size_t i = 0; i < item.Size(); ++i)
    {
      EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithRuIdtf, item[i], ScType::ConstPermPosArc));
    }
  });

  result.Clear();
  {
    bool isFound = false;
    m_ctx->SearchByTemplate(templ,[this, &isFound, &outputStructureWithRuIdtf](ScTemplateSearchResultItem const & item) {
      isFound = true;
      for (size_t i = 0; i < item.Size(); ++i)
      {
        EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithRuIdtf, item[i], ScType::ConstPermPosArc));
      }
    }, 
    [this, outputStructureWithRuIdtf](ScAddr const & elementAddr) -> bool {
      return m_ctx->CheckConnector(outputStructureWithRuIdtf, elementAddr, ScType::ConstPermPosArc);
    });
    EXPECT_TRUE(isFound);
  }

  templ.Clear();
  m_ctx->BuildTemplate(
      templ,
      "test_node _=> nrel_idtf:: _[] (* <-_ lang_en;; *);;"
  );

  result.Clear();
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);
  result.ForEach([this, &outputStructureWithEnIdtf](ScTemplateSearchResultItem const & item) {
    for (size_t i = 0; i < item.Size(); ++i)
    {
      EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithEnIdtf, item[i], ScType::ConstPermPosArc));
    }
  });

  result.Clear();
  {
    bool isFound = false;
    m_ctx->SearchByTemplate(templ, [this, &isFound, &outputStructureWithEnIdtf](ScTemplateSearchResultItem const & item) {
      isFound = true;
      for (size_t i = 0; i < item.Size(); ++i)
      {
        EXPECT_TRUE(m_ctx->CheckConnector(outputStructureWithEnIdtf, item[i], ScType::ConstPermPosArc));
      }
    }, 
    [this, outputStructureWithEnIdtf](ScAddr const & elementAddr) -> bool {
      return m_ctx->CheckConnector(outputStructureWithEnIdtf, elementAddr, ScType::ConstPermPosArc);
    });
    EXPECT_TRUE(isFound);
  }
}

SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
