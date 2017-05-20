/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/sc_timer.hpp"
#include "sc-memory/cpp/utils/sc_lock.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

extern "C"
{
#include "glib.h"
}

#include "test_sc_agent.generated.hpp"

class TestWaiter
{
public:
  TestWaiter()
  {
    m_lock.Lock();
  }

  bool Wait()
  {
    ScTimer timer(5.0);
    while (!timer.IsTimeOut() && m_lock.IsLocked())
      g_usleep(10000); // 10 milliseconds

    return !m_lock.IsLocked();
  }

  void Unlock()
  {
    m_lock.Unlock();
  }

private:
  utils::ScLock m_lock;
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
  SC_CLASS(Agent, Event(msAgentKeynode, ScEvent::Type::AddInputEdge))
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("ATestAddInputEdge"), ForceCreate)
  static ScAddr msAgentKeynode;

  static TestWaiter msWaiter;
};


class ATestAddOutputEdge : public ScAgent
{
  SC_CLASS(Agent, Event(msAgentKeynode, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

public:

  SC_PROPERTY(Keynode("ATestAddOutputEdge"), ForceCreate)
  static ScAddr msAgentKeynode;

  static TestWaiter msWaiter;
};


class ATestRemoveInputEdge : public ScAgent
{
  SC_CLASS(Agent, Event(msAgentKeynode, ScEvent::Type::RemoveInputEdge))
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("ATestRemoveInputEdge"), ForceCreate)
  static ScAddr msAgentKeynode;

  static TestWaiter msWaiter;
};


class ATestRemoveOutputEdge : public ScAgent
{
  SC_CLASS(Agent, Event(msAgentKeynode, ScEvent::Type::RemoveOutputEdge))
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("ATestRemoveOutputEdge"), ForceCreate)
  static ScAddr msAgentKeynode;

  static TestWaiter msWaiter;
};


class ATestRemoveElement : public ScAgent
{
  SC_CLASS(Agent, Event(msAgentKeynode, ScEvent::Type::EraseElement))
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("ATestRemoveElement"), ForceCreate)
  static ScAddr msAgentKeynode;

  static TestWaiter msWaiter;
};


class ATestContentChanged : public ScAgent
{
  SC_CLASS(Agent, Event(msAgentKeynode, ScEvent::Type::ContentChanged))
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("ATestContentChanged"), ForceCreate)
  static ScAddr msAgentKeynode;

  static TestWaiter msWaiter;
};
