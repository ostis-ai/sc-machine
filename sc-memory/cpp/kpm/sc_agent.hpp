/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/utils/sc_log.hpp"

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
  _SC_EXTERN virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr);

protected:
  mutable ScMemoryContext m_memoryCtx;
};

class ScAgentAction : public ScAgent
{
  friend class ScAgentRunner;

  SC_CLASS()
  SC_GENERATED_BODY()

public:
  _SC_EXTERN explicit ScAgentAction(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max);
  _SC_EXTERN virtual ~ScAgentAction();

  // Just for internal usage
  static bool InitSets(ScMemoryContext & ctx);

protected:
  _SC_EXTERN virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

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
  static _SC_EXTERN ScAddr const & GetResultCodeAddr(sc_result resCode);
  static _SC_EXTERN sc_result GetResultCodeByAddr(ScAddr const & resultClassAddr);

  /* Create instance of specified command with arguments
   * cmdClassAddr - node that designate command class
   * For each parameter from params would be added attribute rrel_<i + 1>, where
   * i is an index of parameter in vector. Maximum size of params is 10.
   * Returns ScAddr of created command instance. If addr is not valid, then
   * command doesn't created
   */
  static _SC_EXTERN ScAddr CreateCommand(ScMemoryContext & ctx, ScAddr const & cmdClassAddr, ScAddrVector const & params);

  /* Init specified command.
   * cmdAddr - ScAddr of command instance
   * Returns true, when command initiated; otherwise returns false
   */
  static _SC_EXTERN bool InitiateCommand(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Returns ScAddr of result structure of specified command
   */
  static _SC_EXTERN ScAddr GetCommandResultAddr(ScMemoryContext & ctx, ScAddr const & cmdAddr);

  /* Returns result code of specified command
   * If command still haven't result (possible not finished), then return
   * SC_RESULT_UNKNOWN. Be attentive, SC_RESULT_UNKNOWN is not a guarantee, that 
   * process isn't finished. It could be finished with that result.
   */
  static _SC_EXTERN sc_result GetCommandResultCode(ScMemoryContext & ctx, ScAddr const & cmdAddr);

private:
  ScAddr m_cmdClassAddr;

private:
  SC_PROPERTY(Keynode("command_initiated"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_commandInitiatedAddr;

  SC_PROPERTY(Keynode("command_in_progress"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_commandProgressdAddr;

  SC_PROPERTY(Keynode("command_finished"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_commandFinishedAddr;

  SC_PROPERTY(Keynode("nrel_result"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr ms_nrelResult;

  SC_PROPERTY(Keynode("nrel_common_template"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr ms_nrelCommonTemplate;

  // result codes
  SC_PROPERTY(Keynode("sc_result"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResult;

  SC_PROPERTY(Keynode("sc_result_ok"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultOk;

  SC_PROPERTY(Keynode("sc_result_no"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultNo;

  SC_PROPERTY(Keynode("sc_result_unknown"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultUnknown;

  SC_PROPERTY(Keynode("sc_result_error"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultError;

  SC_PROPERTY(Keynode("sc_result_error_invalid_params"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultErrorInvalidParams;

  SC_PROPERTY(Keynode("sc_result_error_invalid_type"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultErrorInvalidType;

  SC_PROPERTY(Keynode("sc_result_error_io"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultErrorIO;

  SC_PROPERTY(Keynode("sc_result_invalid_state"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultInvalidState;

  SC_PROPERTY(Keynode("sc_result_error_not_found"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultErrorNotFound;

  SC_PROPERTY(Keynode("sc_result_error_no_write_rights"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultErrorNoWriteRights;

  SC_PROPERTY(Keynode("sc_result_error_no_read_rights"), ForceCreate(ScType::NodeConstClass))
  static ScAddr ms_keynodeScResultErrorNoReadRights;
};

#define AGENT_NAME_TYPE(__Name__) __Name__##Type
#define AGENT_NAME_CLASS(__Name__) __Name__##_Agent
#define AGENT_NAME_INST(__Name__) __Name__/*##(__LINE__)##(__FILE__)*/


#define SC_AGENT_IMPLEMENTATION(__AgentName__) \
  SC_COMBINE(ScFileID, _, __AgentName__, _impl) \
  sc_result __AgentName__::Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)

#define SC_AGENT_ACTION_IMPLEMENTATION(__AgentName__) \
  SC_COMBINE(ScFileID, _, __AgentName__, _impl) \
  sc_result __AgentName__::RunImpl(ScAddr const & requestAddr, ScAddr const & resultAddr)



#define SC_AGENT_REGISTER(__AgentName__) \
  SC_OBJECT_INIT_GLOBAL_CALL(__AgentName__) \
  __AgentName__::RegisterHandler();

#define SC_AGENT_UNREGISTER(__AgentName__)	__AgentName__::UnregisterHandler();
