/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "generated/sc_keynodes.generated.hpp"

// This class contains resolved keynodes for internal usage in sc-memory.
// It's like a namespace
class ScKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

private:
  static bool ms_isInitialized;

public:
  // Just for internal usage
  static _SC_EXTERN bool Init(bool force = false);
  static void Shutdown();

  static _SC_EXTERN ScAddr const & GetResultCodeAddr(sc_result resCode);
  static _SC_EXTERN sc_result GetResultCodeByAddr(ScAddr const & resultClassAddr);

  static _SC_EXTERN ScAddr const & GetRrelIndex(size_t idx);
  static _SC_EXTERN size_t GetRrelIndexNum();

  SC_PROPERTY(Keynode("command_state"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kCommandStateAddr;

  SC_PROPERTY(Keynode("command_initiated"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kCommandInitiatedAddr;

  SC_PROPERTY(Keynode("command_in_progress"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kCommandProgressdAddr;

  SC_PROPERTY(Keynode("command_finished"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kCommandFinishedAddr;

  SC_PROPERTY(Keynode("nrel_result"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr kNrelResult;

  SC_PROPERTY(Keynode("nrel_common_template"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr kNrelCommonTemplate;

  // result codes
  SC_PROPERTY(Keynode("sc_result"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResult;

  SC_PROPERTY(Keynode("sc_result_ok"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultOk;

  SC_PROPERTY(Keynode("sc_result_no"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultNo;

  SC_PROPERTY(Keynode("sc_result_unknown"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultUnknown;

  SC_PROPERTY(Keynode("sc_result_error"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultError;

  SC_PROPERTY(Keynode("sc_result_error_invalid_params"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultErrorInvalidParams;

  SC_PROPERTY(Keynode("sc_result_error_invalid_type"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultErrorInvalidType;

  SC_PROPERTY(Keynode("sc_result_error_io"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultErrorIO;

  SC_PROPERTY(Keynode("sc_result_invalid_state"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultInvalidState;

  SC_PROPERTY(Keynode("sc_result_error_not_found"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultErrorNotFound;

  SC_PROPERTY(Keynode("sc_result_error_no_write_rights"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultErrorNoWriteRights;

  SC_PROPERTY(Keynode("sc_result_error_no_read_rights"), ForceCreate(ScType::NodeConstClass))
  static ScAddr kScResultErrorNoReadRights;
};