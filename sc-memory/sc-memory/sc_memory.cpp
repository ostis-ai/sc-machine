/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.hpp"

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
  sc_memory_context * context = sc_memory_initialize(&params, &ms_globalContext->m_context);
  if (context == nullptr)
    return false;

  ScAddr initMemoryGeneratedStructureAddr;
  if (params.init_memory_generated_upload)
    initMemoryGeneratedStructureAddr = ms_globalContext->ResolveElementSystemIdentifier(
        params.init_memory_generated_structure, ScType::NodeConstStruct);
  ms_globalContext->m_contextStructureAddr = initMemoryGeneratedStructureAddr;

  ScKeynodes::Initialize(ms_globalContext);

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

  bool result = sc_memory_shutdown(saveState);

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
  , m_contextStructureAddr(other.m_contextStructureAddr)
{
  other.m_context = nullptr;
}

ScMemoryContext & ScMemoryContext::operator=(ScMemoryContext && other) noexcept
{
  if (this == &other)
    return *this;

  m_context = other.m_context;
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

ScAddr ScMemoryContext::GetUser()
{
  CHECK_CONTEXT;
  return sc_memory_context_get_user_addr(m_context);
}

ScAddr ScMemoryContext::GetContextStructure()
{
  CHECK_CONTEXT;
  return m_contextStructureAddr;
}

sc_memory_context * ScMemoryContext::operator*() const
{
  return m_context;
}

sc_memory_context * ScMemoryContext::GetRealContext() const
{
  return m_context;
}

void ScMemoryContext::BeginEventsPending()
{
  CHECK_CONTEXT;
  sc_memory_context_pending_begin(m_context);
}

void ScMemoryContext::EndEventsPending()
{
  CHECK_CONTEXT;
  sc_memory_context_pending_end(m_context);
}

void ScMemoryContext::BeginEventsBlocking()
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

bool ScMemoryContext::IsElement(ScAddr const & elementAddr) const
{
  CHECK_CONTEXT;

  sc_result result;
  bool status = sc_memory_is_element_ext(m_context, *elementAddr, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to check sc-element because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to check sc-element because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return status;
}

size_t ScMemoryContext::GetElementOutgoingArcsCount(ScAddr const & elementAddr) const
{
  CHECK_CONTEXT;

  sc_result result;
  size_t const count = sc_memory_get_element_outgoing_arcs_count(m_context, *elementAddr, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get outgoing sc-arcs count");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get outgoing sc-arcs count because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get outgoing sc-arcs count because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return count;
}

size_t ScMemoryContext::GetElementOutputArcsCount(ScAddr const & elementAddr) const
{
  return GetElementOutgoingArcsCount(elementAddr);
}

size_t ScMemoryContext::GetElementIncomingArcsCount(ScAddr const & elementAddr) const
{
  CHECK_CONTEXT;

  sc_result result;
  size_t const count = sc_memory_get_element_incoming_arcs_count(m_context, *elementAddr, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get incoming sc-arcs count");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incoming sc-arcs count because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incoming sc-arcs count because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return count;
}

size_t ScMemoryContext::GetElementInputArcsCount(ScAddr const & elementAddr) const
{
  return GetElementIncomingArcsCount(elementAddr);
}

bool ScMemoryContext::EraseElement(ScAddr const & elementAddr)
{
  CHECK_CONTEXT;

  sc_result const result = sc_memory_element_free(m_context, *elementAddr);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to erase sc-element because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to erase sc-element because sc-memory context hasn't erase permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_PERMISSIONS_TO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to erase sc-element because sc-memory context hasn't permissions to erase permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

ScAddr ScMemoryContext::GenerateNode(ScType const & nodeType)
{
  CHECK_CONTEXT;

  sc_result result;
  sc_addr const nodeAddr = sc_memory_node_new_ext(m_context, *nodeType, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-node type. You should provide any of ScType::Node... value as a type");

  case SC_RESULT_ERROR_FULL_MEMORY:
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "Not able to create sc-node because sc-memory is full");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to create sc-node because sc-memory context is not authorized");

  default:
    break;
  }

  return nodeAddr;
}

ScAddr ScMemoryContext::CreateNode(ScType const & nodeType)
{
  return GenerateNode(nodeType);
}

ScAddr ScMemoryContext::GenerateLink(ScType const & linkType /* = ScType::LinkConst */)
{
  CHECK_CONTEXT;

  sc_result result;
  sc_addr const linkAddr = sc_memory_link_new_ext(m_context, *linkType, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-link type. You should provide any of ScType::Link... value as a type");

  case SC_RESULT_ERROR_FULL_MEMORY:
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "Not able to create sc-link because sc-memory is full");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to create sc-link because sc-memory context is not authorized");

  default:
    break;
  }

  return linkAddr;
}

ScAddr ScMemoryContext::CreateLink(ScType const & linkType)
{
  return GenerateLink(linkType);
}

ScAddr ScMemoryContext::GenerateConnector(
    ScType const & connectorType,
    ScAddr const & sourceElementAddr,
    ScAddr const & targetElementAddr)
{
  CHECK_CONTEXT;

  sc_result result;
  sc_addr const connectorAddr =
      sc_memory_arc_new_ext(m_context, *connectorType, *sourceElementAddr, *targetElementAddr, &result);

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
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "Not able to create sc-connector because sc-memory is full");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to create sc-connector because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to create sc-connector because sc-memory context hasn't write permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_PERMISSIONS_TO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to create sc-connector because sc-memory context hasn't permissions to write permissions");

  default:
    break;
  }

  return connectorAddr;
}

ScAddr ScMemoryContext::CreateEdge(
    ScType const & connectorType,
    ScAddr const & sourceElementAddr,
    ScAddr const & targetElementAddr)
{
  return GenerateConnector(connectorType, sourceElementAddr, targetElementAddr);
}

ScType ScMemoryContext::GetElementType(ScAddr const & elementAddr) const
{
  CHECK_CONTEXT;

  sc_type elementType = 0;
  sc_result const result = sc_memory_get_element_type(m_context, *elementAddr, &elementType);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get sc-type");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get sc-type because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get sc-type because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return ScType{elementType};
}

bool ScMemoryContext::SetElementSubtype(ScAddr const & elementAddr, ScType newSubtype)
{
  CHECK_CONTEXT;

  sc_result const result = sc_memory_change_element_subtype(m_context, *elementAddr, newSubtype);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to set sc-type");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set sc-type because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set sc-type because sc-memory context hasn't write permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

ScAddr ScMemoryContext::GetArcSourceElement(ScAddr const & arcAddr) const
{
  CHECK_CONTEXT;

  ScAddr sourceElementAddr;
  sc_result const result = sc_memory_get_arc_begin(m_context, *arcAddr, &sourceElementAddr.m_realAddr);

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
        "Not able to get incident source sc-element because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident source sc-element because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return sourceElementAddr;
}

ScAddr ScMemoryContext::GetEdgeSource(ScAddr const & arcAddr) const
{
  return GetArcSourceElement(arcAddr);
}

ScAddr ScMemoryContext::GetArcTargetElement(ScAddr const & arcAddr) const
{
  CHECK_CONTEXT;

  ScAddr targetElementAddr;
  sc_result const result = sc_memory_get_arc_end(m_context, *arcAddr, &targetElementAddr.m_realAddr);

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
        "Not able to get incident target sc-element because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident target sc-element because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return targetElementAddr;
}

ScAddr ScMemoryContext::GetEdgeTarget(ScAddr const & arcAddr) const
{
  return GetArcTargetElement(arcAddr);
}

std::tuple<ScAddr, ScAddr> ScMemoryContext::GetConnectorIncidentElements(ScAddr const & connectorAddr) const
{
  CHECK_CONTEXT;

  ScAddr firstIncidentElementAddr;
  ScAddr secondIncidentElementAddr;
  sc_result const result = sc_memory_get_arc_info(
      m_context, *connectorAddr, &firstIncidentElementAddr.m_realAddr, &secondIncidentElementAddr.m_realAddr);

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
        utils::ExceptionInvalidState,
        "Not able to get incident sc-elements because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get incident sc-elements because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return {firstIncidentElementAddr, secondIncidentElementAddr};
}

bool ScMemoryContext::GetEdgeInfo(
    ScAddr const & connectorAddr,
    ScAddr & outFirstIncidentElementAddr,
    ScAddr & outSecondIncidentElementAddr) const
{
  auto const [firstIncidentElementAddr, secondIncidentElementAddr] = GetConnectorIncidentElements(connectorAddr);
  outFirstIncidentElementAddr = firstIncidentElementAddr;
  outSecondIncidentElementAddr = secondIncidentElementAddr;
  return true;
}

bool ScMemoryContext::SetLinkContent(
    ScAddr const & linkAddr,
    ScStreamPtr const & linkContentStream,
    bool isSearchableString)
{
  CHECK_CONTEXT;

  if (!linkContentStream || !linkContentStream->IsValid())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified stream is invalid to set content");

  sc_result const result =
      sc_memory_set_link_content_ext(m_context, *linkAddr, linkContentStream->m_stream, isSearchableString);

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
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set content because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set content because sc-memory context hasn't erase permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to set content because sc-memory context hasn't write permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

ScStreamPtr ScMemoryContext::GetLinkContent(ScAddr const & linkAddr)
{
  CHECK_CONTEXT;

  sc_stream * linkContentStream = nullptr;
  sc_result const result = sc_memory_get_link_content(m_context, *linkAddr, &linkContentStream);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-link sc-address is invalid to get content");

  case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element is not sc-link to get content");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "File memory state is invalid to get content");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get content because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get content because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return std::make_shared<ScStream>(linkContentStream);
}

void _PushLinkAddr(void * _data, sc_addr const link_addr)
{
  void ** data = ((void **)_data);
  auto * context = (sc_memory_context *)data[0];

  if (_sc_memory_context_check_local_and_global_permissions(
          (sc_memory_context_manager *)sc_memory_get_context_manager(), context, SC_CONTEXT_PERMISSIONS_READ, link_addr)
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

ScAddrVector ScMemoryContext::SearchLinksByContent(ScStreamPtr const & linkContentStream)
{
  CHECK_CONTEXT;

  if (!linkContentStream || !linkContentStream->IsValid())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified stream is invalid to find sc-links by content");

  ScAddrVector linkAddrList;
  void ** data = _MAKE_DATA(&*m_context, &linkAddrList);
  sc_result const result =
      sc_memory_find_links_with_content_string_ext(m_context, linkContentStream->m_stream, data, _PushLinkAddr);
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
        utils::ExceptionInvalidState,
        "Not able to find sc-links by content because sc-memory context is not authorized");

  default:
    break;
  }

  return linkAddrList;
}

ScAddrVector ScMemoryContext::FindLinksByContent(ScStreamPtr const & linkContentStream)
{
  return SearchLinksByContent(linkContentStream);
}

ScAddrVector ScMemoryContext::SearchLinksByContentSubstring(
    ScStreamPtr const & linkContentSubstringStream,
    size_t maxLengthToSearchAsPrefix)
{
  CHECK_CONTEXT;

  if (!linkContentSubstringStream || !linkContentSubstringStream->IsValid())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified stream is invalid to find sc-links by content substring");

  ScAddrVector linkAddrList;
  void ** data = _MAKE_DATA(&*m_context, &linkAddrList);
  sc_result const result = sc_memory_find_links_by_content_substring_ext(
      m_context, linkContentSubstringStream->m_stream, maxLengthToSearchAsPrefix, data, _PushLinkAddr);
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
        "Not able to find sc-links by content substring because sc-memory context is not authorized");

  default:
    break;
  }

  return linkAddrList;
}

ScAddrVector ScMemoryContext::FindLinksByContentSubstring(
    ScStreamPtr const & linkContentSubstringStream,
    size_t maxLengthToSearchAsPrefix)
{
  return SearchLinksByContentSubstring(linkContentSubstringStream, maxLengthToSearchAsPrefix);
}

void _PushLinkContent(void * data, sc_addr const, sc_char const * link_content)
{
  auto * linkContentList = (std::vector<std::string> *)data;
  linkContentList->emplace_back(link_content);
}

std::vector<std::string> ScMemoryContext::SearchLinksContentsByContentSubstring(
    ScStreamPtr const & linkContentSubstringStream,
    size_t maxLengthToSearchAsPrefix)
{
  CHECK_CONTEXT;

  if (!linkContentSubstringStream || !linkContentSubstringStream->IsValid())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified stream is invalid to find contents by content substring");

  std::vector<std::string> linkContentList;
  sc_result const result = sc_memory_find_links_contents_by_content_substring_ext(
      m_context, linkContentSubstringStream->m_stream, maxLengthToSearchAsPrefix, &linkContentList, _PushLinkContent);

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
        "Not able to find contents by content substring because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to find contents by content substring because sc-memory context hasn't read permissions");

  default:
    break;
  }

  return linkContentList;
}

