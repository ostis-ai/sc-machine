/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.hpp"

#include "sc_event_subscription.hpp"

#include "sc_keynodes.hpp"
#include "sc_utils.hpp"
#include "sc_stream.hpp"

#include "utils/sc_log.hpp"

extern "C"
{
#include "sc-core/sc_keynodes.h"
#include "sc-core/sc_memory_private.h"
#include "sc-core/sc_memory_context_private.h"
#include "sc-core/sc_memory_context_permissions.h"
#include "sc-core/sc_memory_headers.h"
#include "sc-core/sc_helper.h"
}

SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN

namespace
{

bool isLogMuted = false;

void _logPrintHandler(sc_char const *, GLogLevelFlags log_level, sc_char const * message, sc_pointer)
{
  if (isLogMuted)
    return;

  std::string stype;
  switch (log_level)
  {
  case G_LOG_LEVEL_CRITICAL:
  case G_LOG_LEVEL_ERROR:
    SC_LOG_ERROR(message);
    break;

  case G_LOG_LEVEL_WARNING:
    SC_LOG_WARNING(message);
    break;

  case G_LOG_LEVEL_INFO:
  case G_LOG_LEVEL_MESSAGE:
    SC_LOG_INFO(message);
    break;

  default:
    SC_LOG_DEBUG(message);
    break;
  }
}

#define CHECK_CONTEXT SC_CHECK(IsValid(), "Used context is invalid. Make sure that it's initialized")

}  // namespace

// ------------------

ScMemoryContext * ScMemory::ms_globalContext = nullptr;
std::string ScMemory::ms_configPath;

bool ScMemory::Initialize(sc_memory_params const & params)
{
  g_log_set_default_handler(_logPrintHandler, nullptr);

  ms_globalContext = new ScMemoryContext();
  sc_memory_context * ctx = sc_memory_initialize(&params, &ms_globalContext->m_context);
  if (ctx == nullptr)
    return false;

  ScAddr initMemoryGeneratedStructureAddr;
  if (params.init_memory_generated_upload)
    initMemoryGeneratedStructureAddr =
        ms_globalContext->HelperResolveSystemIdtf(params.init_memory_generated_structure, ScType::NodeConstStruct);

  ScKeynodes::Initialize(ms_globalContext, initMemoryGeneratedStructureAddr);

  utils::ScLog::SetUp(params.log_type, params.log_file, params.log_level);

  return ms_globalContext != nullptr;
}

bool ScMemory::IsInitialized()
{
  return ms_globalContext != nullptr;
}

bool ScMemory::Shutdown(bool saveState /* = true */)
{
  ScKeynodes::Shutdown(ms_globalContext);

  utils::ScLog::SetUp("Console", "", "Info");

  sc_bool result = sc_memory_shutdown(saveState);

  delete ms_globalContext;
  ms_globalContext = nullptr;

  g_log_set_default_handler(g_log_default_handler, nullptr);
  return result;
}

void ScMemory::LogMute()
{
  isLogMuted = true;
  utils::ScLog::GetInstance()->SetMuted(true);
}

void ScMemory::LogUnmute()
{
  isLogMuted = false;
  utils::ScLog::GetInstance()->SetMuted(false);
}

// ---------------

ScMemoryContext::ScMemoryContext() noexcept
  : m_context(sc_memory_context_new_ext(*ScAddr::Empty))
{
}

ScMemoryContext::ScMemoryContext(ScAddr const & userAddr) noexcept
  : m_context(sc_memory_context_new_ext(*userAddr))
{
}

ScMemoryContext::ScMemoryContext(sc_memory_context * context) noexcept
  : m_context(context)
{
}

ScMemoryContext::~ScMemoryContext() noexcept
{
  Destroy();
}

ScMemoryContext::ScMemoryContext(ScMemoryContext && other) noexcept
  : m_context(other.m_context)
  , m_name(other.m_name)
{
  other.m_context = nullptr;
}

ScMemoryContext & ScMemoryContext::operator=(ScMemoryContext && other) noexcept
{
  if (this == &other)
    return *this;

  m_context = other.m_context;
  m_name = other.m_name;
  other.m_context = nullptr;

  return *this;
}

void ScMemoryContext::Destroy() noexcept
{
  if (m_context)
  {
    sc_memory_context_free(m_context);
    m_context = nullptr;
  }
}

ScAddr ScMemoryContext::GetUserAddr()
{
  CHECK_CONTEXT;
  return sc_memory_context_get_user_addr(m_context);
}

