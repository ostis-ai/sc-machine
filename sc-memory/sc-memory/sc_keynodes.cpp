/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.hpp"
#include "sc_struct.hpp"

internal::ScKeynodesRegister internal::ScKeynodesRegister::m_instance;

size_t const kKeynodeRrelListNum = 20;
std::array<ScAddr, kKeynodeRrelListNum> kKeynodeRrelList;

ScKeynode<sc_type_node | sc_type_const> const ScKeynodes::kMySelf("myself");

ScKeynodeClass const ScKeynodes::kQuestionState("question_state");
ScKeynodeClass const ScKeynodes::kQuestionInitiated("question_initiated");
ScKeynodeClass const ScKeynodes::kQuestionProgressed("question_progressed");
ScKeynodeClass const ScKeynodes::kQuestionStopped("question_stopped");
ScKeynodeClass const ScKeynodes::kQuestionFinished("question_finished");
ScKeynodeClass const ScKeynodes::kQuestionFinishedSuccessfully("question_finished_successfully");
ScKeynodeClass const ScKeynodes::kQuestionFinishedUnsuccessfully("question_finished_unsuccessfully");
ScKeynodeClass const ScKeynodes::kQuestionFinishedWithError("question_finished_with_error");
ScKeynodeNoRole const ScKeynodes::kNrelAnswer("nrel_answer");

ScKeynodeClass const ScKeynodes::kScResult("sc_result");
ScKeynodeClass const ScKeynodes::kScResultOk("sc_result_ok");
ScKeynodeClass const ScKeynodes::kScResultNo("sc_result_no");
ScKeynodeClass const ScKeynodes::kScResultUnknown("sc_result_unknown");
ScKeynodeClass const ScKeynodes::kScResultError("sc_result_error");
ScKeynodeClass const ScKeynodes::kScResultErrorInvalidParams("sc_result_error_invalid_params");
ScKeynodeClass const ScKeynodes::kScResultErrorInvalidType("sc_result_error_invalid_type");
ScKeynodeClass const ScKeynodes::kScResultErrorIO("sc_result_error_io");
ScKeynodeClass const ScKeynodes::kScResultInvalidState("sc_result_error_invalid_state");
ScKeynodeClass const ScKeynodes::kScResultErrorNotFound("sc_result_error_not_found");
ScKeynodeClass const ScKeynodes::kScResultErrorFullMemory("sc_result_error_full_memory");
ScKeynodeClass const ScKeynodes::kScResultErrorAddrIsNotValid("sc_result_error_addr_is_not_valid");
ScKeynodeClass const ScKeynodes::kScResultErrorElementIsNotNode("sc_result_error_element_is_not_node");
ScKeynodeClass const ScKeynodes::kScResultErrorElementIsNotLink("sc_result_error_element_is_not_link");
ScKeynodeClass const ScKeynodes::kScResultErrorElementIsNotConnector("sc_result_error_element_is_not_corrector");
ScKeynodeClass const ScKeynodes::kScResultErrorFileMemoryIO("sc_result_error_file_memory_io");
ScKeynodeClass const ScKeynodes::kScResultErrorStreamIO("sc_result_error_stream_io");
ScKeynodeClass const ScKeynodes::kScResultErrorInvalidSystemIdentifier("sc_result_error_invalid_system_identifier");
ScKeynodeClass const ScKeynodes::kScResultErrorDuplicatedSystemIdentifier(
    "sc_result_error_duplicated_system_identifier");

ScKeynodeClass const ScKeynodes::kBinaryType("binary_type");
ScKeynodeClass const ScKeynodes::kBinaryFloat("binary_float");
ScKeynodeClass const ScKeynodes::kBinaryDouble("binary_double");
ScKeynodeClass const ScKeynodes::kBinaryInt8("binary_int8");
ScKeynodeClass const ScKeynodes::kBinaryInt16("binary_int16");
ScKeynodeClass const ScKeynodes::kBinaryInt32("binary_int32");
ScKeynodeClass const ScKeynodes::kBinaryInt64("binary_int64");
ScKeynodeClass const ScKeynodes::kBinaryUInt8("binary_uint8");
ScKeynodeClass const ScKeynodes::kBinaryUInt16("binary_uint16");
ScKeynodeClass const ScKeynodes::kBinaryUInt32("binary_uint32");
ScKeynodeClass const ScKeynodes::kBinaryUInt64("binary_uint64");
ScKeynodeClass const ScKeynodes::kBinaryString("binary_string");
ScKeynodeClass const ScKeynodes::kBinaryCustom("binary_custom");

ScKeynodeClass const ScKeynodes::kEmptyClass;

