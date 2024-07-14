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

class ATestAddInputEdge : public ScAgent<ScEventAddInputEdge>
{
public:
  static inline ScKeynode const add_input_edge_action{"add_input_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventAddInputEdge const & event) override;
};

class ATestAddOutputEdge : public ScAgent<ScEventAddOutputEdge>
{
public:
  static inline ScKeynode const add_output_edge_action{"add_output_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventAddOutputEdge const & event) override;
};

class ATestRemoveInputEdge : public ScAgent<ScEventRemoveInputEdge>
{
public:
  static inline ScKeynode const remove_input_edge_action{"remove_input_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventRemoveInputEdge const & event) override;
};

class ATestRemoveOutputEdge : public ScAgent<ScEventRemoveOutputEdge>
{
public:
  static inline ScKeynode const remove_output_edge_action{"remove_output_edge_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventRemoveOutputEdge const & event) override;
};

class ATestRemoveElement : public ScAgent<ScEventRemoveElement>
{
public:
  static inline ScKeynode const remove_element_action{"remove_element_action", ScType::NodeConstClass};
  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventRemoveElement const & event) override;
};

class ATestContentChanged : public ScAgent<ScEventChangeContent>
{
public:
  static inline ScKeynode const content_change_action{"content_change_action", ScType::LinkConst};
  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventChangeContent const & event) override;
};

class ATestAddMultipleOutputEdge : public ScAgent<ScEventAddOutputEdge>
{
public:
  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventAddOutputEdge const &) override;
};

class ATestCheckResult : public ScActionAgent
{
public:
  void OnSuccess(ScEventAddOutputEdge const & event) override;
  void OnUnsuccess(ScEventAddOutputEdge const & event) override;
  void OnError(ScEventAddOutputEdge const & event, sc_result errorCode) override;

  static inline ScKeynode const msAgentSet{"agents_set", ScType::NodeConstClass};

  static inline TestWaiter msWaiter;

  sc_result OnEvent(ScEventAddOutputEdge const & event) override;
};