sc_memory_context * ScMemoryContext::operator*() const
{
  return m_context;
}

sc_memory_context * ScMemoryContext::GetRealContext() const
{
  return m_context;
}

void ScMemoryContext::BeingEventsPending()
{
  CHECK_CONTEXT;
  sc_memory_context_pending_begin(m_context);
}

void ScMemoryContext::EndEventsPending()
{
  CHECK_CONTEXT;
  sc_memory_context_pending_end(m_context);
}

void ScMemoryContext::BeingEventsBlocking()
{
  CHECK_CONTEXT;
  sc_memory_context_blocking_begin(m_context);
}

void ScMemoryContext::EndEventsBlocking()
{
  CHECK_CONTEXT;
  sc_memory_context_blocking_end(m_context);
}

bool ScMemoryContext::IsValid() const
{
  return m_context != nullptr;
}

bool ScMemoryContext::IsElement(ScAddr const & addr) const
{
  CHECK_CONTEXT;

  sc_result result;
  sc_bool status = sc_memory_is_element_ext(m_context, *addr, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to check sc-element due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to check sc-element due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return status;
}

size_t ScMemoryContext::GetElementOutputArcsCount(ScAddr const & addr) const
{
  CHECK_CONTEXT;

  sc_result result;
  size_t const count = sc_memory_get_element_output_arcs_count(m_context, *addr, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get output arcs count");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get output arcs count due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get output arcs count due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return count;
}

size_t ScMemoryContext::GetElementInputArcsCount(ScAddr const & addr) const
{
  CHECK_CONTEXT;

  sc_result result;
  size_t const count = sc_memory_get_element_input_arcs_count(m_context, *addr, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get input arcs count");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get input arcs count due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get input arcs count due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return count;
}

bool ScMemoryContext::EraseElement(ScAddr const & addr)
{
  CHECK_CONTEXT;

  sc_result const result = sc_memory_element_free(m_context, *addr);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to erase sc-element due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to erase sc-element due sc-memory context hasn't erase permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_PERMISSIONS_TO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to erase sc-element due sc-memory context hasn't permissions to erase permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

ScAddr ScMemoryContext::CreateNode(ScType const & type)
{
  CHECK_CONTEXT;

  sc_result result;
  sc_addr const addr = sc_memory_node_new_ext(m_context, *type, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-node type. You should provide any of ScType::Node... value as a type");

  case SC_RESULT_ERROR_FULL_MEMORY:
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "Not able to create sc-node due sc-memory is full");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to create sc-node due sc-memory context is not authorized");

  default:
    break;
  }

  return addr;
}

ScAddr ScMemoryContext::CreateLink(ScType const & type /* = ScType::LinkConst */)
{
  CHECK_CONTEXT;

  sc_result result;
  sc_addr const addr = sc_memory_link_new_ext(m_context, *type, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-link type. You should provide any of ScType::Link... value as a type");

  case SC_RESULT_ERROR_FULL_MEMORY:
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "Not able to create sc-link due sc-memory is full");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to create sc-link due sc-memory context is not authorized");

  default:
    break;
  }

  return addr;
}

ScAddr ScMemoryContext::CreateEdge(ScType const & type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
  CHECK_CONTEXT;

  sc_result result;
  sc_addr const addr = sc_memory_arc_new_ext(m_context, *type, *addrBeg, *addrEnd, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified source or target sc-element sc-address is invalid to create sc-connector");

  case SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-connector type. You should provide any of ScType::Edge... value as a type");

  case SC_RESULT_ERROR_FULL_MEMORY:
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "Not able to create sc-connector due sc-memory is full");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to create sc-connector due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to create sc-connector due sc-memory context hasn't write permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_PERMISSIONS_TO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to create sc-connector due sc-memory context hasn't permissions to write permissions");

  default:
    break;
  }

  return addr;
}

ScType ScMemoryContext::GetElementType(ScAddr const & addr) const
{
  CHECK_CONTEXT;

  sc_type type = 0;
  sc_result const result = sc_memory_get_element_type(m_context, *addr, &type);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get sc-type");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Not able to get sc-type due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get sc-type due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return ScType{type};
}

bool ScMemoryContext::SetElementSubtype(ScAddr const & addr, sc_type subtype)
{
  CHECK_CONTEXT;

  sc_result const result = sc_memory_change_element_subtype(m_context, *addr, subtype);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to set sc-type");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Not able to set sc-type due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set sc-type due sc-memory context hasn't write permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

ScAddr ScMemoryContext::GetEdgeSource(ScAddr const & edgeAddr) const
{
  CHECK_CONTEXT;

  ScAddr addr;
  sc_result const result = sc_memory_get_arc_begin(m_context, *edgeAddr, &addr.m_realAddr);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified sc-connector sc-address is invalid to get incident source sc-element");

  case SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element is not sc-connector to get incident source sc-element");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident source sc-element due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident source sc-element due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return addr;
}

ScAddr ScMemoryContext::GetEdgeTarget(ScAddr const & edgeAddr) const
{
  CHECK_CONTEXT;

  ScAddr addr;
  sc_result const result = sc_memory_get_arc_end(m_context, *edgeAddr, &addr.m_realAddr);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified sc-connector sc-address is invalid to get incident target sc-element");

  case SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element is not sc-connector to get incident target sc-element");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident target sc-element due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident target sc-element due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return addr;
}

bool ScMemoryContext::GetEdgeInfo(ScAddr const & edgeAddr, ScAddr & outSourceAddr, ScAddr & outTargetAddr) const
{
  CHECK_CONTEXT;

  sc_result const result =
      sc_memory_get_arc_info(m_context, *edgeAddr, &outSourceAddr.m_realAddr, &outTargetAddr.m_realAddr);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-connector sc-address is invalid to get incident sc-elements");

  case SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element is not sc-connector to get incident sc-elements");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get incident sc-elements due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident sc-elements due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return true;
}

bool ScMemoryContext::SetLinkContent(ScAddr const & addr, ScStreamPtr const & stream, bool isSearchableString)
{
  CHECK_CONTEXT;

  if (!stream || !stream->IsValid())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified stream is invalid to set content");

  sc_result const result = sc_memory_set_link_content_ext(m_context, *addr, stream->m_stream, isSearchableString);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-link sc-address is invalid to set content");

  case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element is not sc-link to set content");

  case SC_RESULT_ERROR_STREAM_IO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-stream data is invalid to set content");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "File memory state is invalid to set content");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Not able to set content due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set content due sc-memory context hasn't erase permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set content due sc-memory context hasn't write permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

ScStreamPtr ScMemoryContext::GetLinkContent(ScAddr const & addr)
{
  CHECK_CONTEXT;

  sc_stream * s = nullptr;
  sc_result const result = sc_memory_get_link_content(m_context, *addr, &s);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-link sc-address is invalid to get content");

  case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element is not sc-link to get content");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "File memory state is invalid to get content");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Not able to get content due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get content due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return std::make_shared<ScStream>(s);
}

void _PushLinkAddr(void * _data, sc_addr const link_addr)
{
  void ** data = ((void **)_data);
  auto * ctx = (sc_memory_context *)data[0];

  if (_sc_memory_context_check_local_and_global_permissions(
          (sc_memory_context_manager *)sc_memory_get_context_manager(), ctx, SC_CONTEXT_PERMISSIONS_READ, link_addr)
      == SC_FALSE)
    return;

  auto * linkAddrList = (ScAddrVector *)data[1];
  linkAddrList->emplace_back(link_addr);
}

#define _MAKE_DATA(_context_ptr, _list_ptr) \
  ({ \
    void ** _data = new void *[2]; \
    _data[0] = _context_ptr; \
    _data[1] = _list_ptr; \
    _data; \
  })

#define _ERASE_DATA(_data) delete[] _data

ScAddrVector ScMemoryContext::FindLinksByContent(ScStreamPtr const & stream)
{
  CHECK_CONTEXT;

  if (!stream || !stream->IsValid())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified stream is invalid to find sc-links by content");

  ScAddrVector linkAddrList;
  void ** data = _MAKE_DATA(&*m_context, &linkAddrList);
  sc_result const result =
      sc_memory_find_links_with_content_string_ext(m_context, stream->m_stream, data, _PushLinkAddr);
  _ERASE_DATA(data);

  switch (result)
  {
  case SC_RESULT_ERROR_STREAM_IO:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-stream data is invalid to find sc-links by content");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "File memory state is invalid to find sc-links by content");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to find sc-links by content due sc-memory context is not authorized");

  default:
    break;
  }

  return linkAddrList;
}

ScAddrVector ScMemoryContext::FindLinksByContentSubstring(ScStreamPtr const & stream, size_t maxLengthToSearchAsPrefix)
{
  CHECK_CONTEXT;

  if (!stream || !stream->IsValid())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified stream is invalid to find sc-links by content substring");

  ScAddrVector linkAddrList;
  void ** data = _MAKE_DATA(&*m_context, &linkAddrList);
  sc_result const result = sc_memory_find_links_by_content_substring_ext(
      m_context, stream->m_stream, maxLengthToSearchAsPrefix, data, _PushLinkAddr);
  _ERASE_DATA(data);

  switch (result)
  {
  case SC_RESULT_ERROR_STREAM_IO:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-stream data is invalid to find sc-links by content substring");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "File memory state is invalid to find sc-links by content substring");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to find sc-links by content substring due sc-memory context is not authorized");

  default:
    break;
  }

  return linkAddrList;
}

void _PushLinkContent(void * data, sc_addr const, sc_char const * link_content)
{
  auto * linkContentList = (std::vector<std::string> *)data;
  linkContentList->emplace_back(link_content);
}

std::vector<std::string> ScMemoryContext::FindLinksContentsByContentSubstring(
    ScStreamPtr const & stream,
    size_t maxLengthToSearchAsPrefix)
{
  CHECK_CONTEXT;

  if (!stream || !stream->IsValid())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified stream is invalid to find contents by content substring");

  std::vector<std::string> linkContentList;
  sc_result const result = sc_memory_find_links_contents_by_content_substring_ext(
      m_context, stream->m_stream, maxLengthToSearchAsPrefix, &linkContentList, _PushLinkContent);

  switch (result)
  {
  case SC_RESULT_ERROR_STREAM_IO:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-stream data is invalid to find contents by content substring");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "File memory state is invalid to find contents by content substring");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to find contents by content substring due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to find contents by content substring due sc-memory context hasn't read permissions");

  default:
    break;
  }

  return linkContentList;
}