std::vector<std::string> ScMemoryContext::FindLinksContentsByContentSubstring(
    ScStreamPtr const & linkContentSubstringStream,
    size_t maxLengthToSearchAsPrefix)
{
  return SearchLinksContentsByContentSubstring(linkContentSubstringStream, maxLengthToSearchAsPrefix);
}

bool ScMemoryContext::CheckConnector(
    ScAddr const & sourcElementAddr,
    ScAddr const & targetElementAddr,
    ScType const & connectorType) const
{
  CHECK_CONTEXT;

  sc_result result;
  bool status = sc_helper_check_arc_ext(m_context, *sourcElementAddr, *targetElementAddr, *connectorType, &result);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to check sc-connector because sc-memory context is not authorized");

  default:
    break;
  }

  return status;
}

bool ScMemoryContext::HelperCheckEdge(
    ScAddr const & sourcElementAddr,
    ScAddr const & targetElementAddr,
    ScType const & connectorType) const
{
  return CheckConnector(sourcElementAddr, targetElementAddr, connectorType);
}

ScAddr ScMemoryContext::ResolveElementSystemIdentifier(
    std::string const & systemIdentifier,
    ScType const & elementType /* = ScType()*/)
{
  CHECK_CONTEXT;

  ScSystemIdentifierQuintuple quintuple;
  ResolveElementSystemIdentifier(systemIdentifier, elementType, quintuple);
  return quintuple.addr1;
}

