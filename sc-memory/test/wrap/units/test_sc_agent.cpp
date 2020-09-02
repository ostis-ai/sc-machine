/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "catch2/catch.hpp"

#include "test_sc_agent.hpp"

#include "sc-memory/cpp/sc_stream.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

namespace
{

ScAddr CreateKeynode(ScMemoryContext & ctx, std::string const & name)
{
  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node.IsValid());
  REQUIRE(ctx.HelperSetSystemIdtf(name, node));

  return node;
}

} // namespace


/// --------------------------------------
TestWaiter ATestCommand::msWaiter;

SC_AGENT_ACTION_IMPLEMENTATION(ATestCommand)
{
  msWaiter.Unlock();
  return SC_RESULT_ERROR;
}

TEST_CASE("ATestCommand", "[test sc agent]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestCommand");

  ScAddr const command_1 = CreateKeynode(ctx, "command_1");

  ScAgentInit(true);
  ATestCommand::InitGlobal();

  SC_AGENT_REGISTER(ATestCommand);

  ScAddr const cmd = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(cmd.IsValid());
  ScAddr const e1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, command_1, cmd);
  REQUIRE(e1.IsValid());
  ScAddr const e2 = ctx.CreateEdge(ScType::EdgeAccessConstPosTemp, ScAgentAction::GetCommandInitiatedAddr(), cmd);
  REQUIRE(e2.IsValid());

  REQUIRE(ATestCommand::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestCommand);

  ctx.Destroy();
}


/// --------------------------------------
ScAddr ATestAddInputEdge::msAgentKeynode;
TestWaiter ATestAddInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

TEST_CASE("ATestAddInputEdge", "[test sc agent]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestAddInputEdge");

  ScAgentInit(true);
  ATestAddInputEdge::InitGlobal();
  SC_AGENT_REGISTER(ATestAddInputEdge);

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node.IsValid());
  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, ATestAddInputEdge::msAgentKeynode);
  REQUIRE(e.IsValid());
  REQUIRE(ATestAddInputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestAddInputEdge);

  ctx.Destroy();
}


/// --------------------------------------
ScAddr ATestAddOutputEdge::msAgentKeynode;
TestWaiter ATestAddOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

TEST_CASE("ATestAddOutputEdge", "[test sc agent]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestAddOutputEdge");

  ScAgentInit(true);
  ATestAddOutputEdge::InitGlobal();
  SC_AGENT_REGISTER(ATestAddOutputEdge);

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node.IsValid());

  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddOutputEdge::msAgentKeynode, node);
  REQUIRE(e.IsValid());

  REQUIRE(ATestAddOutputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestAddOutputEdge);

  ctx.Destroy();
}


/// --------------------------------------
ScAddr ATestRemoveInputEdge::msAgentKeynode;
TestWaiter ATestRemoveInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

TEST_CASE("ATestRemoveInputEdge", "[test sc agent]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestRemoveInputEdge");

  ScAgentInit(true);
  ATestRemoveInputEdge::InitGlobal();

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node.IsValid());
  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccess, node, ATestRemoveInputEdge::msAgentKeynode);
  REQUIRE(e.IsValid());

  SC_AGENT_REGISTER(ATestRemoveInputEdge);

  REQUIRE(ctx.EraseElement(e));
  REQUIRE(ATestRemoveInputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestRemoveInputEdge);

  ctx.Destroy();
}


/// --------------------------------------
ScAddr ATestRemoveOutputEdge::msAgentKeynode;
TestWaiter ATestRemoveOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

TEST_CASE("ATestRemoveOutputEdge", "[test sc agent]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestRemoveOutputEdge");

  ScAgentInit(true);
  ATestRemoveOutputEdge::InitGlobal();

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  REQUIRE(node.IsValid());
  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccess, ATestRemoveOutputEdge::msAgentKeynode, node);
  REQUIRE(e.IsValid());

  SC_AGENT_REGISTER(ATestRemoveOutputEdge);

  REQUIRE(ctx.EraseElement(e));
  REQUIRE(ATestRemoveOutputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestRemoveOutputEdge);

  ctx.Destroy();
}


/// --------------------------------------
ScAddr ATestRemoveElement::msAgentKeynode;
TestWaiter ATestRemoveElement::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveElement)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

TEST_CASE("ATestRemoveElement", "[test sc agent]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestRemoveElement");

  ScAgentInit(true);
  ATestRemoveElement::InitGlobal();

  SC_AGENT_REGISTER(ATestRemoveElement);

  REQUIRE(ctx.EraseElement(ATestRemoveElement::msAgentKeynode));
  REQUIRE(ATestRemoveElement::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestRemoveElement);

  ctx.Destroy();
}


/// --------------------------------------
ScAddr ATestContentChanged::msAgentKeynode;
TestWaiter ATestContentChanged::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestContentChanged)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

TEST_CASE("ATestContentChanged", "[test sc agent]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestContentChanged");

  ScAddr const link = ctx.CreateLink();
  REQUIRE(link.IsValid());
  REQUIRE(ctx.HelperSetSystemIdtf("ATestContentChanged", link));

  ScAgentInit(true);
  ATestContentChanged::InitGlobal();

  SC_AGENT_REGISTER(ATestContentChanged);

  uint32_t const value = 100;
  ScStream stream((char *) &value, sizeof(value), SC_STREAM_FLAG_READ);

  REQUIRE(ctx.SetLinkContent(link, stream));
  REQUIRE(ATestContentChanged::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestContentChanged);

  ctx.Destroy();
}
