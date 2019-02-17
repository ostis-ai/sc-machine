/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "test_sc_agent.hpp"

#include "sc-memory/sc_stream.hpp"
#include "sc-memory/utils/sc_test.hpp"

namespace
{

ScAddr CreateKeynode(ScMemoryContext & ctx, std::string const & name)
{
  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(node.IsValid(), ());
  SC_CHECK(ctx.HelperSetSystemIdtf(name, node), ());

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

UNIT_TEST(ATestCommand)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestCommand");

  ScAddr const command_1 = CreateKeynode(ctx, "command_1");

  ScAgentInit(true);
  ATestCommand::InitGlobal();

  SC_AGENT_REGISTER(ATestCommand);

  ScAddr const cmd = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(cmd.IsValid(), ());
  ScAddr const e1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, command_1, cmd);
  SC_CHECK(e1.IsValid(), ());
  ScAddr const e2 = ctx.CreateEdge(ScType::EdgeAccessConstPosTemp, ScAgentAction::GetCommandInitiatedAddr(), cmd);
  SC_CHECK(e2.IsValid(), ());

  SC_CHECK(ATestCommand::msWaiter.Wait(), ());

  SC_AGENT_UNREGISTER(ATestCommand);
}


/// --------------------------------------
ScAddr ATestAddInputEdge::msAgentKeynode;
TestWaiter ATestAddInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

UNIT_TEST(ATestAddInputEdge)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestAddInputEdge");

  ScAgentInit(true);
  ATestAddInputEdge::InitGlobal();
  SC_AGENT_REGISTER(ATestAddInputEdge);

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(node.IsValid(), ());
  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, ATestAddInputEdge::msAgentKeynode);
  SC_CHECK(e.IsValid(), ());
  SC_CHECK(ATestAddInputEdge::msWaiter.Wait(), ());

  SC_AGENT_UNREGISTER(ATestAddInputEdge);
}


/// --------------------------------------
ScAddr ATestAddOutputEdge::msAgentKeynode;
TestWaiter ATestAddOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

UNIT_TEST(ATestAddOutputEdge)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestAddOutputEdge");

  ScAgentInit(true);
  ATestAddOutputEdge::InitGlobal();
  SC_AGENT_REGISTER(ATestAddOutputEdge);

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(node.IsValid(), ());

  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddOutputEdge::msAgentKeynode, node);
  SC_CHECK(e.IsValid(), ());

  SC_CHECK(ATestAddOutputEdge::msWaiter.Wait(), ());

  SC_AGENT_UNREGISTER(ATestAddOutputEdge);
}


/// --------------------------------------
ScAddr ATestRemoveInputEdge::msAgentKeynode;
TestWaiter ATestRemoveInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

UNIT_TEST(ATestRemoveInputEdge)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestRemoveInputEdge");

  ScAgentInit(true);
  ATestRemoveInputEdge::InitGlobal();

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(node.IsValid(), ());
  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccess, node, ATestRemoveInputEdge::msAgentKeynode);
  SC_CHECK(e.IsValid(), ());

  SC_AGENT_REGISTER(ATestRemoveInputEdge);

  SC_CHECK(ctx.EraseElement(e), ());
  SC_CHECK(ATestRemoveInputEdge::msWaiter.Wait(), ());

  SC_AGENT_UNREGISTER(ATestRemoveInputEdge);
}


/// --------------------------------------
ScAddr ATestRemoveOutputEdge::msAgentKeynode;
TestWaiter ATestRemoveOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

UNIT_TEST(ATestRemoveOutputEdge)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestRemoveOutputEdge");

  ScAgentInit(true);
  ATestRemoveOutputEdge::InitGlobal();

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(node.IsValid(), ());
  ScAddr const e = ctx.CreateEdge(ScType::EdgeAccess, ATestRemoveOutputEdge::msAgentKeynode, node);
  SC_CHECK(e.IsValid(), ());

  SC_AGENT_REGISTER(ATestRemoveOutputEdge);

  SC_CHECK(ctx.EraseElement(e), ());
  SC_CHECK(ATestRemoveOutputEdge::msWaiter.Wait(), ());

  SC_AGENT_UNREGISTER(ATestRemoveOutputEdge);
}


/// --------------------------------------
ScAddr ATestRemoveElement::msAgentKeynode;
TestWaiter ATestRemoveElement::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveElement)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

UNIT_TEST(ATestRemoveElement)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestRemoveElement");

  ScAgentInit(true);
  ATestRemoveElement::InitGlobal();

  SC_AGENT_REGISTER(ATestRemoveElement);

  SC_CHECK(ctx.EraseElement(ATestRemoveElement::msAgentKeynode), ());
  SC_CHECK(ATestRemoveElement::msWaiter.Wait(), ());

  SC_AGENT_UNREGISTER(ATestRemoveElement);
}


/// --------------------------------------
ScAddr ATestContentChanged::msAgentKeynode;
TestWaiter ATestContentChanged::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestContentChanged)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

UNIT_TEST(ATestContentChanged)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "ATestContentChanged");

  ScAddr const link = ctx.CreateLink();
  SC_CHECK(link.IsValid(), ());
  SC_CHECK(ctx.HelperSetSystemIdtf("ATestContentChanged", link), ());

  ScAgentInit(true);
  ATestContentChanged::InitGlobal();

  SC_AGENT_REGISTER(ATestContentChanged);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);

  SC_CHECK(ctx.SetLinkContent(link, stream), ());
  SC_CHECK(ATestContentChanged::msWaiter.Wait(), ());

  SC_AGENT_UNREGISTER(ATestContentChanged);
}
