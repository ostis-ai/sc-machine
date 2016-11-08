/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "../test.hpp"
#include "test_sc_agent.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

namespace
{
    ScAddr CreateKeynode(ScMemoryContext & ctx, std::string const & name)
    {
        ScAddr const node = ctx.createNode(ScType::NODE_CONST);
        SC_CHECK(node.isValid(), ());
        SC_CHECK(ctx.helperSetSystemIdtf(name, node), ());

        return node;
    }
}


/// --------------------------------------
TestWaiter ATestCommand::msWaiter;

SC_AGENT_ACTION_IMPLEMENTATION(ATestCommand)
{
    msWaiter.Unlock();
	return SC_RESULT_ERROR;
}

UNIT_TEST(ATestCommand)
{
    ScMemoryContext ctx;

    // create used keynodes
    ScAddr const cmd_init = CreateKeynode(ctx, "command_initiated");
    ScAddr const cmd_prog = CreateKeynode(ctx, "command_in_progress");
    ScAddr const cmd_finish = CreateKeynode(ctx, "command_finished");
    ScAddr const cmd_result = CreateKeynode(ctx, "nrel_result");
    ScAddr const command_1 = CreateKeynode(ctx, "command_1");

    ScAgentInit();
    ATestCommand::initGlobal();

    SC_AGENT_REGISTER(ATestCommand);

    ScAddr const cmd = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(cmd.isValid(), ());
    ScAddr const e1 = ctx.createEdge(ScType::EDGE_ACCESS_CONST_POS_PERM, command_1, cmd);
    SC_CHECK(e1.isValid(), ());
    ScAddr const e2 = ctx.createEdge(ScType::EDGE_ACCESS_CONST_POS_PERM, cmd_init, cmd);
    SC_CHECK(e2.isValid(), ());

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
    ScMemoryContext ctx;
    ATestAddInputEdge::initGlobal();
    SC_AGENT_REGISTER(ATestAddInputEdge);

    ScAddr const node = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(node.isValid(), ());
    ScAddr const e = ctx.createEdge(ScType::EDGE_ACCESS_CONST_POS_PERM, node, ATestAddInputEdge::msAgentKeynode);
    SC_CHECK(e.isValid(), ());
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
    ScMemoryContext ctx;
    ATestAddOutputEdge::initGlobal();
    SC_AGENT_REGISTER(ATestAddOutputEdge);

    ScAddr const node = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(node.isValid(), ());

    ScAddr const e = ctx.createEdge(ScType::EDGE_ACCESS_CONST_POS_PERM, ATestAddOutputEdge::msAgentKeynode, node);
    SC_CHECK(e.isValid(), ());
        
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
    ScMemoryContext ctx;
    ATestRemoveInputEdge::initGlobal();
    
    ScAddr const node = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(node.isValid(), ());
    ScAddr const e = ctx.createEdge(ScType::EDGE_ACCESS, node, ATestRemoveInputEdge::msAgentKeynode);
    SC_CHECK(e.isValid(), ());

    SC_AGENT_REGISTER(ATestRemoveInputEdge);

    SC_CHECK(ctx.eraseElement(e), ());
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
    ScMemoryContext ctx;
    ATestRemoveOutputEdge::initGlobal();

    ScAddr const node = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(node.isValid(), ());
    ScAddr const e = ctx.createEdge(ScType::EDGE_ACCESS, ATestRemoveOutputEdge::msAgentKeynode, node);
    SC_CHECK(e.isValid(), ());

    SC_AGENT_REGISTER(ATestRemoveOutputEdge);

    SC_CHECK(ctx.eraseElement(e), ());
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
    ScMemoryContext ctx;
    ATestRemoveElement::initGlobal();

    SC_AGENT_REGISTER(ATestRemoveElement);

    SC_CHECK(ctx.eraseElement(ATestRemoveElement::msAgentKeynode), ());
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
    ScMemoryContext ctx;

    ScAddr const link = ctx.createLink();
    SC_CHECK(link.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("ATestContentChanged", link), ());

    ATestContentChanged::initGlobal();

    SC_AGENT_REGISTER(ATestContentChanged);

    uint32_t const value = 100;
    ScStream stream((char*)&value, sizeof(value), SC_STREAM_FLAG_READ);

    SC_CHECK(ctx.setLinkContent(link, stream), ());
    SC_CHECK(ATestContentChanged::msWaiter.Wait(), ());

    SC_AGENT_UNREGISTER(ATestContentChanged);
}