#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"
#include "sc-memory/sc_timer.hpp"
#include "sc-memory/utils/sc_lock.hpp"
#include "sc-memory/kpm/sc_agent.hpp"

#include <thread>

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

class ATestAddInputEdge : public ScAgent<ScEvent::Type::AddInputEdge>
{
public:
  SC_AGENT_BODY(ATestAddInputEdge);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestAddOutputEdge : public ScAgent<ScEvent::Type::AddOutputEdge>
{
public:
  SC_AGENT_BODY(ATestAddOutputEdge);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestRemoveInputEdge : public ScAgent<ScEvent::Type::RemoveInputEdge>
{
public:
  SC_AGENT_BODY(ATestRemoveInputEdge);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestRemoveOutputEdge : public ScAgent<ScEvent::Type::RemoveOutputEdge>
{
public:
  SC_AGENT_BODY(ATestRemoveOutputEdge);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestRemoveElement : public ScAgent<ScEvent::Type::RemoveElement>
{
public:
  SC_AGENT_BODY(ATestRemoveElement);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestContentChanged : public ScAgent<ScEvent::Type::ChangeContent>
{
public:
  SC_AGENT_BODY(ATestContentChanged);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeLink const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestAddMultipleOutputEdge : public ScAgent<ScEvent::Type::AddOutputEdge>
{
public:
  SC_AGENT_BODY(ATestAddMultipleOutputEdge);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeClass const msAgentKeynode1;
  static ScKeynodeClass const msAgentKeynode2;
  static TestWaiter msWaiter;
};

class ATestCheckResult : public ScAgent<ScEvent::Type::AddOutputEdge>
{
public:
  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  void OnSuccess(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
  void OnUnsuccess(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
  void OnError(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr, sc_result errorCode)
      override;

  static ScKeynodeClass const msAgentKeynode;
  static ScKeynodeClass const msAgentSet;
  static TestWaiter msWaiter;
};