bool ScMemoryContext::Save()
{
  CHECK_CONTEXT;
  sc_result const result = sc_memory_save(m_context);
  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to save sc-memory state due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to save sc-memory state due sc-memory context hasn't write permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

ScAddr ScMemoryContext::HelperResolveSystemIdtf(std::string const & sysIdtf, ScType const & type /* = ScType()*/)
{
  CHECK_CONTEXT;

  ScSystemIdentifierQuintuple quintuple;
  HelperResolveSystemIdtf(sysIdtf, type, quintuple);
  return quintuple.addr1;
}

bool ScMemoryContext::HelperResolveSystemIdtf(
    std::string const & sysIdtf,
    ScType const & type,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  CHECK_CONTEXT;

  bool result = HelperFindBySystemIdtf(sysIdtf, outQuintuple);
  if (result)
    return result;

  if (type.IsUnknown())
    return false;

  ScAddr const & resultAddr = CreateNode(type);
  result = HelperSetSystemIdtf(sysIdtf, resultAddr, outQuintuple);
  if (result)
    return result;

  EraseElement(resultAddr);
  outQuintuple =
      (ScSystemIdentifierQuintuple){ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScAddr::Empty};

  return result;
}

bool ScMemoryContext::HelperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr)
{
  ScSystemIdentifierQuintuple quintuple;
  return HelperSetSystemIdtf(sysIdtf, addr, quintuple);
}