sc_result ScKeynodes::Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr)
{
  internal::ScKeynodesRegister::m_instance.Register(ctx, initMemoryGeneratedStructureAddr);

  sc_bool result;

  // init sc_result set
  for (size_t i = 0; i < SC_RESULT_COUNT; ++i)
  {
    ScAddr const resAddr = GetResultCodeAddr(static_cast<sc_result>(i));
    result = result && resAddr.IsValid();
    if (!ctx->HelperCheckEdge(kScResult, resAddr, ScType::EdgeAccessConstPosPerm))
    {
      ScAddr const & edge = ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kScResult, resAddr);
      result = result && edge.IsValid();

      if (initMemoryGeneratedStructureAddr.IsValid()
          && !ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructureAddr, edge).IsValid())
        result = SC_FALSE;
    }
  }

  // resolve rrel_n relations
  for (size_t i = 0; i < kKeynodeRrelListNum; ++i)
  {
    ScAddr & item = kKeynodeRrelList[i];
    item = ctx->HelperResolveSystemIdtf("rrel_" + std::to_string(i + 1), ScType::NodeConstRole);
    if (!item.IsValid())
      result = SC_FALSE;
  }

  // command states
  ScAddr states[] = {kQuestionInitiated, kQuestionProgressed, kQuestionStopped, kQuestionFinished};
  for (auto const & a : states)
  {
    if (!ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kQuestionState, a).IsValid())
      result = SC_FALSE;
  }

  // binary types
  ScAddr binaryTypes[] = {
      kBinaryDouble,
      kBinaryFloat,
      kBinaryString,
      kBinaryInt8,
      kBinaryInt16,
      kBinaryInt32,
      kBinaryInt64,
      kBinaryUInt8,
      kBinaryUInt16,
      kBinaryUInt32,
      kBinaryUInt64,
      kBinaryCustom};
  for (auto const & b : binaryTypes)
  {
    ScAddr const & edge = ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kBinaryType, b);
    if (!edge.IsValid())
      result = SC_FALSE;

    if (initMemoryGeneratedStructureAddr.IsValid()
        && !ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructureAddr, edge).IsValid())
      result = SC_FALSE;
  }

  return result ? SC_RESULT_OK : SC_RESULT_ERROR;
}

sc_result ScKeynodes::Shutdown(ScMemoryContext * ctx)
{
  internal::ScKeynodesRegister::m_instance.Unregister(ctx);

  return SC_RESULT_OK;
}

ScAddr const & ScKeynodes::GetResultCodeAddr(sc_result resCode)
{
  switch (resCode)
  {
  case SC_RESULT_ERROR_FULL_MEMORY:
    return kScResultErrorFullMemory;
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    return kScResultErrorAddrIsNotValid;
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE:
    return kScResultErrorElementIsNotNode;
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
    return kScResultErrorElementIsNotLink;
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR:
    return kScResultErrorElementIsNotConnector;
  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    return kScResultErrorFileMemoryIO;
  case SC_RESULT_ERROR_STREAM_IO:
    return kScResultErrorStreamIO;
  case SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER:
    return kScResultErrorInvalidSystemIdentifier;
  case SC_RESULT_ERROR_DUPLICATED_SYSTEM_IDENTIFIER:
    return kScResultErrorDuplicatedSystemIdentifier;
  case SC_RESULT_UNKNOWN:
    return kScResultUnknown;
  default:
    break;
  }

  return kScResultError;
}

sc_result ScKeynodes::GetResultCodeByAddr(ScAddr const & resultClassAddr)
{
  if (!resultClassAddr.IsValid())
    return SC_RESULT_UNKNOWN;

  if (resultClassAddr == kScResultError)
    return SC_RESULT_ERROR;
  else if (resultClassAddr == kScResultOk)
    return SC_RESULT_OK;
  else if (resultClassAddr == kScResultNo)
    return SC_RESULT_NO;
  else if (resultClassAddr == kScResultErrorInvalidParams)
    return SC_RESULT_ERROR_INVALID_PARAMS;
  else if (resultClassAddr == kScResultErrorInvalidType)
    return SC_RESULT_ERROR_INVALID_TYPE;
  else if (resultClassAddr == kScResultInvalidState)
    return SC_RESULT_ERROR_INVALID_STATE;
  else if (resultClassAddr == kScResultErrorNotFound)
    return SC_RESULT_ERROR_NOT_FOUND;
  else if (resultClassAddr == kScResultErrorFullMemory)
    return SC_RESULT_ERROR_FULL_MEMORY;
  else if (resultClassAddr == kScResultErrorAddrIsNotValid)
    return SC_RESULT_ERROR_ADDR_IS_NOT_VALID;
  else if (resultClassAddr == kScResultErrorElementIsNotNode)
    return SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE;
  else if (resultClassAddr == kScResultErrorElementIsNotLink)
    return SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK;
  else if (resultClassAddr == kScResultErrorElementIsNotConnector)
    return SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
  else if (resultClassAddr == kScResultErrorFileMemoryIO)
    return SC_RESULT_ERROR_FILE_MEMORY_IO;
  else if (resultClassAddr == kScResultErrorStreamIO)
    return SC_RESULT_ERROR_STREAM_IO;
  else if (resultClassAddr == kScResultErrorInvalidSystemIdentifier)
    return SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER;
  else if (resultClassAddr == kScResultErrorDuplicatedSystemIdentifier)
    return SC_RESULT_ERROR_DUPLICATED_SYSTEM_IDENTIFIER;

  return SC_RESULT_UNKNOWN;
}

ScAddr const & ScKeynodes::GetRrelIndex(size_t idx)
{
  if (idx >= kKeynodeRrelListNum)
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "You should use index in range[0; " + std::to_string(kKeynodeRrelListNum) + "]");
  }

  return kKeynodeRrelList[idx];
}

size_t ScKeynodes::GetRrelIndexNum()
{
  return kKeynodeRrelListNum;
}
