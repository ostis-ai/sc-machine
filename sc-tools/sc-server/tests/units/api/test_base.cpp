/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_test.hpp"

extern "C"
{
#include <sc-core/sc_types.h>
}

#include <sc-memory/sc_type.hpp>

#include "sc-client/sc_client.hpp"

#include "sc-client/sc_memory_json_converter.hpp"

TEST_F(ScServerTest, GenerateElements)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "create_elements",
      ScMemoryJsonPayload::array({
          {
              {"el", "node"},
              {"type", sc_type_node | sc_type_const},
          },
          {
              {"el", "link"},
              {"type", sc_type_const_node_link},
              {"content", "connector_end"},
          },
          {
              {"el", "edge"},
              {"src",
               {
                   {"type", "ref"},
                   {"value", 0},
               }},
              {"trg",
               {
                   {"type", "ref"},
                   {"value", 1},
               }},
              {"type", sc_type_const_perm_pos_arc},
          },
          {
              {"el", "link"},
              {"type", sc_type_const_node_link},
              {"content", 100},
          },
          {
              {"el", "link"},
              {"type", sc_type_const_node_link},
              {"content", 100.0f},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  ScAddr const & src = ScAddr(responsePayload[0].get<size_t>());
  EXPECT_TRUE(src.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(src).IsNode());
  ScAddr const & connector = ScAddr(responsePayload[2].get<size_t>());
  EXPECT_TRUE(connector.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(connector).IsConnector());
  ScAddr const & trg = ScAddr(responsePayload[1].get<size_t>());
  EXPECT_TRUE(trg.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(trg).IsLink());

  auto const & links = m_ctx->SearchLinksByContent("connector_end");
  EXPECT_TRUE(std::find(links.begin(), links.end(), trg) != links.end());

  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(src, sc_type_const_perm_pos_arc, trg);
  EXPECT_TRUE(iter3->Next());
  EXPECT_TRUE(iter3->Get(1) == connector);

  ScAddr const & linkInt = ScAddr(responsePayload[3].get<size_t>());
  EXPECT_TRUE(linkInt.IsValid());
  sc_int contentInt;
  m_ctx->GetLinkContent(linkInt, contentInt);
  EXPECT_EQ(contentInt, 100);

  ScAddr const & linkFloat = ScAddr(responsePayload[4].get<size_t>());
  EXPECT_TRUE(linkFloat.IsValid());
  float contentFloat;
  m_ctx->GetLinkContent(linkFloat, contentFloat);
  EXPECT_EQ(contentFloat, 100.0f);

  client.Stop();
}

TEST_F(ScServerTest, GenerateEmptyElements)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(0, "create_elements", ScMemoryJsonPayload::array({}));

  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  EXPECT_TRUE(responsePayload.is_object());
  EXPECT_TRUE(responsePayload.empty());

  client.Stop();
}

TEST_F(ScServerTest, GenerateElementsBySCs)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "create_elements_by_scs",
      ScMemoryJsonPayload::array({
          "concept_set -> set1;;",
          "concept_set -> ",
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"][0]["message"].is_null());

  ScAddr const & classSet = m_ctx->SearchElementBySystemIdentifier("concept_set");
  EXPECT_TRUE(classSet.IsValid());
  ScAddr const & set1 = m_ctx->SearchElementBySystemIdentifier("set1");
  EXPECT_TRUE(set1.IsValid());
  EXPECT_TRUE(m_ctx->CheckConnector(classSet, set1, ScType::ConstPermPosArc));
  EXPECT_TRUE(responsePayload[0].get<sc_bool>());

  EXPECT_FALSE(responsePayload[1].get<sc_bool>());

  client.Stop();
}

TEST_F(ScServerTest, GenerateElementsBySCsUploadToStructure)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const structure = m_ctx->GenerateNode(ScType::ConstNodeStructure);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "create_elements_by_scs",
      ScMemoryJsonPayload::array({
          {{"scs", "concept_set -> set1;;"}, {"output_structure", structure.Hash()}},
          {{"scs", "concept_set ->;;"}, {"output_structure", structure.Hash()}},
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"][0]["message"].is_null());

  ScSystemIdentifierQuintuple classSetSysIdtfFiver;
  EXPECT_TRUE(m_ctx->SearchElementBySystemIdentifier("concept_set", classSetSysIdtfFiver));
  ScAddr const & classSet = classSetSysIdtfFiver.addr1;
  EXPECT_TRUE(classSet.IsValid());

  ScSystemIdentifierQuintuple set1SysIdtfFiver;
  EXPECT_TRUE(m_ctx->SearchElementBySystemIdentifier("set1", set1SysIdtfFiver));
  ScAddr const & set1 = set1SysIdtfFiver.addr1;
  EXPECT_TRUE(set1.IsValid());

  ScIterator3Ptr it3 = m_ctx->CreateIterator3(classSet, ScType::ConstPermPosArc, set1);
  EXPECT_TRUE(it3->Next());
  ScAddr const & connector = it3->Get(1);
  EXPECT_TRUE(connector.IsValid());

  EXPECT_TRUE(m_ctx->CheckConnector(structure, classSet, ScType::ConstPermPosArc));
  EXPECT_TRUE(m_ctx->CheckConnector(structure, set1, ScType::ConstPermPosArc));
  EXPECT_TRUE(m_ctx->CheckConnector(structure, connector, ScType::ConstPermPosArc));

  EXPECT_TRUE(m_ctx->CheckConnector(structure, classSetSysIdtfFiver.addr2, ScType::ConstPermPosArc));
  EXPECT_TRUE(m_ctx->CheckConnector(structure, classSetSysIdtfFiver.addr3, ScType::ConstPermPosArc));
  EXPECT_TRUE(m_ctx->CheckConnector(structure, classSetSysIdtfFiver.addr4, ScType::ConstPermPosArc));
  EXPECT_TRUE(m_ctx->CheckConnector(structure, classSetSysIdtfFiver.addr5, ScType::ConstPermPosArc));

  EXPECT_TRUE(m_ctx->CheckConnector(structure, set1SysIdtfFiver.addr2, ScType::ConstPermPosArc));
  EXPECT_TRUE(m_ctx->CheckConnector(structure, set1SysIdtfFiver.addr3, ScType::ConstPermPosArc));
  EXPECT_TRUE(m_ctx->CheckConnector(structure, set1SysIdtfFiver.addr4, ScType::ConstPermPosArc));

  EXPECT_TRUE(responsePayload[0].get<sc_bool>());

  EXPECT_FALSE(responsePayload[1].get<sc_bool>());

  client.Stop();
}

TEST_F(ScServerTest, GenerateEmptyElementsBySCs)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString =
      ScMemoryJsonConverter::From(0, "create_elements_by_scs", ScMemoryJsonPayload::array({}));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  EXPECT_TRUE(responsePayload.is_array());
  EXPECT_TRUE(responsePayload.empty());

  client.Stop();
}

TEST_F(ScServerTest, GenerateElementsByWrongSCs)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "create_elements_by_scs",
      ScMemoryJsonPayload::array({
          "concept_set -> node;",
          "concept_set -> ",
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"].size() == 2);

  EXPECT_TRUE(responsePayload.is_array());
  EXPECT_FALSE(responsePayload.empty());

  client.Stop();
}

TEST_F(ScServerTest, CheckElements)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & src = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & trg = m_ctx->GenerateLink();
  ScAddr const & connector = m_ctx->GenerateConnector(ScType::ConstCommonArc, src, trg);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "check_elements",
      ScMemoryJsonPayload::array({
          src.Hash(),
          connector.Hash(),
          trg.Hash(),
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  sc_type const & srcType = responsePayload[0].get<sc_type>();
  EXPECT_TRUE(*m_ctx->GetElementType(src) == srcType);
  sc_type const & connectorType = responsePayload[1].get<sc_type>();
  EXPECT_TRUE(*m_ctx->GetElementType(connector) == connectorType);
  sc_type const & trgType = responsePayload[2].get<sc_type>();
  EXPECT_TRUE(*m_ctx->GetElementType(trg) == trgType);

  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(src, ScType::ConstCommonArc, trg);
  EXPECT_TRUE(iter3->Next());

  client.Stop();
}

TEST_F(ScServerTest, EraseElements)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & src = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & trg = m_ctx->GenerateLink();
  ScAddr const & connector = m_ctx->GenerateConnector(ScType::ConstPermPosArc, src, trg);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "delete_elements",
      ScMemoryJsonPayload::array({
          src.Hash(),
          connector.Hash(),
          trg.Hash(),
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  ScIterator3Ptr const iter3 = m_ctx->CreateIterator3(src, ScType::ConstPermPosArc, trg);
  EXPECT_FALSE(iter3->Next());

  client.Stop();
}

TEST_F(ScServerTest, HandleKeynodes)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "keynodes",
      ScMemoryJsonPayload::array({
          {
              {"command", "resolve"},
              {"idtf", "any_system_identifier"},
              {"elType", sc_type_node | sc_type_const | sc_type_node_class},
          },
          {
              {"command", "find"},
              {"idtf", "any_system_identifier"},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  ScAddr const & addr1 = ScAddr(responsePayload[0].get<size_t>());
  EXPECT_TRUE(addr1.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(addr1) == ScType::ConstNodeClass);
  ScAddr const & addr2 = ScAddr(responsePayload[1].get<size_t>());
  EXPECT_TRUE(addr2.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(addr2) == ScType::ConstNodeClass);
  EXPECT_TRUE(addr1 == addr2);
  EXPECT_TRUE("any_system_identifier" == m_ctx->GetElementSystemIdentifier(addr1));

  client.Stop();
}

TEST_F(ScServerTest, HandleEmptyKeynodes)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(0, "keynodes", ScMemoryJsonPayload::array({}));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  EXPECT_TRUE(responsePayload.is_object());
  EXPECT_TRUE(responsePayload.empty());

  client.Stop();
}

TEST_F(ScServerTest, HandleContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "set"},
              {"type", "string"},
              {"data", "some content"},
              {"addr", link.Hash()},
          },
          {
              {"command", "get"},
              {"addr", link.Hash()},
          },
          {
              {"command", "find"},
              {"data", "some content"},
          },
          {
              {"command", "find_by_substr"},
              {"data", "some"},
          },
          {
              {"command", "find_links_by_substr"},
              {"data", "some"},
          },
          {
              {"command", "find_strings_by_substr"},
              {"data", "some"},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  EXPECT_TRUE(responsePayload[0].get<sc_bool>());
  EXPECT_TRUE(responsePayload[1]["value"].get<std::string>() == "some content");
  auto links = responsePayload[2].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  links = responsePayload[3].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  links = responsePayload[4].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  auto strings = responsePayload[5].get<std::vector<std::string>>();
  EXPECT_FALSE(strings.empty());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "some content") != strings.end());

  client.Stop();
}

TEST_F(ScServerTest, SetContentForNode)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "set"},
              {"type", "string"},
              {"data", "some content"},
              {"addr", nodeAddr.Hash()},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());

  client.Stop();
}

