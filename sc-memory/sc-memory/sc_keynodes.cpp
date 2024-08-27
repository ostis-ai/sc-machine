/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.hpp"

#include "sc_memory.hpp"

#include "sc_struct.hpp"

void internal::ScKeynodesRegister::Remember(ScKeynode * keynode)
{
  m_keynodes.push_back(keynode);
}

void internal::ScKeynodesRegister::Forget(ScKeynode * keynode)
{
  m_keynodes.remove(keynode);
}

void internal::ScKeynodesRegister::Register(ScMemoryContext * context)
{
  for (auto * keynode : m_keynodes)
    keynode->Initialize(context);
}

void internal::ScKeynodesRegister::Unregister(ScMemoryContext *) {}

ScKeynode::ScKeynode(std::string_view const & sysIdtf, ScType const & type)
  : ScAddr(ScAddr::Empty)
  , m_sysIdtf(sysIdtf)
  , m_type(type)
{
  if (!m_sysIdtf.empty())
    internal::ScKeynodesRegister::Remember(this);
}

ScKeynode::~ScKeynode()
{
  internal::ScKeynodesRegister::Forget(this);
}

ScKeynode::operator std::string() const
{
  return std::string(m_sysIdtf);
}

void ScKeynode::Initialize(ScMemoryContext * context)
{
  ScAddr const & contextStructureAddr = context->GetContextStructure();
  auto const & AppendToContextStructure = [&](ScAddr const & addr)
  {
    if (context->HelperCheckEdge(contextStructureAddr, addr, ScType::EdgeAccessConstPosPerm))
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, contextStructureAddr, addr);
  };

  ScSystemIdentifierQuintuple quintuple;
  context->HelperResolveSystemIdtf(std::string(m_sysIdtf), m_type, quintuple);
  this->m_realAddr = quintuple.addr1.GetRealAddr();

  if (contextStructureAddr.IsValid())
  {
    AppendToContextStructure(quintuple.addr1);
    AppendToContextStructure(quintuple.addr2);
    AppendToContextStructure(quintuple.addr3);
    AppendToContextStructure(quintuple.addr4);
  }
}

ScTemplateKeynode::ScTemplateKeynode(std::string_view const & sysIdtf)
  : ScKeynode(sysIdtf)
{
  if (!m_sysIdtf.empty())
    internal::ScKeynodesRegister::Remember(this);
}

ScTemplateKeynode::~ScTemplateKeynode() = default;

ScTemplateKeynode::ScTemplateKeynode(ScTemplateKeynode && other) noexcept
  : ScKeynode(other)
  , ScTemplate(std::move(other))
  , m_constructionInitializers(other.m_constructionInitializers)
{
  internal::ScKeynodesRegister::Forget(&other);
  internal::ScKeynodesRegister::Remember(this);
}

void ScTemplateKeynode::Initialize(ScMemoryContext * context)
{
  this->Clear();

  ScAddr const & contextStructureAddr = context->GetContextStructure();
  auto const & AppendToContextStructure = [&](ScAddr const & addr)
  {
    if (context->HelperCheckEdge(contextStructureAddr, addr, ScType::EdgeAccessConstPosPerm))
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, contextStructureAddr, addr);
  };

  ScSystemIdentifierQuintuple quintuple;
  std::string const & sysIdtf = std::string(m_sysIdtf);
  if (!context->HelperFindBySystemIdtf(sysIdtf, quintuple))
  {
    for (auto const & initializer : m_constructionInitializers)
      initializer(*this);

    context->HelperLoadTemplate(*this, *this);
    context->HelperSetSystemIdtf(sysIdtf, *this, quintuple);
  }
  else
  {
    this->m_realAddr = quintuple.addr1.GetRealAddr();
    context->HelperBuildTemplate(*this, *this);
  }

  if (contextStructureAddr.IsValid())
  {
    AppendToContextStructure(quintuple.addr1);
    AppendToContextStructure(quintuple.addr2);
    AppendToContextStructure(quintuple.addr3);
    AppendToContextStructure(quintuple.addr4);
  }
}

size_t const kKeynodeRrelListNum = 20;
std::array<ScAddr, kKeynodeRrelListNum> kKeynodeRrelList;

void ScKeynodes::Initialize(ScMemoryContext * ctx)
{
  ScAddr const & contextStructureAddr = ctx->GetContextStructure();

  auto const & ResolveArc = [&](ScAddr const & beginAddr, ScAddr const & endAddr)
  {
    ScAddr arcAddr;
    ScIterator3Ptr it3 = ctx->Iterator3(ScType::EdgeAccessConstPosPerm, beginAddr, endAddr);
    if (it3->Next())
      arcAddr = it3->Get(1);
    else
      arcAddr = ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, beginAddr, endAddr);

    if (contextStructureAddr.IsValid())
      ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, contextStructureAddr, arcAddr);
  };

  SC_LOG_INFO("Initialize " << GetName<ScKeynodes>());

  internal::ScKeynodesRegister::Register(ctx);

  // init sc_result set
  for (size_t i = 0; i < SC_RESULT_COUNT; ++i)
  {
    ScAddr const resultAddr = GetResultCodeAddr(static_cast<sc_result>(i));
    ResolveArc(sc_result_class, resultAddr);
  }

  // resolve rrel_n relations
  for (size_t i = 0; i < kKeynodeRrelListNum; ++i)
  {
    ScAddr & item = kKeynodeRrelList[i];
    item = ctx->HelperResolveSystemIdtf("rrel_" + std::to_string(i + 1), ScType::NodeConstRole);
  }

  // command states
  ScAddr states[] = {action_deactivated, action_initiated, action_finished};
  for (auto const & state : states)
  {
    ResolveArc(action_state, state);
  }

  // sc-events
  ScAddr events[] = {
      sc_event_unknown,
      sc_event_after_generate_connector,
      sc_event_after_generate_incoming_arc,
      sc_event_after_generate_outgoing_arc,
      sc_event_after_generate_edge,
      sc_event_before_erase_connector,
      sc_event_before_erase_incoming_arc,
      sc_event_before_erase_outgoing_arc,
      sc_event_before_erase_edge,
      sc_event_before_erase_element,
      sc_event_before_change_link_content};
  for (auto const & event : events)
  {
    ResolveArc(sc_event, event);
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
  for (auto const & type : binaryTypes)
  {
    ResolveArc(binary_type, type);
  }
}

void ScKeynodes::Shutdown(ScMemoryContext * ctx)
{
  SC_LOG_INFO("Shutdown " << GetName<ScKeynodes>());
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

ScAddr const & ScKeynodes::GetRrelIndex(size_t idx)
{
  if (idx == 0 || idx >= kKeynodeRrelListNum)
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "You should use index in range[1; " + std::to_string(kKeynodeRrelListNum) + "]");
  }

  return kKeynodeRrelList[idx - 1];
}

size_t ScKeynodes::GetRrelIndexNum()
{
  return kKeynodeRrelListNum;
}