ScAddr ScMemoryContext::HelperResolveSystemIdtf(
    std::string const & systemIdentifier,
    ScType const & elementType /* = ScType()*/)
{
  return ResolveElementSystemIdentifier(systemIdentifier, elementType);
}

bool ScMemoryContext::ResolveElementSystemIdentifier(
    std::string const & systemIdentifier,
    ScType const & elementType,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  CHECK_CONTEXT;

  bool result = SearchElementBySystemIdentifier(systemIdentifier, outQuintuple);
  if (result)
    return result;

  if (elementType.IsUnknown())
    return false;

  ScAddr const & resultAddr = GenerateNode(elementType);
  result = SetElementSystemIdentifier(systemIdentifier, resultAddr, outQuintuple);
  if (result)
    return result;

  EraseElement(resultAddr);
  outQuintuple =
      (ScSystemIdentifierQuintuple){ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScAddr::Empty};

  return result;
}

bool ScMemoryContext::HelperResolveSystemIdtf(
    std::string const & systemIdentifier,
    ScType const & elementType,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  return ResolveElementSystemIdentifier(systemIdentifier, elementType, outQuintuple);
}

bool ScMemoryContext::SetElementSystemIdentifier(std::string const & systemIdentifier, ScAddr const & elementAddr)
{
  ScSystemIdentifierQuintuple quintuple;
  return SetElementSystemIdentifier(systemIdentifier, elementAddr, quintuple);
}

