#include <gtest/gtest.h>

#include "sc-core/sc-store/sc_types.h"
#include "sc-memory/sc_type.hpp"
#include "sc_server_test.hpp"
#include "../sc_client.hpp"

#include "../sc_memory_json_converter.hpp"

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
      }));
  WAIT_SERVER;
  EXPECT_TRUE(client.Send(payloadString));

  WAIT_SERVER;
  auto const response = client.GetResponsePayload();
  EXPECT_FALSE(response.is_null());

  ScAddr const & src = ScAddr(response[0].get<size_t>());
  EXPECT_TRUE(src.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(src).IsNode());
  ScAddr const & edge = ScAddr(response[2].get<size_t>());
  EXPECT_TRUE(edge.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(edge).IsEdge());
  ScAddr const & trg = ScAddr(response[1].get<size_t>());
  EXPECT_TRUE(trg.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(trg).IsLink());

  auto const & links = m_ctx->FindLinksByContent("edge_end");
  EXPECT_TRUE(std::find(links.begin(), links.end(), trg) != links.end());

  ScIterator3Ptr const iter3 = m_ctx->Iterator3(src, sc_type_arc_pos_const_perm, trg);
  EXPECT_TRUE(iter3->Next());
  EXPECT_TRUE(iter3->Get(1) == edge);

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
  WAIT_SERVER;
  EXPECT_TRUE(client.Send(payloadString));

  WAIT_SERVER;
  auto const response = client.GetResponsePayload();
  EXPECT_FALSE(response.is_null());

  ScType const & srcType = ScType(response[0].get<size_t>());
  EXPECT_TRUE(m_ctx->GetElementType(src) == srcType);
  ScType const & edgeType = ScType(response[1].get<size_t>());
  EXPECT_TRUE(m_ctx->GetElementType(edge) == edgeType);
  ScType const & trgType = ScType(response[2].get<size_t>());
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
  WAIT_SERVER;
  EXPECT_TRUE(client.Send(payloadString));

  WAIT_SERVER;
  auto const response = client.GetResponsePayload();
  EXPECT_FALSE(response.is_null());

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
  WAIT_SERVER;
  EXPECT_TRUE(client.Send(payloadString));

  WAIT_SERVER;
  WAIT_SERVER;
  WAIT_SERVER;
  auto const response = client.GetResponsePayload();
  EXPECT_FALSE(response.is_null());

  ScAddr const & addr1 = ScAddr(response[0].get<size_t>());
  EXPECT_TRUE(addr1.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(addr1) == ScType::NodeConstClass);
  ScAddr const & addr2 = ScAddr(response[1].get<size_t>());
  EXPECT_TRUE(addr2.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(addr2) == ScType::NodeConstClass);
  EXPECT_TRUE(addr1 == addr2);
  EXPECT_TRUE("any_system_identifier" == m_ctx->HelperGetSystemIdtf(addr1));

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
      }));
  WAIT_SERVER;
  EXPECT_TRUE(client.Send(payloadString));

  WAIT_SERVER;
  WAIT_SERVER;
  auto const response = client.GetResponsePayload();
  EXPECT_FALSE(response.is_null());

  EXPECT_TRUE(response[0].get<sc_bool>());
  EXPECT_TRUE(response[1]["value"].get<std::string>() == "some content");
  auto const & links = response[2].get<std::vector<size_t>>();
  EXPECT_FALSE(links.empty());
  EXPECT_TRUE(std::find(links.begin(), links.end(), link.Hash()) != links.end());

  client.Stop();
}