TEST_F(ScServerTest, SetContentForEdge)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const connectorAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr1, nodeAddr2);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "set"},
              {"type", "string"},
              {"data", "some content"},
              {"addr", connectorAddr.Hash()},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());

  client.Stop();
}

TEST_F(ScServerTest, SetContentForInvalidLink)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "set"},
              {"type", "string"},
              {"data", "some content"},
              {"addr", 999999999},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());

  client.Stop();
}

TEST_F(ScServerTest, GetContentForNode)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "get"},
              {"addr", nodeAddr.Hash()},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());

  client.Stop();
}

TEST_F(ScServerTest, GetContentForEdge)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const connectorAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr1, nodeAddr2);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "get"},
              {"addr", connectorAddr.Hash()},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());

  client.Stop();
}

TEST_F(ScServerTest, GetContentForInvalidLink)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "get"},
              {"addr", 999999999},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());

  client.Stop();
}

TEST_F(ScServerTest, HandleContentOld)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"type", "string"},
          {"data", "content"},
          {"addr", link.Hash()},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  EXPECT_TRUE(responsePayload[0].get<sc_bool>());
  std::string content;
  m_ctx->GetLinkContent(link, content);
  EXPECT_TRUE(content == "content");

  client.Stop();
}

TEST_F(ScServerTest, HandleIntContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "set"},
              {"type", "int"},
              {"data", 5226},
              {"addr", link.Hash()},
          },
          {
              {"command", "get"},
              {"addr", link.Hash()},
          },
          {
              {"command", "find"},
              {"data", 5226},
          },
          {
              {"command", "find_by_substr"},
              {"data", 5226},
          },
          {
              {"command", "find_links_by_substr"},
              {"data", 5226},
          },
          {
              {"command", "find_strings_by_substr"},
              {"data", 5226},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  EXPECT_TRUE(responsePayload[0].get<sc_bool>());
  EXPECT_TRUE(responsePayload[1]["value"].get<sc_int>() == 5226);
  auto links = responsePayload[2].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  links = responsePayload[3].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  links = responsePayload[4].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  auto strings = responsePayload[5].get<std::vector<std::string>>();
  EXPECT_FALSE(strings.empty());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "5226") != strings.end());

  client.Stop();
}

