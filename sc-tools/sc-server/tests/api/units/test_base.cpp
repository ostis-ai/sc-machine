/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "gtest/gtest.h"

#include "sc-core/sc-store/sc_types.h"
#include "sc-memory/sc_type.hpp"
#include "sc_server_test.hpp"
#include "../../sc_client.hpp"

#include "../../sc_memory_json_converter.hpp"

TEST_F(ScServerTest, CreateElements)
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
              {"type", sc_type_link | sc_type_const},
              {"content", "edge_end"},
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
              {"type", sc_type_arc_pos_const_perm},
          },
          {
              {"el", "link"},
              {"type", sc_type_link | sc_type_const},
              {"content", 100},
          },
          {
              {"el", "link"},
              {"type", sc_type_link | sc_type_const},
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
  ScAddr const & edge = ScAddr(responsePayload[2].get<size_t>());
  EXPECT_TRUE(edge.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(edge).IsEdge());
  ScAddr const & trg = ScAddr(responsePayload[1].get<size_t>());
  EXPECT_TRUE(trg.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(trg).IsLink());

  auto const & links = m_ctx->FindLinksByContent("edge_end");
  EXPECT_TRUE(std::find(links.begin(), links.end(), trg) != links.end());

  ScIterator3Ptr const iter3 = m_ctx->Iterator3(src, sc_type_arc_pos_const_perm, trg);
  EXPECT_TRUE(iter3->Next());
  EXPECT_TRUE(iter3->Get(1) == edge);

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

TEST_F(ScServerTest, CreateEmptyElements)
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

TEST_F(ScServerTest, CreateElementsBySCs)
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

  ScAddr const & classSet = m_ctx->HelperFindBySystemIdtf("concept_set");
  EXPECT_TRUE(classSet.IsValid());
  ScAddr const & set1 = m_ctx->HelperFindBySystemIdtf("set1");
  EXPECT_TRUE(set1.IsValid());
  EXPECT_TRUE(m_ctx->HelperCheckEdge(classSet, set1, ScType::EdgeAccessConstPosPerm));
  EXPECT_TRUE(responsePayload[0].get<sc_bool>());

  EXPECT_FALSE(responsePayload[1].get<sc_bool>());

  client.Stop();
}

TEST_F(ScServerTest, CreateEmptyElementsBySCs)
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

TEST_F(ScServerTest, CreateElementsByWrongSCs)
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

  ScAddr const & src = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & trg = m_ctx->CreateLink();
  ScAddr const & edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, src, trg);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "check_elements",
      ScMemoryJsonPayload::array({
          src.Hash(),
          edge.Hash(),
          trg.Hash(),
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  ScType const & srcType = ScType(responsePayload[0].get<size_t>());
  EXPECT_TRUE(m_ctx->GetElementType(src) == srcType);
  ScType const & edgeType = ScType(responsePayload[1].get<size_t>());
  EXPECT_TRUE(m_ctx->GetElementType(edge) == edgeType);
  ScType const & trgType = ScType(responsePayload[2].get<size_t>());
  EXPECT_TRUE(m_ctx->GetElementType(trg) == trgType);

  ScIterator3Ptr const iter3 = m_ctx->Iterator3(src, ScType::EdgeDCommonConst, trg);
  EXPECT_TRUE(iter3->Next());

  client.Stop();
}

TEST_F(ScServerTest, DeleteElements)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & src = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & trg = m_ctx->CreateLink();
  ScAddr const & edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, src, trg);

  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "delete_elements",
      ScMemoryJsonPayload::array({
          src.Hash(),
          edge.Hash(),
          trg.Hash(),
      }));
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  ScIterator3Ptr const iter3 = m_ctx->Iterator3(src, ScType::EdgeAccessConstPosPerm, trg);
  EXPECT_FALSE(iter3->IsValid());

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
  EXPECT_TRUE(m_ctx->GetElementType(addr1) == ScType::NodeConstClass);
  ScAddr const & addr2 = ScAddr(responsePayload[1].get<size_t>());
  EXPECT_TRUE(addr2.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(addr2) == ScType::NodeConstClass);
  EXPECT_TRUE(addr1 == addr2);
  EXPECT_TRUE("any_system_identifier" == m_ctx->HelperGetSystemIdtf(addr1));

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

  ScAddr const & link = m_ctx->CreateLink();

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

TEST_F(ScServerTest, HandleContentOld)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScAddr const & link = m_ctx->CreateLink();

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

  ScAddr const & link = m_ctx->CreateLink();

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

  ScAddr const & link = m_ctx->CreateLink();

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

TEST_F(ScServerTest, SearchTemplate)
{
  ScAddr const & addr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & link = m_ctx->CreateLink();
  ScAddr const & noroleAddr = m_ctx->CreateNode(ScType::NodeConstNoRole);

  ScAddr const & edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, addr, link);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, noroleAddr, edge);

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
              {"value", sc_type_edge_common | sc_type_var},
              {"alias", "_edge1"},
          },
          {
              {"type", "type"},
              {"value", sc_type_link | sc_type_var},
              {"alias", "_trg"},

          },
      },
      {
          {
              {"type", "addr"},
              {"value", noroleAddr.Hash()},
          },
          {
              {"type", "type"},
              {"value", sc_type_arc_pos_var_perm},
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
  EXPECT_TRUE(ScAddr(addrs[3]) == noroleAddr);

  client.Stop();
}

TEST_F(ScServerTest, SearchStringTemplate)
{
  ScAddr const & addr1 = m_ctx->HelperResolveSystemIdtf("node1", ScType::NodeConst);
  ScAddr const & addr2 = m_ctx->HelperResolveSystemIdtf("node2", ScType::NodeConst);
  ScAddr const & noroleAddr = m_ctx->HelperResolveSystemIdtf("norole1", ScType::NodeConstNoRole);

  ScAddr const & edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, addr1, addr2);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, noroleAddr, edge);

  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"] = "@alias = (_node1 _=> _node2);; norole1 _-> @alias;;";
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
  EXPECT_TRUE(ScAddr(addrs[3]) == noroleAddr);

  client.Stop();
}

TEST_F(ScServerTest, SearchTemplateByIdtf)
{
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
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"]["type"] = "addr";
  payload["templ"]["value"] = m_ctx->HelperFindBySystemIdtf("test_template_1").Hash();
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
  ScAddr const & addr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & link = m_ctx->CreateLink();
  ScAddr const & noroleAddr = m_ctx->CreateNode(ScType::NodeConstNoRole);

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
              {"value", sc_type_edge_common | sc_type_var},
              {"alias", "_edge1"},
          },
          {
              {"type", "type"},
              {"value", sc_type_link | sc_type_var},
              {"alias", "_trg"},

          },
      },
      {
          {
              {"type", "addr"},
              {"value", noroleAddr.Hash()},
          },
          {
              {"type", "type"},
              {"value", sc_type_arc_pos_var_perm},
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
  EXPECT_TRUE(ScAddr(addrs[3]) == noroleAddr);

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
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  ScMemoryJsonPayload payload;
  payload["templ"]["type"] = "addr";
  payload["templ"]["value"] = m_ctx->HelperFindBySystemIdtf("test_template_1").Hash();
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

  ScAddr const & addr1 = m_ctx->CreateNode(ScType::NodeConst);

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

  ScAddr const & addr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  EXPECT_TRUE(response["event"].get<sc_bool>());

  responsePayload = response["payload"];

  EXPECT_TRUE(responsePayload[0].get<uint64_t>() == addr1.Hash());
  EXPECT_TRUE(responsePayload[1].get<uint64_t>() == edge.Hash());
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