bool ScMemoryContext::HelperSetSystemIdtf(std::string const & systemIdentifier, ScAddr const & elementAddr)
{
  return SetElementSystemIdentifier(systemIdentifier, elementAddr);
}

bool ScMemoryContext::SetElementSystemIdentifier(
    std::string const & systemIdentifier,
    ScAddr const & elementAddr,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  CHECK_CONTEXT;

  sc_system_identifier_fiver quintuple;
  sc_result const result = sc_helper_set_system_identifier_ext(
      m_context, *elementAddr, systemIdentifier.c_str(), (sc_uint32)systemIdentifier.size(), &quintuple);

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
        utils::ExceptionInvalidState, "Not able to set system identifier because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to set system identifier because sc-memory context hasn't write permissions");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to set system identifier because sc-memory context hasn't erase permissions");

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

bool ScMemoryContext::HelperSetSystemIdtf(
    std::string const & systemIdentifier,
    ScAddr const & elementAddr,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  return SetElementSystemIdentifier(systemIdentifier, elementAddr, outQuintuple);
}

std::string ScMemoryContext::GetElementSystemIdentifier(ScAddr const & elementAddr)
{
  CHECK_CONTEXT;

  ScAddr identifierLinkAddr;
  sc_result result = sc_helper_get_system_identifier_link(m_context, *elementAddr, &identifierLinkAddr.m_realAddr);

  switch (result)
  {
  case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Specified sc-element sc-address is invalid to get system identifier");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to get system identifier because sc-memory context is not authorized");

  default:
    break;
  }

  std::string systemIdentifier;
  if (result == SC_RESULT_NO)
    return systemIdentifier;

  ScStreamPtr linkContentStream = GetLinkContent(identifierLinkAddr);
  if (linkContentStream && linkContentStream->IsValid())
  {
    if (ScStreamConverter::StreamToString(linkContentStream, systemIdentifier))
      return systemIdentifier;
  }

  return systemIdentifier;
}