TEST_F(ScServerTest, HandleFloatContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      ScMemoryJsonPayload::array({
          {
              {"command", "set"},
              {"type", "float"},
              {"data", 10.53f},
              {"addr", link.Hash()},
          },
          {
              {"command", "get"},
              {"addr", link.Hash()},
          },
          {
              {"command", "find"},
              {"data", 10.53f},
          },
          {
              {"command", "find_by_substr"},
              {"data", 10.53f},
          },
          {
              {"command", "find_links_by_substr"},
              {"data", 10.53f},
          },
          {
              {"command", "find_strings_by_substr"},
              {"data", 10.53f},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  EXPECT_TRUE(responsePayload[0].get<sc_bool>());
  EXPECT_TRUE(responsePayload[1]["value"].get<float>() == 10.53f);
  auto links = responsePayload[2].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  links = responsePayload[3].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  links = responsePayload[4].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());
  auto strings = responsePayload[5].get<std::vector<std::string>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(strings.begin(), strings.end(), "10.53") != strings.end());

  client.Stop();
}

TEST_F(ScServerTest, HandleContentWithoutCommand)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(0, "content", {});
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());

  client.Stop();
}

TEST_F(ScServerTest, HandleContentWithUndefinedCommand)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", 1221},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, HandleContentWithInvalidCommand)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "invalid"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithoutSpecifiedAddr)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithInvalidAddr)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", "my link"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithoutSpecifiedContentType)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", link.Hash()},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithoutInvalidContentType)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", link.Hash()},
          {"type", {}},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithoutSpecifiedContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", link.Hash()},
          {"type", "string"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithInvalidStringContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", link.Hash()},
          {"type", "string"},
          {"data", {}},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithInvalidIntContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", link.Hash()},
          {"type", "int"},
          {"data", {}},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithInvalidFloatContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", link.Hash()},
          {"type", "float"},
          {"data", {}},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SetContentWithUnknownContentType)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "set"},
          {"addr", link.Hash()},
          {"type", "unknown"},
          {"data", {}},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, GetContentWithoutSpecifiedAddr)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "get"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, GetContentWithInvalidAddr)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "get"},
          {"addr", "invalid"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SearchLinksWithoutSpecifiedContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "find"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SearchLinksWithInvalidContent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(0, "content", {{"command", "find"}, {"data", {}}});
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SearchLinksWithoutSpecifiedContentSubstring)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "find_links_by_substr"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SearchLinksWithInvalidContentSubstring)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString =
      ScMemoryJsonConverter::From(0, "content", {{"command", "find_links_by_substr"}, {"data", {}}});
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SearchLinksContentsWithoutSpecifiedContentSubstring)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "find_strings_by_substr"},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SearchLinksContentsWithInvalidContentSubstring)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->GenerateLink();

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "content",
      {
          {"command", "find_strings_by_substr"},
          {"data", {}},
      });
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_TRUE(response["errors"][0].is_string());

  client.Stop();
}

