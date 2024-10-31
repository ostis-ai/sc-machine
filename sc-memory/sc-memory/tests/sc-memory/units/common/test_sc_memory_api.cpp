/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>

#include <algorithm>

using ScMemoryAPITest = ScMemoryTest;

TEST_F(ScMemoryAPITest, ScMemory)
{
  EXPECT_TRUE(ScMemory::IsInitialized());
  EXPECT_TRUE(m_ctx->IsValid());
}

TEST_F(ScMemoryAPITest, MoveContext)
{
  ScMemoryContext context1;
  ScMemoryContext context2 = ScMemoryContext();
  EXPECT_TRUE(context2.IsValid());

  context1 = std::move(context2);

  EXPECT_TRUE(context1.IsValid());
  EXPECT_FALSE(context2.IsValid());
}

TEST_F(ScMemoryAPITest, GenerateElements)
{
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(nodeAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(nodeAddr));

  ScAddr const & linkAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);
  EXPECT_TRUE(linkAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(linkAddr));

  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::MembershipArc, nodeAddr, linkAddr);
  EXPECT_TRUE(arcAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(arcAddr));
}

TEST_F(ScMemoryAPITest, GenerateElementsWithInvalidTypes)
{
  EXPECT_THROW(m_ctx->GenerateNode(ScType::MembershipArc), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->GenerateLink(ScType::ConstNode), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->GenerateLink(ScType::MembershipArc), utils::ExceptionInvalidParams);

  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(nodeAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(nodeAddr));

  ScAddr const & linkAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);
  EXPECT_TRUE(linkAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(linkAddr));

  EXPECT_THROW(m_ctx->GenerateConnector(ScType::ConstNode, nodeAddr, linkAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->GenerateConnector(ScType::Const, nodeAddr, linkAddr), utils::ExceptionInvalidParams);
}

TEST_F(ScMemoryAPITest, SetGetFindSystemIdentifier)
{
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);

  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("test_node", addr));
  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(addr), "test_node");
  EXPECT_EQ(m_ctx->SearchElementBySystemIdentifier("test_node"), addr);
}

TEST_F(ScMemoryAPITest, SetGetFindSystemIdentifierWithOutFiver)
{
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);

  ScSystemIdentifierQuintuple setFiver;
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("test_node", addr, setFiver));
  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(addr), "test_node");

  ScSystemIdentifierQuintuple quintuple;
  EXPECT_TRUE(m_ctx->SearchElementBySystemIdentifier("test_node", quintuple));

  EXPECT_EQ(setFiver.addr1, quintuple.addr1);
  EXPECT_EQ(setFiver.addr2, quintuple.addr2);
  EXPECT_EQ(setFiver.addr3, quintuple.addr3);
  EXPECT_EQ(setFiver.addr4, quintuple.addr4);
  EXPECT_EQ(setFiver.addr5, quintuple.addr5);
  EXPECT_TRUE(setFiver.addr1.IsValid());
  EXPECT_TRUE(setFiver.addr2.IsValid());
  EXPECT_TRUE(setFiver.addr3.IsValid());
  EXPECT_TRUE(setFiver.addr4.IsValid());
  EXPECT_TRUE(setFiver.addr5.IsValid());
}

TEST_F(ScMemoryAPITest, SetGetSystemIdentifierErrorSetTwice)
{
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);

  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("test_node", addr));
  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(addr), "test_node");
  EXPECT_EQ(m_ctx->SearchElementBySystemIdentifier("test_node"), addr);

  ScAddr const & otherAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_FALSE(m_ctx->SetElementSystemIdentifier("test_node", otherAddr));
}

TEST_F(ScMemoryAPITest, ResolveGetSystemIdentifier)
{
  ScAddr const & addr = m_ctx->ResolveElementSystemIdentifier("test_node", ScType::ConstNode);

  EXPECT_TRUE(addr.IsValid());
  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(addr), "test_node");
  EXPECT_EQ(m_ctx->SearchElementBySystemIdentifier("test_node"), addr);
}

