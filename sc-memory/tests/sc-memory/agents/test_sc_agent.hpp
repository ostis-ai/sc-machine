#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"
#include "sc-memory/sc_timer.hpp"
#include "sc-memory/utils/sc_lock.hpp"
#include "sc-memory/sc_agent.hpp"

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

class ATestAddInputEdge : public ScAgent<ScSubscription{ScEvent::Type::AddInputEdge, ScKeynodes::action_initiated}, ScSubscription{ScEvent::Type::ChangeContent, ScKeynodes::action_initiated}>
{
public:
  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

class ATestAddOutputEdge : public ScAgent<ScSubscription{ScEvent::Type::AddOutputEdge, ScKeynodes::action_initiated}>
{
public:
  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

class ATestRemoveInputEdge : public ScAgent<ScSubscription{ScEvent::Type::RemoveInputEdge, ScKeynodes::action_initiated}>
{
public:
  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

class ATestRemoveOutputEdge : public ScAgent<ScSubscription{ScEvent::Type::RemoveOutputEdge, ScKeynodes::action_initiated}>
{
public:
  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

class ATestRemoveElement : public ScAgent<ScSubscription{ScEvent::Type::RemoveElement, ScKeynodes::action_initiated}>
{
public:
  static ScKeynodeClass const msAgentKeynode;
  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

class ATestContentChanged : public ScAgent<ScSubscription{ScEvent::Type::ChangeContent, ScKeynodes::action_initiated}>
{
public:
  static ScKeynodeLink const msAgentKeynode;
  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

class ATestAddMultipleOutputEdge : public ScAgent<ScSubscription{ScEvent::Type::AddOutputEdge, ScKeynodes::action_initiated}, ScSubscription{ScEvent::Type::AddOutputEdge, ScKeynodes::action_initiated}>
{
public:
  static ScKeynodeClass const msAgentKeynode1;
  static ScKeynodeClass const msAgentKeynode2;
  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

class ATestCheckResult : public ScActionAgent<>
{
public:
  void OnSuccess(ScAddr const & actionAddr) override;
  void OnUnsuccess(ScAddr const & actionAddr) override;
  void OnError(ScAddr const & actionAddr, sc_result errorCode) override;

  static ScKeynodeClass const msAgentKeynode;
  static ScKeynodeClass const msAgentSet;

  static TestWaiter msWaiter;

  sc_result OnEvent(ScAddr const & actionAddr) override;
};
