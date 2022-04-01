/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../sc_object.hpp"
#include "../sc_memory.hpp"
#include "../sc_keynodes.hpp"

#include "../utils/sc_log.hpp"

#include "../generated/sc_agent.generated.hpp"

/* Call this function before agent module usage.
 * If module initialized, then returns true; otherwise returns false
 */
_SC_EXTERN bool ScAgentInit(bool force = false);

class ScAgent : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  _SC_EXTERN explicit ScAgent(char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max);
  _SC_EXTERN virtual ~ScAgent();

protected:
  virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) = 0;

protected:
  mutable ScMemoryContext m_memoryCtx;
};

class ScAgentAction : public ScAgent
{
  friend class ScAgentRunner;

  SC_CLASS()
  SC_GENERATED_BODY()

public:
  enum class State : uint8_t
  {
    Unknown = 0,
    Initiated,
    Progress,
    Finished
  };

  _SC_EXTERN explicit ScAgentAction(
      ScAddr const & cmdClassAddr,
      char const * name,
      sc_uint8 accessLvl = sc_access_lvl_make_max);
  _SC_EXTERN virtual ~ScAgentAction();

protected:
  _SC_EXTERN virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
      override;

  _SC_EXTERN ScAddr GetParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, ScType const & paramType) const;
  /* This funtion returns parameter by index. It's equal to call
   * GetParam(cmdAddr, realtionAddr, ScType()), where realtionAddr is
   * an ScAddr of 'rrel_<index + 1>' relation
   */
  _SC_EXTERN ScAddr GetParam(ScAddr const & cmdAddr, size_t index) const;

  _SC_EXTERN virtual sc_result RunImpl(ScAddr const & requestAddr, ScAddr const & resultAddr) = 0;

public:
  static _SC_EXTERN ScAddr const & GetCommandInitiatedAddr();
  static _SC_EXTERN ScAddr const & GetCommandFinishedAddr();
  static _SC_EXTERN ScAddr const & GetNrelResultAddr();

  /* Create instance of specified command with arguments
   * cmdClassAddr - node that designate command class
   * For each parameter from params would be added attribute rrel_<i + 1>, where
   * i is an index of parameter in vector. Maximum size of params is 10.
   * Returns ScAddr of created command instance. If addr is not valid, then
   * command doesn't created
   */
  static _SC_EXTERN ScAddr
  CreateCommand(ScMemoryContext & ctx, ScAddr const & cmdClassAddr, ScAddrVector const & params);

  /* Init specified command.
   * cmdAddr - ScAddr of command instance
   * Returns true, when command initiated; otherwise returns false
   */
  static _SC_EXTERN bool InitiateCommand(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Initiate command and wait until it would be finished
   * cmdAddr - ScAddr of command instance to initiate
   * waitTimeOutMS - timeout for a waiting (milliseconds)
   * Returns true, if command initiated and finished; otherwise returns false (wait timeout)
   */
  static _SC_EXTERN bool InitiateCommandWait(
      ScMemoryContext & ctx,
      ScAddr const & cmdAddr,
      uint32_t waitTimeOutMS = 5000);

  /* Returns ScAddr of result structure of specified command
   */
  static _SC_EXTERN ScAddr GetCommandResultAddr(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Returns result code of specified command
   * If command still haven't result (possible not finished), then return
   * SC_RESULT_UNKNOWN. Be attentive, SC_RESULT_UNKNOWN is not a guarantee, that
   * process isn't finished. It could be finished with that result.
   */
  static _SC_EXTERN sc_result GetCommandResultCode(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Same as GetCommandResultCode, but returns ScAddr of result code set.
   * If it can't be determined, then return empty value
   */
  static _SC_EXTERN ScAddr GetCommandResultCodeAddr(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Returns state of specified command
   */
  static _SC_EXTERN State GetCommandState(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Reutrns true, if specified command included into set of finished commands
   */
  static _SC_EXTERN bool IsCommandFishined(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Reutrns true, if specified command included into set of initiated commands
   */
  static _SC_EXTERN bool IsCommandInitiated(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Reutrns true, if specified command included into set of in-progress commands
   */
  static _SC_EXTERN bool IsCommandInProgress(ScMemoryContext & ctx, ScAddr const & cmdAddr);

private:
  ScAddr m_cmdClassAddr;

private:
};

#define AGENT_NAME_TYPE(__Name__) __Name__##Type
#define AGENT_NAME_CLASS(__Name__) __Name__##_Agent
#define AGENT_NAME_INST(__Name__) __Name__ /*##(__LINE__)##(__FILE__)*/

#define SC_AGENT_IMPLEMENTATION(__AgentName__) \
  SC_COMBINE(ScFileID, _, __AgentName__, _impl) \
  sc_result __AgentName__::Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)

#define SC_AGENT_ACTION_IMPLEMENTATION(__AgentName__) \
  SC_COMBINE(ScFileID, _, __AgentName__, _impl) \
  sc_result __AgentName__::RunImpl(ScAddr const & requestAddr, ScAddr const & resultAddr)

#define SC_AGENT_REGISTER(__AgentName__) \
  SC_OBJECT_INIT_GLOBAL_CALL(__AgentName__) \
  __AgentName__::RegisterHandler();

#define SC_AGENT_UNREGISTER(__AgentName__) __AgentName__::UnregisterHandler();