TEST_F(ScMemoryAPITest, ResolveGetSystemIdentifierWithOutFiver)
{
  ScSystemIdentifierQuintuple resolveQuintuple;
  EXPECT_TRUE(m_ctx->ResolveElementSystemIdentifier("test_node", ScType::ConstNode, resolveQuintuple));

  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(resolveQuintuple.addr1), "test_node");
  EXPECT_EQ(m_ctx->SearchElementBySystemIdentifier("test_node"), resolveQuintuple.addr1);

  ScSystemIdentifierQuintuple foundQuintuple;
  EXPECT_TRUE(m_ctx->SearchElementBySystemIdentifier("test_node", foundQuintuple));

  ScAddr addr;
  EXPECT_FALSE(m_ctx->SearchElementBySystemIdentifier("test_node1", addr));

  EXPECT_EQ(resolveQuintuple.addr1, foundQuintuple.addr1);
  EXPECT_EQ(resolveQuintuple.addr2, foundQuintuple.addr2);
  EXPECT_EQ(resolveQuintuple.addr3, foundQuintuple.addr3);
  EXPECT_EQ(resolveQuintuple.addr4, foundQuintuple.addr4);
  EXPECT_EQ(resolveQuintuple.addr5, foundQuintuple.addr5);
  EXPECT_TRUE(resolveQuintuple.addr1.IsValid());
  EXPECT_TRUE(resolveQuintuple.addr2.IsValid());
  EXPECT_TRUE(resolveQuintuple.addr3.IsValid());
  EXPECT_TRUE(resolveQuintuple.addr4.IsValid());
  EXPECT_TRUE(resolveQuintuple.addr5.IsValid());
}

SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN

TEST_F(ScMemoryAPITest, CreateNode_Deprecated)
{
  ScAddr const & nodeAddr = m_ctx->CreateNode(ScType::ConstNode);
  EXPECT_TRUE(nodeAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(nodeAddr));

  EXPECT_TRUE(m_ctx->EraseElement(nodeAddr));
}

TEST_F(ScMemoryAPITest, CreateLink_Deprecated)
{
  ScAddr const & linkAddr = m_ctx->CreateLink(ScType::ConstNodeLink);
  EXPECT_TRUE(linkAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(linkAddr));

  EXPECT_TRUE(m_ctx->EraseElement(linkAddr));
}

TEST_F(ScMemoryAPITest, CreateEdge_Deprecated)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::ConstPermPosArc, sourceNodeAddr, targetNodeAddr);
  EXPECT_TRUE(arcAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(arcAddr));

  EXPECT_TRUE(m_ctx->EraseElement(arcAddr));
}

TEST_F(ScMemoryAPITest, GetElementOutputArcsCount_Deprecated)
{
  ScAddr const & elementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(elementAddr.IsValid());

  size_t outputArcsCount = m_ctx->GetElementOutputArcsCount(elementAddr);
  EXPECT_EQ(outputArcsCount, 0u);
}

TEST_F(ScMemoryAPITest, GetElementInputArcsCount_Deprecated)
{
  ScAddr const & elementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(elementAddr.IsValid());

  size_t inputArcsCount = m_ctx->GetElementInputArcsCount(elementAddr);
  EXPECT_EQ(inputArcsCount, 0u);
}

TEST_F(ScMemoryAPITest, GetEdgeSource_Deprecated)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const & connectorAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, sourceNodeAddr, targetNodeAddr);

  ScAddr const & retrievedSourceAddr = m_ctx->GetEdgeSource(connectorAddr);
  EXPECT_EQ(retrievedSourceAddr, sourceNodeAddr);
}

TEST_F(ScMemoryAPITest, GetEdgeTarget_Deprecated)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const & connectorAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, sourceNodeAddr, targetNodeAddr);

  ScAddr const & retrievedTargetAddr = m_ctx->GetEdgeTarget(connectorAddr);
  EXPECT_EQ(retrievedTargetAddr, targetNodeAddr);
}

