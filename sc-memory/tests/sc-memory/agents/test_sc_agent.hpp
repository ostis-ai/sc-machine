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

class ATestAddInputEdge : public ScSubject<ScEvent::Type::AddInputEdge>
{
public:
  SC_SUBJECT_BODY(ATestAddInputEdge);

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestAddOutputEdge : public ScSubject<ScEvent::Type::AddOutputEdge>
{
public:
  SC_SUBJECT_BODY(ATestAddOutputEdge);

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestRemoveInputEdge : public ScSubject<ScEvent::Type::RemoveInputEdge>
{
public:
  SC_SUBJECT_BODY(ATestRemoveInputEdge);

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestRemoveOutputEdge : public ScSubject<ScEvent::Type::RemoveOutputEdge>
{
public:
  SC_SUBJECT_BODY(ATestRemoveOutputEdge);

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestRemoveElement : public ScSubject<ScEvent::Type::RemoveElement>
{
public:
  SC_SUBJECT_BODY(ATestRemoveElement);

  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestContentChanged : public ScSubject<ScEvent::Type::ChangeContent>
{
public:
  SC_SUBJECT_BODY(ATestContentChanged);

  static ScKeynodeLink const msAgentKeynode;
  static TestWaiter msWaiter;
};

class ATestAddMultipleOutputEdge : public ScSubject<ScEvent::Type::AddOutputEdge>
{
public:
  SC_SUBJECT_BODY(ATestAddMultipleOutputEdge);

  static ScKeynodeClass const msAgentKeynode1;
  static ScKeynodeClass const msAgentKeynode2;
  static TestWaiter msWaiter;
};

class ATestCheckResult : public ScAgent<>
{
public:
  SC_AGENT_BODY(ATestCheckResult);

  void OnSuccess(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
  void OnUnsuccess(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
  void OnError(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr, sc_result errorCode)
      override;

  static ScKeynodeClass const msAgentKeynode;
  static ScKeynodeClass const msAgentSet;
  static TestWaiter msWaiter;
};