TEST_F(ScServerTest, SearchTemplate)
{
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & link = m_ctx->GenerateLink();
  ScAddr const & nonRoleAddr = m_ctx->GenerateNode(ScType::ConstNodeNonRole);

  ScAddr const & connectorAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, addr, link);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nonRoleAddr, connectorAddr);

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"] = ScMemoryJsonPayload::array({
      {
          {
              {"type", "type"},
              {"value", *ScType::VarNode},
              {"alias", "_src"},
          },
          {
              {"type", "type"},
              {"value", *ScType::VarCommonArc},
              {"alias", "_connector1"},
          },
          {
              {"type", "type"},
              {"value", *ScType::VarNodeLink},
              {"alias", "_trg"},

          },
      },
      {
          {
              {"type", "addr"},
              {"value", nonRoleAddr.Hash()},
          },
          {
              {"type", "type"},
              {"value", *ScType::VarPermPosArc},
              {"alias", "_connector2"},
          },
          {
              {"type", "alias"},
              {"value", "_connector1"},
          },
      },
  });
  payload["params"]["_src"] = addr.Hash();
  payload["params"]["_trg"] = link.Hash();
  std::string const payloadString = ScMemoryJsonConverter::From(0, "search_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"][0].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]) == addr);
  EXPECT_TRUE(ScAddr(addrs[2]) == link);
  EXPECT_TRUE(ScAddr(addrs[3]) == nonRoleAddr);

  client.Stop();
}