TEST_F(ScMemoryAPITest, GetEdgeInfo_Deprecated)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::ConstPermPosArc, sourceNodeAddr, targetNodeAddr);
  EXPECT_TRUE(arcAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(arcAddr));

  ScAddr retrievedSourceAddr;
  ScAddr retrievedTargetAddr;
  m_ctx->GetEdgeInfo(arcAddr, retrievedSourceAddr, retrievedTargetAddr);
  EXPECT_EQ(retrievedSourceAddr, sourceNodeAddr);
  EXPECT_EQ(retrievedTargetAddr, targetNodeAddr);
}

TEST_F(ScMemoryAPITest, FindLinksByContent_Deprecated)
{
  ScAddr const & linkAddr1 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr1, "content");
  ScAddr const & linkAddr2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr2, "content");
  ScAddr const & linkAddr3 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr3, "content");

  ScAddrVector const & foundLinks = m_ctx->FindLinksByContent("content");

  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr1) != foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr2) != foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr3) != foundLinks.cend());
}

TEST_F(ScMemoryAPITest, FindLinksByContentWithStream_Deprecated)
{
  ScAddr const & linkAddr1 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr1, "content");
  ScAddr const & linkAddr2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr2, "content");
  ScAddr const & linkAddr3 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr3, "content");

  ScAddrVector const & foundLinks = m_ctx->FindLinksByContent(ScStreamMakeRead("content"));

  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr1) != foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr2) != foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr3) != foundLinks.cend());
}

TEST_F(ScMemoryAPITest, FindLinksByContentSubstring_Deprecated)
{
  ScAddr const & linkAddr1 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr1, "Hello, world!");

  ScAddr const & linkAddr2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr2, "This is a test link.");

  ScAddr const & linkAddr3 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr3, "Another link with different content.");

  ScAddrVector const & foundLinks = m_ctx->FindLinksByContentSubstring("link");

  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr1) == foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr2) != foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr3) != foundLinks.cend());
}

TEST_F(ScMemoryAPITest, FindLinksByContentSubstringWithStream_Deprecated)
{
  ScAddr const & linkAddr1 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr1, "Hello, world!");

  ScAddr const & linkAddr2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr2, "This is a test link.");

  ScAddr const & linkAddr3 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr3, "Another link with different content.");

  ScAddrVector const & foundLinks = m_ctx->FindLinksByContentSubstring(ScStreamMakeRead("link"));

  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr1) == foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr2) != foundLinks.cend());
  EXPECT_TRUE(std::find(foundLinks.cbegin(), foundLinks.cend(), linkAddr3) != foundLinks.cend());
}

TEST_F(ScMemoryAPITest, FindLinksContentsByContentSubstring_Deprecated)
{
  ScAddr const & linkAddr1 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr1, "Hello, world!");

  ScAddr const & linkAddr2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr2, "This is a test link.");

  ScAddr const & linkAddr3 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr3, "Another link with different content.");

  std::vector<std::string> const & foundLinkContents = m_ctx->FindLinksContentsByContentSubstring("link");

  EXPECT_TRUE(
      std::find(foundLinkContents.cbegin(), foundLinkContents.cend(), "Hello, world!") == foundLinkContents.cend());
  EXPECT_TRUE(
      std::find(foundLinkContents.cbegin(), foundLinkContents.cend(), "This is a test link.")
      != foundLinkContents.cend());
  EXPECT_TRUE(
      std::find(foundLinkContents.cbegin(), foundLinkContents.cend(), "Another link with different content.")
      != foundLinkContents.cend());
}

