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
ScAddr ScKeynodes::kScResultUnknown;
ScAddr ScKeynodes::kScResultError;
ScAddr ScKeynodes::kScResultErrorInvalidParams;
ScAddr ScKeynodes::kScResultErrorInvalidType;
ScAddr ScKeynodes::kScResultErrorIO;
ScAddr ScKeynodes::kScResultInvalidState;
ScAddr ScKeynodes::kScResultErrorNotFound;
ScAddr ScKeynodes::kScResultErrorNoWriteRights;
ScAddr ScKeynodes::kScResultErrorNoReadRights;

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

bool ScKeynodes::Init(bool force)
{
  if (ms_isInitialized && !force)
    return true;

  bool result = ScKeynodes::InitGlobal();
  ScMemoryContext ctx(sc_access_lvl_make_min, "ScKeynodes::Init");

  // init sc_result set
  for (size_t i = 0; i < SC_RESULT_COUNT; ++i)
  {
    ScAddr const resAddr = GetResultCodeAddr(static_cast<sc_result>(i));
    result = result && resAddr.IsValid();
    if (!ctx.HelperCheckEdge(kScResult, resAddr, ScType::EdgeAccessConstPosPerm))
    {
      result = result && ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, kScResult, resAddr).IsValid();
    }
  }

  // resolve rrel_n relations
  for (size_t i = 0; i < kKeynodeRrelListNum; ++i)
  {
    ScAddr & item = kKeynodeRrelList[i];
    item = ctx.HelperResolveSystemIdtf("rrel_" + std::to_string(i + 1), ScType::NodeConstRole);
    if (!item.IsValid())
      result = false;
    SC_ASSERT(item.IsValid(), ());
  }

  // command states
  ScAddr states[] = {kCommandFinishedAddr, kCommandInitiatedAddr, kCommandProgressedAddr};
  for (auto const & a : states)
  {
    if (!ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, kCommandStateAddr, a).IsValid())
      result = true;
  }

  // binary types
  {
    ScSet set(ctx, kBinaryType);
    set << kBinaryDouble << kBinaryFloat << kBinaryString << kBinaryInt8 << kBinaryInt16 << kBinaryInt32 << kBinaryInt64
        << kBinaryUInt8 << kBinaryUInt16 << kBinaryUInt32 << kBinaryUInt64 << kBinaryCustom;
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
  case SC_RESULT_UNKNOWN:
    return kScResultUnknown;
  case SC_RESULT_NO:
    return kScResultNo;
  case SC_RESULT_ERROR:
    return kScResultError;
  case SC_RESULT_OK:
    return kScResultOk;
  case SC_RESULT_ERROR_INVALID_PARAMS:
    return kScResultErrorInvalidParams;
  case SC_RESULT_ERROR_INVALID_TYPE:
    return kScResultErrorInvalidType;
  case SC_RESULT_ERROR_IO:
    return kScResultErrorIO;
  case SC_RESULT_ERROR_INVALID_STATE:
    return kScResultInvalidState;
  case SC_RESULT_ERROR_NOT_FOUND:
    return kScResultErrorNotFound;
  case SC_RESULT_ERROR_NO_WRITE_RIGHTS:
    return kScResultErrorNoWriteRights;
  case SC_RESULT_ERROR_NO_READ_RIGHTS:
    return kScResultErrorNoReadRights;

  default:
    return kScResultError;
  };

  return kScResultError;
}

sc_result ScKeynodes::GetResultCodeByAddr(ScAddr const & resultClassAddr)
{
  if (!resultClassAddr.IsValid())
    return SC_RESULT_UNKNOWN;

  if (resultClassAddr == kScResultNo)
    return SC_RESULT_NO;
  else if (resultClassAddr == kScResultError)
    return SC_RESULT_ERROR;
  else if (resultClassAddr == kScResultOk)
    return SC_RESULT_OK;
  else if (resultClassAddr == kScResultErrorInvalidParams)
    return SC_RESULT_ERROR_INVALID_PARAMS;
  else if (resultClassAddr == kScResultErrorInvalidType)
    return SC_RESULT_ERROR_INVALID_TYPE;
  else if (resultClassAddr == kScResultErrorIO)
    return SC_RESULT_ERROR_IO;
  else if (resultClassAddr == kScResultInvalidState)
    return SC_RESULT_ERROR_INVALID_STATE;
  else if (resultClassAddr == kScResultErrorNotFound)
    return SC_RESULT_ERROR_NOT_FOUND;
  else if (resultClassAddr == kScResultErrorNoWriteRights)
    return SC_RESULT_ERROR_NO_WRITE_RIGHTS;
  else if (resultClassAddr == kScResultErrorNoReadRights)
    return SC_RESULT_ERROR_NO_READ_RIGHTS;

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