TEST_F(ScServerTest, SearchStringTemplate)
{
  ScAddr const & addr1 = m_ctx->ResolveElementSystemIdentifier("node1", ScType::ConstNode);
  ScAddr const & addr2 = m_ctx->ResolveElementSystemIdentifier("node2", ScType::ConstNode);
  ScAddr const & nonRoleAddr = m_ctx->ResolveElementSystemIdentifier("nonRole1", ScType::ConstNodeNonRole);

  ScAddr const & connectorAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, addr1, addr2);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nonRoleAddr, connectorAddr);

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"] = "@alias = (_node1 _=> _node2);; nonRole1 _-> @alias;;";
  std::string const payloadString = ScMemoryJsonConverter::From(0, "search_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"][0].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]) == addr1);
  EXPECT_TRUE(ScAddr(addrs[2]) == addr2);
  EXPECT_TRUE(ScAddr(addrs[3]) == nonRoleAddr);

  client.Stop();
}

TEST_F(ScServerTest, SearchTemplateByIdtf)
{
  LoadKB(m_ctx, {"templates.scs", "user.scs"});

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"]["type"] = "idtf";
  payload["templ"]["value"] = "test_template_1";
  std::string const payloadString = ScMemoryJsonConverter::From(0, "search_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"][0].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[1]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[2]).IsValid());

  client.Stop();
}

TEST_F(ScServerTest, SearchTemplateByAddr)
{
  LoadKB(m_ctx, {"templates.scs", "user.scs"});

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"]["type"] = "addr";
  payload["templ"]["value"] = m_ctx->SearchElementBySystemIdentifier("test_template_1").Hash();
  std::string const payloadString = ScMemoryJsonConverter::From(0, "search_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"][0].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[1]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[2]).IsValid());

  client.Stop();
}

TEST_F(ScServerTest, GenerateTemplate)
{
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & link = m_ctx->GenerateLink();
  ScAddr const & nonRoleAddr = m_ctx->GenerateNode(ScType::ConstNodeNonRole);

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"] = ScMemoryJsonPayload::array({
      {
          {
              {"type", "type"},
              {"value", sc_type_node | sc_type_var},
              {"alias", "_src"},
          },
          {
              {"type", "type"},
              {"value", sc_type_common_edge | sc_type_var},
              {"alias", "_edge1"},
          },
          {
              {"type", "type"},
              {"value", sc_type_node_link | sc_type_var},
              {"alias", "_trg"},

          },
      },
      {
          {
              {"type", "addr"},
              {"value", nonRoleAddr.Hash()},
          },
          {
              {"type", "type"},
              {"value", sc_type_var_perm_pos_arc},
              {"alias", "_edge2"},
          },
          {
              {"type", "alias"},
              {"value", "_edge1"},
          },
      },
  });
  payload["params"]["_src"] = addr.Hash();
  payload["params"]["_trg"] = link.Hash();
  std::string const payloadString = ScMemoryJsonConverter::From(0, "generate_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]) == addr);
  EXPECT_TRUE(ScAddr(addrs[1]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[2]) == link);
  EXPECT_TRUE(ScAddr(addrs[3]) == nonRoleAddr);

  client.Stop();
}