TEST_F(ScMemoryAPITest, FindLinksContentsByContentSubstringWithStream_Deprecated)
{
  ScAddr const & linkAddr1 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr1, "Hello, world!");

  ScAddr const & linkAddr2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr2, "This is a test link.");

  ScAddr const & linkAddr3 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkAddr3, "Another link with different content.");

  std::vector<std::string> const & foundLinkContents =
      m_ctx->FindLinksContentsByContentSubstring(ScStreamMakeRead("link"));

  EXPECT_TRUE(
      std::find(foundLinkContents.cbegin(), foundLinkContents.cend(), "Hello, world!") == foundLinkContents.cend());
  EXPECT_TRUE(
      std::find(foundLinkContents.cbegin(), foundLinkContents.cend(), "This is a test link.")
      != foundLinkContents.cend());
  EXPECT_TRUE(
      std::find(foundLinkContents.cbegin(), foundLinkContents.cend(), "Another link with different content.")
      != foundLinkContents.cend());
}

TEST_F(ScMemoryAPITest, Iterator3_Deprecated)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScType const & arcType = ScType::ConstCommonArc;

  m_ctx->GenerateConnector(arcType, nodeAddr1, nodeAddr2);

  EXPECT_TRUE(m_ctx->Iterator3(nodeAddr1, arcType, nodeAddr2)->Next());
}

TEST_F(ScMemoryAPITest, Iterator5_Deprecated)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScType const & arcType = ScType::ConstCommonArc;

  ScAddr const & arcAddr = m_ctx->GenerateConnector(arcType, nodeAddr1, nodeAddr2);
  m_ctx->GenerateConnector(arcType, nodeAddr2, arcAddr);

  EXPECT_TRUE(m_ctx->Iterator5(nodeAddr1, arcType, nodeAddr2, arcType, nodeAddr2)->Next());
}

TEST_F(ScMemoryAPITest, ForEachIter3_Deprecated)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScType const & arcType = ScType::ConstCommonArc;

  m_ctx->GenerateConnector(arcType, nodeAddr1, nodeAddr2);

  size_t count = 0;
  m_ctx->ForEachIter3(
      nodeAddr1,
      arcType,
      nodeAddr2,
      [&](ScAddr const &, ScAddr const &, ScAddr const &)
      {
        ++count;
      });
  EXPECT_EQ(count, 1u);
}

TEST_F(ScMemoryAPITest, ForEachIter5_Deprecated)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScType const & arcType = ScType::ConstCommonArc;

  ScAddr const & arcAddr = m_ctx->GenerateConnector(arcType, nodeAddr1, nodeAddr2);
  m_ctx->GenerateConnector(arcType, nodeAddr2, arcAddr);

  size_t count = 0;
  m_ctx->ForEachIter5(
      nodeAddr1,
      arcType,
      nodeAddr2,
      arcType,
      nodeAddr2,
      [&](ScAddr const &, ScAddr const &, ScAddr const &, ScAddr const &, ScAddr const &)
      {
        ++count;
      });
  EXPECT_EQ(count, 1u);
}

TEST_F(ScMemoryAPITest, HelperCheckEdge_Deprecated)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScType const & arcType = ScType::ConstCommonArc;

  m_ctx->GenerateConnector(arcType, nodeAddr1, nodeAddr2);

  EXPECT_TRUE(m_ctx->HelperCheckEdge(nodeAddr1, nodeAddr2, arcType));
}

TEST_F(ScMemoryAPITest, HelperResolveSystemIdtf_Deprecated)
{
  std::string systemIdentifier = "example_identifier";
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SetElementSystemIdentifier(systemIdentifier, nodeAddr);

  ScAddr resolvedAddr = m_ctx->HelperResolveSystemIdtf(systemIdentifier, ScType::ConstNodeClass);
  EXPECT_EQ(resolvedAddr, nodeAddr);
}

TEST_F(ScMemoryAPITest, HelperResolveSystemIdtfWithQuintuple_Deprecated)
{
  std::string systemIdentifier = "example_identifier";
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SetElementSystemIdentifier(systemIdentifier, nodeAddr);

  ScSystemIdentifierQuintuple quintuple;
  EXPECT_TRUE(m_ctx->HelperResolveSystemIdtf(systemIdentifier, ScType::ConstNodeClass, quintuple));
  EXPECT_EQ(quintuple.addr1, nodeAddr);
}

