#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"
#include "sc-memory/sc_timer.hpp"
#include "sc-memory/utils/sc_lock.hpp"
#include "sc-memory/kpm/sc_agent.hpp"

#include <thread>

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
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return !m_lock.IsLocked();
  }

  void Unlock()
  {
    m_lock.Unlock();
  }

private:
  utils::ScLock m_lock;
};


class ATestAction : public ScAgentAction
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
