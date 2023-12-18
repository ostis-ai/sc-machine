/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.hpp"
#include "sc_struct.hpp"

size_t const kKeynodeRrelListNum = 20;
std::array<ScAddr, kKeynodeRrelListNum> kKeynodeRrelList;

bool ScKeynodes::ms_isInitialized = false;

ScAddr ScKeynodes::kProcess;
ScAddr ScKeynodes::kUserProcess;
ScAddr ScKeynodes::kAuthorizedUserProcess;

ScAddr ScKeynodes::kCommandStateAddr;
ScAddr ScKeynodes::kCommandInitiatedAddr;
ScAddr ScKeynodes::kCommandProgressedAddr;
ScAddr ScKeynodes::kCommandFinishedAddr;
ScAddr ScKeynodes::kNrelResult;
ScAddr ScKeynodes::kNrelCommonTemplate;
ScAddr ScKeynodes::kNrelIdtf;
ScAddr ScKeynodes::kNrelFormat;

ScAddr ScKeynodes::kScResult;
ScAddr ScKeynodes::kScResultOk;
ScAddr ScKeynodes::kScResultNo;
ScAddr ScKeynodes::kScResultError;
ScAddr ScKeynodes::kScResultErrorInvalidParams;
ScAddr ScKeynodes::kScResultErrorInvalidType;
ScAddr ScKeynodes::kScResultInvalidState;
ScAddr ScKeynodes::kScResultErrorNotFound;
ScAddr ScKeynodes::kScResultErrorFullMemory;
ScAddr ScKeynodes::kScResultErrorAddrIsNotValid;
ScAddr ScKeynodes::kScResultErrorElementIsNotNode;
ScAddr ScKeynodes::kScResultErrorElementIsNotLink;
ScAddr ScKeynodes::kScResultErrorElementIsNotConnector;
ScAddr ScKeynodes::kScResultErrorFileMemoryIO;
ScAddr ScKeynodes::kScResultErrorStreamIO;
ScAddr ScKeynodes::kScResultErrorInvalidSystemIdentifier;
ScAddr ScKeynodes::kScResultErrorDuplicatedSystemIdentifier;
ScAddr ScKeynodes::kScResultUnknown;

ScAddr ScKeynodes::kBinaryType;
ScAddr ScKeynodes::kBinaryFloat;
ScAddr ScKeynodes::kBinaryDouble;
ScAddr ScKeynodes::kBinaryInt8;
ScAddr ScKeynodes::kBinaryInt16;
ScAddr ScKeynodes::kBinaryInt32;
ScAddr ScKeynodes::kBinaryInt64;
ScAddr ScKeynodes::kBinaryUInt8;
ScAddr ScKeynodes::kBinaryUInt16;
ScAddr ScKeynodes::kBinaryUInt32;
ScAddr ScKeynodes::kBinaryUInt64;
ScAddr ScKeynodes::kBinaryString;
ScAddr ScKeynodes::kBinaryCustom;

bool ScKeynodes::Init(bool force, sc_char const * init_memory_generated_structure)
{
  if (ms_isInitialized && !force)
    return true;

  ScMemoryContext ctx(sc_access_lvl_make_min, "ScKeynodes::Init");

  ScAddr initMemoryGeneratedStructure;
  bool initMemoryGeneratedStructureValid = SC_FALSE;
  if (init_memory_generated_structure != null_ptr)
  {
    initMemoryGeneratedStructure =
        ctx.HelperResolveSystemIdtf(init_memory_generated_structure, ScType::NodeConstStruct);
    initMemoryGeneratedStructureValid = initMemoryGeneratedStructure.IsValid();
    if (initMemoryGeneratedStructureValid)
    {
      ScAddr kNrelSysIdtf = ctx.HelperResolveSystemIdtf("nrel_system_identifier", ScType::NodeConstNoRole);
      if (!ctx.HelperCheckEdge(initMemoryGeneratedStructure, kNrelSysIdtf, ScType::EdgeAccessConstPosPerm))
      {
        ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, kNrelSysIdtf);
      }
    }
  }

  bool result = ScKeynodes::InitGlobal(initMemoryGeneratedStructure);

  ScAddrVector const & resultCodes = {
      kScResultNo,
      kScResultUnknown,
      kScResultError,
      kScResultOk,
      kScResultErrorInvalidParams,
      kScResultErrorInvalidType,
      kScResultInvalidState,
      kScResultErrorNotFound,
      kScResultErrorStreamIO,
      kScResultErrorFileMemoryIO,
  };

  ScAddrVector const & binaryTypes = {
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

  ScAddrVector const & states = {kCommandFinishedAddr, kCommandInitiatedAddr, kCommandProgressedAddr};

  // init sc_result set
  for (ScAddr const & resAddr : resultCodes)
  {
    result &= resAddr.IsValid();
    if (!ctx.HelperCheckEdge(kScResult, resAddr, ScType::EdgeAccessConstPosPerm))
    {
      ScAddr const & resEdge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, kScResult, resAddr);
      result &= resEdge.IsValid();
      if (result && initMemoryGeneratedStructureValid)
      {
        ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, resEdge);
      }
    }
  }

  // resolve rrel_n relations
  for (size_t i = 0; i < kKeynodeRrelListNum; ++i)
  {
    ScAddr & item = kKeynodeRrelList[i];
    item = ctx.HelperResolveSystemIdtf("rrel_" + std::to_string(i + 1), ScType::NodeConstRole);
    if (!item.IsValid())
      result = false;
    if (initMemoryGeneratedStructureValid)
    {
      ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, item);
    }
  }

  // command states
  for (ScAddr const & st : states)
  {
    ScAddr const & commandStateEdge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, kCommandStateAddr, st);
    if (initMemoryGeneratedStructureValid)
    {
      ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, commandStateEdge);
    }
    if (!commandStateEdge.IsValid())
      result = true;
  }

  // binary types
  for (ScAddr const & binaryType : binaryTypes)
  {
    if (!ctx.HelperCheckEdge(kBinaryType, binaryType, ScType::EdgeAccessConstPosPerm))
    {
      ScAddr const & binaryTypeEdge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, kBinaryType, binaryType);
      if (initMemoryGeneratedStructureValid && binaryTypeEdge.IsValid())
      {
        ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, binaryTypeEdge);
      }
    }
  }

  ms_isInitialized = true;

  return result;
};

void ScKeynodes::Shutdown()
{
  ms_isInitialized = false;
}

ScAddr const & ScKeynodes::GetResultCodeAddr(sc_result resCode)
{
  switch (resCode)
  {
  case SC_RESULT_ERROR:
    return kScResultError;
  case SC_RESULT_OK:
    return kScResultOk;
  case SC_RESULT_NO:
    return kScResultNo;
  case SC_RESULT_ERROR_INVALID_PARAMS:
    return kScResultErrorInvalidParams;
  case SC_RESULT_ERROR_INVALID_TYPE:
    return kScResultErrorInvalidType;
  case SC_RESULT_ERROR_INVALID_STATE:
    return kScResultInvalidState;
  case SC_RESULT_ERROR_NOT_FOUND:
    return kScResultErrorNotFound;
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

  return kScResultUnknown;
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