TEST_F(ScMemoryAPITest, HelperFindBySystemIdtf_Deprecated)
{
  std::string systemIdentifier = "example_identifier";
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SetElementSystemIdentifier(systemIdentifier, nodeAddr);

  ScAddr foundAddr;
  EXPECT_TRUE(m_ctx->HelperFindBySystemIdtf(systemIdentifier, foundAddr));
  EXPECT_EQ(foundAddr, nodeAddr);
  foundAddr = m_ctx->HelperFindBySystemIdtf(systemIdentifier);
  EXPECT_EQ(foundAddr, nodeAddr);
}

TEST_F(ScMemoryAPITest, HelperFindBySystemIdtfWithQuintuple_Deprecated)
{
  std::string systemIdentifier = "example_identifier";
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SetElementSystemIdentifier(systemIdentifier, nodeAddr);

  ScSystemIdentifierQuintuple quintuple;
  EXPECT_TRUE(m_ctx->HelperFindBySystemIdtf(systemIdentifier, quintuple));
  EXPECT_EQ(quintuple.addr1, nodeAddr);
}

TEST_F(ScMemoryAPITest, HelperGetSystemIdtf_Deprecated)
{
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  std::string systemIdentifier = "example_identifier";
  m_ctx->SetElementSystemIdentifier(systemIdentifier, nodeAddr);

  EXPECT_EQ(m_ctx->HelperGetSystemIdtf(nodeAddr), systemIdentifier);
}

TEST_F(ScMemoryAPITest, HelperSetSystemIdtf_Deprecated)
{
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(nodeAddr), "");

  std::string systemIdentifier = "example_identifier";
  m_ctx->HelperSetSystemIdtf(systemIdentifier, nodeAddr);

  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(nodeAddr), systemIdentifier);
}

TEST_F(ScMemoryAPITest, HelperSetSystemIdtfWithQuintuple_Deprecated)
{
  ScAddr const & nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  std::string systemIdentifier = "example_identifier";
  ScSystemIdentifierQuintuple quintuple;
  m_ctx->HelperSetSystemIdtf(systemIdentifier, nodeAddr, quintuple);
  EXPECT_EQ(quintuple.addr1, nodeAddr);

  EXPECT_EQ(m_ctx->GetElementSystemIdentifier(nodeAddr), systemIdentifier);
}

TEST_F(ScMemoryAPITest, HelperGenTemplate_Deprecated)
{
  ScAddr const & classAddr = m_ctx->ResolveElementSystemIdentifier("my_class", ScType::ConstNode);

  ScTemplate templateToGenerate;
  templateToGenerate.Triple(classAddr, ScType::VarPermPosArc >> "_arc", ScType::VarNode >> "_addr2");

  ScTemplateResultItem result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templateToGenerate, result));
  EXPECT_TRUE(m_ctx->IsElement(result["_addr2"]));

  EXPECT_TRUE(m_ctx->CheckConnector(classAddr, result["_addr2"], ScType::ConstPermPosArc));
}

TEST_F(ScMemoryAPITest, HelperSearchTemplate_Deprecated)
{
  ScAddr const & classAddr = m_ctx->ResolveElementSystemIdentifier("my_class", ScType::ConstNode);
  ScAddr const & setAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScTemplate templateToFind;
  templateToFind.Triple(classAddr, ScType::VarPermPosArc >> "_arc", ScType::VarNode >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templateToFind, result));

  m_ctx->HelperSearchTemplate(
      templateToFind,
      [&](ScTemplateSearchResultItem const & item)
      {
        m_ctx->GenerateConnector(ScType::ConstTempPosArc, setAddr, item["_addr2"]);
      },
      [&](ScTemplateSearchResultItem const & item) -> bool
      {
        return true;
      },
      [&](ScAddr const & elementAddr) -> bool
      {
        return true;
      });

  EXPECT_TRUE(m_ctx->HelperCheckEdge(setAddr, result["_addr2"], ScType::ConstTempPosArc));

  m_ctx->HelperSearchTemplate(
      templateToFind,
      [&](ScTemplateSearchResultItem const & item)
      {
        m_ctx->GenerateConnector(ScType::ConstPermPosArc, setAddr, item["_addr2"]);
      },
      [&](ScAddr const & elementAddr) -> bool
      {
        return true;
      });

  EXPECT_TRUE(m_ctx->HelperCheckEdge(setAddr, result["_addr2"], ScType::ConstPermPosArc));
}