bool ScMemoryContext::HelperSetSystemIdtf(
    std::string const & sysIdtf,
    ScAddr const & addr,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  CHECK_CONTEXT;

  sc_system_identifier_fiver quintuple;
  sc_result const result =
      sc_helper_set_system_identifier_ext(m_context, *addr, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &quintuple);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to set system identifier");

  case SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified system identifier is invalid");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "File memory state is invalid to set system identifier");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set system identifier due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to set system identifier due sc-memory context hasn't write permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to set system identifier due sc-memory context hasn't erase permissions");

  default:
    break;
  }

  outQuintuple = (ScSystemIdentifierQuintuple){
      ScAddr(quintuple.addr1),
      ScAddr(quintuple.addr2),
      ScAddr(quintuple.addr3),
      ScAddr(quintuple.addr4),
      ScAddr(quintuple.addr5)};

  return result == SC_RESULT_OK;
}

std::string ScMemoryContext::HelperGetSystemIdtf(ScAddr const & addr)
{
  CHECK_CONTEXT;

  ScAddr idtfLink;
  sc_result result = sc_helper_get_system_identifier_link(m_context, *addr, &idtfLink.m_realAddr);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get system identifier");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get system identifier due sc-memory context is not authorized");

  default:
    break;
  }

  std::string systemIdtf;
  if (result == SC_RESULT_NO)
    return systemIdtf;

  ScStreamPtr stream = GetLinkContent(idtfLink);
  if (stream && stream->IsValid())
  {
    if (ScStreamConverter::StreamToString(stream, systemIdtf))
      return systemIdtf;
  }

  return systemIdtf;
}

