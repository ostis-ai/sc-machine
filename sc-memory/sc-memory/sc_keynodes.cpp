/*
 * This source file is part of an OSTIS project. For the lat_est info, see http://ostis.net
 * Distribut_ed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.hpp"

#include "sc_memory.hpp"

#include "sc_struct.hpp"

void internal::ScKeynodesRegister::Remember(ScAddr * keynode, std::string_view const & idtf, ScType const & keynodeType)
{
  m_keynodes.insert({keynode, {idtf, keynodeType}});
}

void internal::ScKeynodesRegister::Forget(ScAddr * keynode)
{
  m_keynodes.erase(keynode);
}

void internal::ScKeynodesRegister::Register(ScMemoryContext * context, ScAddr initMemoryGeneratedStructure)
{
  for (auto const & item : m_keynodes)
  {
    ScAddr * keynode = item.first;
    auto const & keynodeInfo = item.second;
    ScSystemIdentifierQuintuple fiver;
    context->HelperResolveSystemIdtf(std::string(keynodeInfo.first), keynodeInfo.second, fiver);
    *keynode = fiver.addr1;

    if (initMemoryGeneratedStructure.IsValid())
    {
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr1);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr2);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr3);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr4);
    }
  }
}

void internal::ScKeynodesRegister::Unregister(ScMemoryContext *) {}

ScKeynode::ScKeynode(std::string_view const & sysIdtf, ScType const & type)
  : ScAddr(ScAddr::Empty)
{
  if (!sysIdtf.empty())
    internal::ScKeynodesRegister::Remember(this, sysIdtf, type);
}

ScKeynode::ScKeynode(sc_addr const & addr)
  : ScAddr(addr)
{
}

ScKeynode::~ScKeynode()
{
  internal::ScKeynodesRegister::Forget(this);
}

ScKeynode::ScKeynode(ScKeynode const & other)
  : ScAddr(other)
{
}

ScKeynode & ScKeynode::operator=(ScKeynode const & other)
{
  this->m_realAddr = other.m_realAddr;
  return *this;
}

size_t const kKeynodeRrelListNum = 20;
std::array<ScAddr, kKeynodeRrelListNum> kKeynodeRrelList;

void ScKeynodes::Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr)
{
  SC_LOG_INFO("Initialize " << this->GetName());

  internal::ScKeynodesRegister::Register(ctx, initMemoryGeneratedStructureAddr);

  // init sc_result set
  for (size_t i = 0; i < SC_RESULT_COUNT; ++i)
  {
    ScAddr const resAddr = GetResultCodeAddr(static_cast<sc_result>(i));
    if (!ctx->HelperCheckEdge(sc_result_class, resAddr, ScType::EdgeAccessConstPosPerm))
    {
      ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, sc_result_class, resAddr);

      if (initMemoryGeneratedStructureAddr.IsValid())
        ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructureAddr, arcAddr);
    }
  }

  // resolve rrel_n relations
  for (size_t i = 0; i < kKeynodeRrelListNum; ++i)
  {
    ScAddr & item = kKeynodeRrelList[i];
    item = ctx->HelperResolveSystemIdtf("rrel_" + std::to_string(i + 1), ScType::NodeConstRole);
  }

  // command states
  ScAddr states[] = {action_initiated, action_finished};
  for (auto const & a : states)
  {
    ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, action_state, a);
  }

  // binary types
  ScAddr binaryTypes[] = {
      binary_double,
      binary_float,
      binary_string,
      binary_int8,
      binary_int16,
      binary_int32,
      binary_int64,
      binary_uint8,
      binary_uint16,
      binary_uint32,
      binary_uint64,
      binary_custom};
  for (auto const & b : binaryTypes)
  {
    ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, binary_type, b);

    if (initMemoryGeneratedStructureAddr.IsValid())
      ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructureAddr, arcAddr);
  }
}

void ScKeynodes::Shutdown(ScMemoryContext * ctx)
{
  SC_LOG_INFO("Shutdown " << this->GetName());
  internal::ScKeynodesRegister::Unregister(ctx);
}

ScAddr const & ScKeynodes::GetResultCodeAddr(sc_result resCode)
{
  switch (resCode)
  {
  case SC_RESULT_ERROR_FULL_MEMORY:
    return sc_result_error_full_memory;
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    return sc_result_error_addr_is_not_valid;
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE:
    return sc_result_error_element_is_not_node;
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
    return sc_result_error_element_is_not_link;
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR:
    return sc_result_error_element_is_not_connector;
  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    return sc_result_error_file_memory_io;
  case SC_RESULT_ERROR_STREAM_IO:
    return sc_result_error_stream_io;
  case SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER:
    return sc_result_error_invalid_system_identifier;
  case SC_RESULT_ERROR_DUPLICATED_SYSTEM_IDENTIFIER:
    return sc_result_error_duplicated_system_identifier;
  case SC_RESULT_UNKNOWN:
    return sc_result_unknown;
  default:
    break;
  }

  return sc_result_error;
}

sc_result ScKeynodes::GetResultCodeByAddr(ScAddr const & resultClassAddr)
{
  if (!resultClassAddr.IsValid())
    return SC_RESULT_UNKNOWN;

  if (resultClassAddr == sc_result_error)
    return SC_RESULT_ERROR;
  else if (resultClassAddr == sc_result_ok)
    return SC_RESULT_OK;
  else if (resultClassAddr == sc_result_no)
    return SC_RESULT_NO;
  else if (resultClassAddr == sc_result_error_invalid_params)
    return SC_RESULT_ERROR_INVALID_PARAMS;
  else if (resultClassAddr == sc_result_error_invalid_type)
    return SC_RESULT_ERROR_INVALID_TYPE;
  else if (resultClassAddr == sc_result_invalid_state)
    return SC_RESULT_ERROR_INVALID_STATE;
  else if (resultClassAddr == sc_result_error_not_found)
    return SC_RESULT_ERROR_NOT_FOUND;
  else if (resultClassAddr == sc_result_error_full_memory)
    return SC_RESULT_ERROR_FULL_MEMORY;
  else if (resultClassAddr == sc_result_error_addr_is_not_valid)
    return SC_RESULT_ERROR_ADDR_IS_NOT_VALID;
  else if (resultClassAddr == sc_result_error_element_is_not_node)
    return SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE;
  else if (resultClassAddr == sc_result_error_element_is_not_link)
    return SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK;
  else if (resultClassAddr == sc_result_error_element_is_not_connector)
    return SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
  else if (resultClassAddr == sc_result_error_file_memory_io)
    return SC_RESULT_ERROR_FILE_MEMORY_IO;
  else if (resultClassAddr == sc_result_error_stream_io)
    return SC_RESULT_ERROR_STREAM_IO;
  else if (resultClassAddr == sc_result_error_invalid_system_identifier)
    return SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER;
  else if (resultClassAddr == sc_result_error_duplicated_system_identifier)
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