TEST_F(ScMemoryAPITest, HelperSmartSearchTemplate_Deprecated)
{
  ScAddr const & classAddr = m_ctx->ResolveElementSystemIdentifier("my_class", ScType::ConstNode);
  ScAddr const & setAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScTemplate templateToFind;
  templateToFind.Triple(classAddr, ScType::VarPermPosArc >> "_arc", ScType::VarNode >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templateToFind, result));

  m_ctx->HelperSmartSearchTemplate(
      templateToFind,
      [&](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest
      {
        m_ctx->GenerateConnector(ScType::ConstTempPosArc, setAddr, item["_addr2"]);
        return ScTemplateSearchRequest::CONTINUE;
      },
      [&](ScTemplateSearchResultItem const & item) -> bool
      {
        return true;
      },
      [&](ScAddr const & elementAddr) -> bool
      {
        return true;
      });

  EXPECT_TRUE(m_ctx->HelperCheckEdge(setAddr, result["_addr2"], ScType::ConstTempPosArc));

  m_ctx->HelperSmartSearchTemplate(
      templateToFind,
      [&](ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest
      {
        m_ctx->GenerateConnector(ScType::ConstPermPosArc, setAddr, item["_addr2"]);
        return ScTemplateSearchRequest::CONTINUE;
      },
      [&](ScAddr const & elementAddr) -> bool
      {
        return true;
      });

  EXPECT_TRUE(m_ctx->HelperCheckEdge(setAddr, result["_addr2"], ScType::ConstPermPosArc));
}

TEST_F(ScMemoryAPITest, HelperBuildTemplate_Deprecated)
{
  ScAddr const addr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("d", addr));

  ScTemplate templ;
  sc_char const * data = "_a _-> d;; _a <- sc_node_class;;";
  m_ctx->HelperBuildTemplate(templ, data);
}

class ScTemplateLoadContext : public ScMemoryContext
{
public:
  void LoadTemplate(
      ScTemplate & translatableTemplate,
      ScAddr & resultTemplateAddr,
      ScTemplateParams const & params = ScTemplateParams())
  {
    ScMemoryContext::LoadTemplate(translatableTemplate, resultTemplateAddr, params);
  }
};

TEST_F(ScMemoryAPITest, GenerateSearchLoadCheckBuildSearchTemplate_Deprecated)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & testRelationAddr = m_ctx->GenerateNode(ScType::ConstNodeNonRole);

  ScTemplate templ;
  templ.Triple(testClassAddr, ScType::VarPermPosArc >> "_arc_to_test_object", ScType::VarNodeLink >> "_test_object");
  templ.Quintuple(
      "_test_object", ScType::VarCommonArc, ScType::VarNode >> "_test_set", ScType::VarPermPosArc, testRelationAddr);
  templ.Triple("_test_set", ScType::VarPermPosArc, "_arc_to_test_object");
  EXPECT_EQ(templ.Size(), 4u);

  ScTemplateGenResult genResult;
  m_ctx->HelperGenTemplate(templ, genResult);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);

  ScAddr templAddr;
  ScTemplateLoadContext ctx;
  ctx.LoadTemplate(templ, templAddr);

  ScTemplate builtTemplate;
  m_ctx->HelperBuildTemplate(builtTemplate, templAddr);

  EXPECT_EQ(builtTemplate.Size(), 4u);

  EXPECT_TRUE(m_ctx->HelperSearchTemplate(builtTemplate, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
}

TEST_F(ScMemoryAPITest, CalculateStat_Deprecated)
{
  EXPECT_NO_THROW(m_ctx->CalculateStat());
}

SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