TEST_F(ScServerTest, GenerateStringTemplate)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"] = "@alias = (_node1 _=> _node2);;";
  std::string const payloadString = ScMemoryJsonConverter::From(0, "generate_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[1]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[2]).IsValid());

  client.Stop();
}

TEST_F(ScServerTest, GenerateTemplateByIdtf)
{
  LoadKB(m_ctx, {"templates.scs"});

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"]["type"] = "idtf";
  payload["templ"]["value"] = "test_template_1";
  std::string const payloadString = ScMemoryJsonConverter::From(0, "generate_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[1]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[2]).IsValid());

  client.Stop();
}

TEST_F(ScServerTest, GenerateTemplateByAddr)
{
  LoadKB(m_ctx, {"templates.scs"});

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"]["type"] = "addr";
  payload["templ"]["value"] = m_ctx->SearchElementBySystemIdentifier("test_template_1").Hash();
  std::string const payloadString = ScMemoryJsonConverter::From(0, "generate_template", payload);
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  auto const & addrs = responsePayload["addrs"].get<std::vector<size_t>>();
  EXPECT_FALSE(addrs.empty());
  EXPECT_TRUE(ScAddr(addrs[0]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[1]).IsValid());
  EXPECT_TRUE(ScAddr(addrs[2]).IsValid());

  client.Stop();
}

TEST_F(ScServerTest, HandleEvents)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & addr1 = m_ctx->GenerateNode(ScType::ConstNode);

  std::string payloadString = ScMemoryJsonConverter::From(
      0,
      "events",
      ScMemoryJsonPayload::object({{
          "create",
          ScMemoryJsonPayload::array({
              {
                  {"type", "add_outgoing_edge"},
                  {"addr", addr1.Hash()},
              },
          }),
      }}));
  EXPECT_TRUE(client.Send(payloadString));

  auto response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());
  EXPECT_FALSE(response["event"].get<sc_bool>());

  EXPECT_TRUE(responsePayload[0].get<sc_int>() == 0);

  ScAddr const & addr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & connectorAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addr1, addr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  EXPECT_TRUE(response["event"].get<sc_bool>());

  responsePayload = response["payload"];

  EXPECT_TRUE(responsePayload[0].get<uint64_t>() == addr1.Hash());
  EXPECT_TRUE(responsePayload[1].get<uint64_t>() == connectorAddr.Hash());
  EXPECT_TRUE(responsePayload[2].get<uint64_t>() == addr2.Hash());

  payloadString = ScMemoryJsonConverter::From(
      0,
      "events",
      ScMemoryJsonPayload::object(
          {{"delete",
            ScMemoryJsonPayload::array({
                0,
            })}}));
  EXPECT_TRUE(client.Send(payloadString));

  response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());

  responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload[0].get<sc_int>() == 0);
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  payloadString = ScMemoryJsonConverter::From(
      0,
      "events",
      ScMemoryJsonPayload::object(
          {{"delete",
            ScMemoryJsonPayload::array({
                0,
                1,
            })}}));
  EXPECT_TRUE(client.Send(payloadString));

  response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(responsePayload.is_array());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());
  EXPECT_FALSE(response["event"].get<sc_bool>());

  client.Stop();
}

TEST_F(ScServerTest, UnknownEvent)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & addr1 = m_ctx->GenerateNode(ScType::ConstNode);

  std::string payloadString = ScMemoryJsonConverter::From(
      0,
      "events",
      ScMemoryJsonPayload::object({{
          "create",
          ScMemoryJsonPayload::array({
              {
                  {"type", "unknown_event"},
                  {"addr", addr1.Hash()},
              },
          }),
      }}));
  EXPECT_TRUE(client.Send(payloadString));

  auto response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
  EXPECT_FALSE(response["event"].get<sc_bool>());

  client.Stop();
}

TEST_F(ScServerTest, Unknown)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = ScMemoryJsonConverter::From(0, "unknown", ScMemoryJsonPayload::object({}));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_TRUE(response["payload"].is_null());

  client.Stop();
}

TEST_F(ScServerTest, UnknownJson)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = "unknown";
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_TRUE(response.is_string());
  EXPECT_FALSE(response.get<std::string>().empty());

  client.Stop();
}