std::string ScMemoryContext::HelperGetSystemIdtf(ScAddr const & elementAddr)
{
  return GetElementSystemIdentifier(elementAddr);
}

bool ScMemoryContext::SearchElementBySystemIdentifier(std::string const & systemIdentifier, ScAddr & outAddr)
{
  CHECK_CONTEXT;

  ScSystemIdentifierQuintuple outQuintuple;
  bool status = SearchElementBySystemIdentifier(systemIdentifier, outQuintuple);
  outAddr = outQuintuple.addr1;

  return status;
}

bool ScMemoryContext::HelperFindBySystemIdtf(std::string const & systemIdentifier, ScAddr & outAddr)
{
  return SearchElementBySystemIdentifier(systemIdentifier, outAddr);
}

ScAddr ScMemoryContext::SearchElementBySystemIdentifier(std::string const & systemIdentifier)
{
  CHECK_CONTEXT;

  ScSystemIdentifierQuintuple outQuintuple;
  SearchElementBySystemIdentifier(systemIdentifier, outQuintuple);

  return outQuintuple.addr1;
}

ScAddr ScMemoryContext::HelperFindBySystemIdtf(std::string const & systemIdentifier)
{
  return SearchElementBySystemIdentifier(systemIdentifier);
}

bool ScMemoryContext::SearchElementBySystemIdentifier(
    std::string const & systemIdentifier,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  CHECK_CONTEXT;

  sc_system_identifier_fiver quintuple;
  sc_result const result = sc_helper_find_element_by_system_identifier_ext(
      m_context, systemIdentifier.c_str(), (sc_uint32)systemIdentifier.size(), &quintuple);

  switch (result)
  {
  case SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified system identifier is invalid");

  case SC_RESULT_ERROR_FILE_MEMORY_IO:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "File memory state is invalid to find sc-element by system identifier");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to find by system identifier because sc-memory context is not authorized");

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

bool ScMemoryContext::HelperFindBySystemIdtf(
    std::string const & systemIdentifier,
    ScSystemIdentifierQuintuple & outQuintuple)
{
  return SearchElementBySystemIdentifier(systemIdentifier, outQuintuple);
}

void ScMemoryContext::GenerateByTemplate(
    ScTemplate const & templateToGenerate,
    ScTemplateResultItem & result,
    ScTemplateParams const & params)
{
  CHECK_CONTEXT;
  templateToGenerate.Generate(*this, result, params, nullptr);
}

ScTemplate::Result ScMemoryContext::HelperGenTemplate(
    ScTemplate const & templateToGenerate,
    ScTemplateResultItem & result,
    ScTemplateParams const & params,
    ScTemplateResultCode *)
{
  GenerateByTemplate(templateToGenerate, result, params);
  return ScTemplate::Result(true);
}

ScTemplate::Result ScMemoryContext::SearchByTemplate(ScTemplate const & templateToFind, ScTemplateSearchResult & result)
{
  CHECK_CONTEXT;
  return templateToFind.Search(*this, result);
}

ScTemplate::Result ScMemoryContext::HelperSearchTemplate(
    ScTemplate const & templateToFind,
    ScTemplateSearchResult & result)
{
  return SearchByTemplate(templateToFind, result);
}

void ScMemoryContext::SearchByTemplate(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templateToFind.Search(*this, callback, filterCallback, checkCallback);
}

void ScMemoryContext::HelperSearchTemplate(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  SearchByTemplate(templateToFind, callback, filterCallback, checkCallback);
}

void ScMemoryContext::SearchByTemplate(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templateToFind.Search(*this, callback, {}, checkCallback);
}

void ScMemoryContext::HelperSearchTemplate(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  SearchByTemplate(templateToFind, callback, checkCallback);
}

void ScMemoryContext::SearchByTemplateWithControl(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templateToFind.Search(*this, callback, filterCallback, checkCallback);
}

void ScMemoryContext::HelperSmartSearchTemplate(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  SearchByTemplateWithControl(templateToFind, callback, filterCallback, checkCallback);
}

void ScMemoryContext::SearchByTemplateWithControl(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  CHECK_CONTEXT;
  templateToFind.Search(*this, callback, {}, checkCallback);
}

void ScMemoryContext::HelperSmartSearchTemplate(
    ScTemplate const & templateToFind,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultCheckCallback const & checkCallback)
{
  SearchByTemplateWithControl(templateToFind, callback, checkCallback);
}

void ScMemoryContext::BuildTemplate(
    ScTemplate & resultTemplate,
    ScAddr const & translatableTemplateAddr,
    ScTemplateParams const & params)
{
  CHECK_CONTEXT;
  resultTemplate.TranslateFrom(*this, translatableTemplateAddr, params);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(
    ScTemplate & resultTemplate,
    ScAddr const & translatableTemplateAddr,
    ScTemplateParams const & params)
{
  BuildTemplate(resultTemplate, translatableTemplateAddr, params);
  return ScTemplate::Result(true);
}

void ScMemoryContext::BuildTemplate(ScTemplate & resultTemplate, std::string const & translatableSCsTemplate)
{
  CHECK_CONTEXT;
  resultTemplate.TranslateFrom(*this, translatableSCsTemplate);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(
    ScTemplate & resultTemplate,
    std::string const & translatableSCsTemplate)
{
  BuildTemplate(resultTemplate, translatableSCsTemplate);
  return ScTemplate::Result(true);
}

void ScMemoryContext::LoadTemplate(
    ScTemplate & translatableTemplate,
    ScAddr & resultTemplateAddr,
    ScTemplateParams const & params)
{
  CHECK_CONTEXT;
  translatableTemplate.TranslateTo(*this, resultTemplateAddr, params);
}

ScMemoryContext::ScMemoryStatistics ScMemoryContext::CalculateStatistics() const
{
  CHECK_CONTEXT;

  sc_stat stat;
  sc_result const result = sc_memory_stat(m_context, &stat);

  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get sc-memory statistics state because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get sc-memory statistics because sc-memory context hasn't read permissions");

  default:
    break;
  }

  ScMemoryStatistics statistics{};
  statistics.m_edgesNum = uint32_t(stat.arc_count);
  statistics.m_linksNum = uint32_t(stat.link_count);
  statistics.m_nodesNum = uint32_t(stat.node_count);

  return statistics;
}

ScMemoryContext::ScMemoryStatistics ScMemoryContext::CalculateStat() const
{
  return CalculateStatistics();
}

bool ScMemoryContext::Save()
{
  CHECK_CONTEXT;
  sc_result const result = sc_memory_save(m_context);
  switch (result)
  {
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not able to save sc-memory state because sc-memory context is not authorized");

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to save sc-memory state because sc-memory context hasn't write permissions");

  default:
    break;
  }

  return result == SC_RESULT_OK;
}

SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
