/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include <atomic>
#include <ctime>

extern "C"
{
#include "glib.h"
}

#include "test_sc_agent.generated.hpp"

class TestWaiter
{
public:
    TestWaiter()
        : mLock(1)
    {
    }

    bool Wait()
    {
        std::time_t const t = std::time(nullptr);
        std::time_t nt = t;
        while (mLock.load() > 0)
        {
            nt = std::time(nullptr);
            if (nt - t > 5) // seconds
                break;
            g_usleep(100000); // 100 milliseconds
        };

        return (mLock.load() == 0);
    }

    void Unlock()
    {
        mLock.store(0);
    }

private:
    std::atomic<uint32_t> mLock;
};


class ATestCommand : public ScAgentAction
{
	SC_CLASS(Agent, CmdClass("command_1"))
	SC_GENERATED_BODY()

public:
    static TestWaiter msWaiter;
};


class ATestAddInputEdge : public ScAgent
{
    SC_CLASS(Agent, Event(msAgentKeynode, SC_EVENT_ADD_INPUT_ARC))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("ATestAddInputEdge"), ForceCreate)
    static ScAddr msAgentKeynode;

    static TestWaiter msWaiter;
};


class ATestAddOutputEdge : public ScAgent
{
    SC_CLASS(Agent, Event(msAgentKeynode, SC_EVENT_ADD_OUTPUT_ARC))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("ATestAddOutputEdge"), ForceCreate)
    static ScAddr msAgentKeynode;

    static TestWaiter msWaiter;
};


class ATestRemoveInputEdge : public ScAgent
{
    SC_CLASS(Agent, Event(msAgentKeynode, SC_EVENT_REMOVE_INPUT_ARC))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("ATestRemoveInputEdge"), ForceCreate)
    static ScAddr msAgentKeynode;

    static TestWaiter msWaiter;
};


class ATestRemoveOutputEdge : public ScAgent
{
    SC_CLASS(Agent, Event(msAgentKeynode, SC_EVENT_REMOVE_OUTPUT_ARC))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("ATestRemoveOutputEdge"), ForceCreate)
    static ScAddr msAgentKeynode;

    static TestWaiter msWaiter;
};


class ATestRemoveElement : public ScAgent
{
    SC_CLASS(Agent, Event(msAgentKeynode, SC_EVENT_REMOVE_ELEMENT))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("ATestRemoveElement"), ForceCreate)
    static ScAddr msAgentKeynode;

    static TestWaiter msWaiter;
};


class ATestContentChanged : public ScAgent
{
    SC_CLASS(Agent, Event(msAgentKeynode, SC_EVENT_CONTENT_CHANGED))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("ATestContentChanged"), ForceCreate)
    static ScAddr msAgentKeynode;

    static TestWaiter msWaiter;
};