bool ScMemoryContext::HelperCheckEdge(ScAddr const & begin, ScAddr end, ScType const & edgeType) const
{
  CHECK_CONTEXT;

  sc_result result;
  sc_bool status = sc_helper_check_arc_ext(m_context, *begin, *end, *edgeType, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to check sc-connector due sc-memory context is not authorized");

  default:
    break;
  }

  return status;
}

bool ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf, ScAddr & outAddr)
{
  CHECK_CONTEXT;

  ScSystemIdentifierQuintuple outQuintuple;
  bool status = HelperFindBySystemIdtf(sysIdtf, outQuintuple);
  outAddr = outQuintuple.addr1;

  return status;
}

ScAddr ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf)
{
  CHECK_CONTEXT;

  ScSystemIdentifierQuintuple outQuintuple;
  HelperFindBySystemIdtf(sysIdtf, outQuintuple);

  return outQuintuple.addr1;
}

bool ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf, ScSystemIdentifierQuintuple & outQuintuple)
{
  CHECK_CONTEXT;

  sc_system_identifier_fiver quintuple;
  sc_result const result = sc_helper_find_element_by_system_identifier_ext(
      m_context, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &quintuple);

  switch (result)
  {
  case SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified system identifier is invalid");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "File memory state is invalid to find sc-element by system identifier");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to find by system identifier due sc-memory context is not authorized");

  default:
    break;
  }

  outQuintuple = (ScSystemIdentifierQuintuple){
      ScAddr(quintuple.addr1),
      ScAddr(quintuple.addr2),
      ScAddr(quintuple.addr3),
      ScAddr(quintuple.addr4),
      ScAddr(quintuple.addr5)};
  return result == SC_RESULT_OK;
}

ScTemplate::Result ScMemoryContext::HelperGenTemplate(
    ScTemplate const & templ,
    ScTemplateResultItem & result,
    ScTemplateParams const & params,
    ScTemplateResultCode * resultCode)
{
  CHECK_CONTEXT;
  return templ.Generate(*this, result, params, resultCode);
}

ScTemplate::Result ScMemoryContext::HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResult & result)
{
  CHECK_CONTEXT;
  return templ.Search(*this, result);
}

void ScMemoryContext::HelperSearchTemplate(
    ScTemplate const & templ,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templ.Search(*this, callback, filterCallback, checkCallback);
}

void ScMemoryContext::HelperSearchTemplate(
    ScTemplate const & templ,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templ.Search(*this, callback, {}, checkCallback);
}

void ScMemoryContext::HelperSmartSearchTemplate(
    ScTemplate const & templ,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templ.Search(*this, callback, filterCallback, checkCallback);
}

void ScMemoryContext::HelperSmartSearchTemplate(
    ScTemplate const & templ,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templ.Search(*this, callback, {}, checkCallback);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(
    ScTemplate & resultTemplate,
    ScAddr const & translatableTemplateAddr,
    ScTemplateParams const & params)
{
  CHECK_CONTEXT;
  resultTemplate.TranslateFrom(*this, translatableTemplateAddr, params);
  return ScTemplate::Result(true);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(
    ScTemplate & resultTemplate,
    std::string const & translatableSCsTemplate)
{
  CHECK_CONTEXT;
  resultTemplate.TranslateFrom(*this, translatableSCsTemplate);
  return ScTemplate::Result(true);
}

void ScMemoryContext::HelperLoadTemplate(
    ScTemplate & translatableTemplate,
    ScAddr & resultTemplateAddr,
    ScTemplateParams const & params)
{
  CHECK_CONTEXT;
  translatableTemplate.TranslateTo(*this, resultTemplateAddr, params);
}

ScMemoryContext::ScMemoryStatistics ScMemoryContext::CalculateStat() const
{
  CHECK_CONTEXT;

  sc_stat stat;
  sc_result const result = sc_memory_stat(m_context, &stat);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get sc-memory statistics state due sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get sc-memory statistics due sc-memory context hasn't read permissions");

  default:
    break;
  }

  ScMemoryStatistics res{};
  res.m_edgesNum = uint32_t(stat.arc_count);
  res.m_linksNum = uint32_t(stat.link_count);
  res.m_nodesNum = uint32_t(stat.node_count);

  return res;
}

SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
