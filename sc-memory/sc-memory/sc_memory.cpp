/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.hpp"
#include "sc_keynodes.hpp"
#include "sc_utils.hpp"
#include "sc_stream.hpp"

#include "kpm/sc_agent.hpp"

#include "utils/sc_log.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utility>

extern "C"
{
#include "glib.h"
}

namespace
{

bool isLogMuted = false;

void _logPrintHandler(
    sc_char const * log_domain,
    GLogLevelFlags log_level,
    sc_char const * message,
    sc_pointer user_data)
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

#define CHECK_CONTEXT SC_ASSERT(IsValid(), "Used context is invalid. Make sure that it's initialized")

}  // namespace

// ------------------

sc_memory_context * ScMemory::ms_globalContext = nullptr;

bool ScMemory::Initialize(sc_memory_params const & params)
{
  std::srand(unsigned(std::time(nullptr)));

  g_log_set_default_handler(_logPrintHandler, nullptr);

  ms_globalContext = sc_memory_initialize(&params);
  if (ms_globalContext == nullptr)
    return false;

  ScKeynodes::Init(
      false, params.init_memory_generated_upload ? params.init_memory_generated_structure : (sc_char *)null_ptr);
  ScAgentInit(true);

  utils::ScLog::SetUp(params.log_type, params.log_file, params.log_level);

  return ms_globalContext != nullptr;
}

bool ScMemory::IsInitialized()
{
  return ms_globalContext != nullptr;
}

bool ScMemory::Shutdown(bool saveState /* = true */)
{
  utils::ScLog::SetUp("Console", "", "Info");

  ScKeynodes::Shutdown();

  sc_bool result = sc_memory_shutdown(saveState);

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

ScMemoryContext::ScMemoryContext(sc_uint8 accessLevels, std::string name)
  : m_context(nullptr)
  , m_name(std::move(name))
{
  m_context = sc_memory_context_new(accessLevels);
}

ScMemoryContext::ScMemoryContext(std::string const & name)
  : ScMemoryContext(sc_access_lvl_make_min, name)
{
}

ScMemoryContext::~ScMemoryContext()
{
  Destroy();
}

void ScMemoryContext::Destroy()
{
  if (m_context)
  {
    sc_memory_context_free(m_context);
    m_context = nullptr;
  }
}

void ScMemoryContext::BeingEventsPending()
{
  sc_memory_context_pending_begin(m_context);
}

void ScMemoryContext::EndEventsPending()
{
  sc_memory_context_pending_end(m_context);
}

bool ScMemoryContext::IsValid() const
{
  return m_context != nullptr;
}

bool ScMemoryContext::IsElement(ScAddr const & addr) const
{
  CHECK_CONTEXT;
  return sc_memory_is_element(m_context, *addr) == SC_TRUE;
}

size_t ScMemoryContext::GetElementOutputArcsCount(ScAddr const & addr) const
{
  CHECK_CONTEXT;
  return sc_memory_get_element_output_arcs_count(m_context, *addr);
}

size_t ScMemoryContext::GetElementInputArcsCount(ScAddr const & addr) const
{
  CHECK_CONTEXT;
  return sc_memory_get_element_input_arcs_count(m_context, *addr);
}

bool ScMemoryContext::EraseElement(ScAddr const & addr)
{
  CHECK_CONTEXT;
  return sc_memory_element_free(m_context, *addr) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::CreateNode(ScType const & type)
{
  CHECK_CONTEXT;
  if (type.IsEdge())  // needed to create ScType::Unknown, ScType::Const... elements
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-node type. You should provide any of ScType::Node... value as a type");

  return sc_memory_node_new(m_context, *type);
}

ScAddr ScMemoryContext::CreateLink(ScType const & type /* = ScType::LinkConst */)
{
  CHECK_CONTEXT;
  if (!type.IsLink())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-link type. You should provide any of ScType::Link... value as a type");

  return sc_memory_link_new2(m_context, type);
}

ScAddr ScMemoryContext::CreateEdge(ScType const & type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
  CHECK_CONTEXT;
  if (!type.IsEdge())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Specified type must be sc-connector type. You should provide any of ScType::Edge... value as a type");

  return sc_memory_arc_new(m_context, *type, *addrBeg, *addrEnd);
}

ScType ScMemoryContext::GetElementType(ScAddr const & addr) const
{
  CHECK_CONTEXT;
  sc_type type = 0;
  return sc_memory_get_element_type(m_context, *addr, &type) == SC_RESULT_OK ? ScType(type) : ScType(0);
}

bool ScMemoryContext::SetElementSubtype(ScAddr const & addr, sc_type subtype)
{
  CHECK_CONTEXT;
  return sc_memory_change_element_subtype(m_context, *addr, subtype) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::GetEdgeSource(ScAddr const & edgeAddr) const
{
  CHECK_CONTEXT;

  ScAddr addr;
  if (sc_memory_get_arc_begin(m_context, *edgeAddr, &addr.m_realAddr) != SC_RESULT_OK)
    addr.Reset();

  return addr;
}

ScAddr ScMemoryContext::GetEdgeTarget(ScAddr const & edgeAddr) const
{
  CHECK_CONTEXT;

  ScAddr addr;
  if (sc_memory_get_arc_end(m_context, *edgeAddr, &addr.m_realAddr) != SC_RESULT_OK)
    addr.Reset();

  return addr;
}

bool ScMemoryContext::GetEdgeInfo(ScAddr const & edgeAddr, ScAddr & outSourceAddr, ScAddr & outTargetAddr) const
{
  CHECK_CONTEXT;

  if (sc_memory_get_arc_info(m_context, *edgeAddr, &outSourceAddr.m_realAddr, &outTargetAddr.m_realAddr) !=
      SC_RESULT_OK)
  {
    outSourceAddr.Reset();
    outTargetAddr.Reset();

    return false;
  }

  return true;
}

bool ScMemoryContext::SetLinkContent(ScAddr const & addr, ScStreamPtr const & stream, bool isSearchableString)
{
  CHECK_CONTEXT;
  if (!stream || !stream->IsValid())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified stream is invalid");

  sc_result result = sc_memory_set_link_content_ext(m_context, *addr, stream->m_stream, isSearchableString);

  switch (result)
  {
    case SC_RESULT_ERROR_ADDR_IS_NOT_VALID:
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-address is invalid to set content");

    case SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK:
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified sc-element is not sc-link to set content");

    default:
      break;
  }

  return result == SC_RESULT_OK;
}

ScStreamPtr ScMemoryContext::GetLinkContent(ScAddr const & addr)
{
  CHECK_CONTEXT;

  sc_stream * s = nullptr;
  if (sc_memory_get_link_content(m_context, *addr, &s) != SC_RESULT_OK && s == nullptr)
    return std::make_shared<ScStream>(nullptr);

  return std::make_shared<ScStream>(s);
}

ScAddrVector ScMemoryContext::FindLinksByContent(ScStreamPtr const & stream)
{
  CHECK_CONTEXT;

  ScAddrVector contents;
  sc_list * result = nullptr;

  sc_stream * str = stream->m_stream;
  if (sc_memory_find_links_with_content_string(m_context, str, &result) == SC_RESULT_OK)
  {
    sc_iterator * it = sc_list_iterator(result);
    while (sc_iterator_next(it))
    {
      auto addr_hash = (sc_addr_hash)sc_iterator_get(it);
      contents.emplace_back(addr_hash);
    }
    sc_iterator_destroy(it);
  }
  sc_list_destroy(result);

  return contents;
}

ScAddrVector ScMemoryContext::FindLinksByContentSubstring(ScStreamPtr const & stream, size_t maxLengthToSearchAsPrefix)
{
  CHECK_CONTEXT;

  ScAddrVector contents;
  sc_list * result = nullptr;

  sc_stream * str = stream->m_stream;
  if (sc_memory_find_links_by_content_substring(m_context, str, &result, maxLengthToSearchAsPrefix) == SC_RESULT_OK)
  {
    sc_iterator * it = sc_list_iterator(result);
    while (sc_iterator_next(it))
    {
      auto addr_hash = (sc_addr_hash)sc_iterator_get(it);
      contents.emplace_back(addr_hash);
    }
    sc_iterator_destroy(it);
  }
  sc_list_destroy(result);

  return contents;
}

std::vector<std::string> ScMemoryContext::FindLinksContentsByContentSubstring(
    ScStreamPtr const & stream,
    size_t maxLengthToSearchAsPrefix)
{
  CHECK_CONTEXT;

  std::vector<std::string> contents;
  sc_list * result = nullptr;

  sc_stream * str = stream->m_stream;
  if (sc_memory_find_links_contents_by_content_substring(m_context, str, &result, maxLengthToSearchAsPrefix) ==
      SC_RESULT_OK)
  {
    sc_iterator * it = sc_list_iterator(result);
    while (sc_iterator_next(it))
    {
      auto string = (sc_char *)sc_iterator_get(it);
      contents.emplace_back(string);
      free(string);
    }
    sc_iterator_destroy(it);
  }
  sc_list_destroy(result);

  return contents;
}

bool ScMemoryContext::Save()
{
  CHECK_CONTEXT;
  return sc_memory_save(m_context) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::HelperResolveSystemIdtf(std::string const & sysIdtf, ScType const & type /* = ScType()*/)
{
  CHECK_CONTEXT;

  ScSystemIdentifierFiver outFiver;
  HelperResolveSystemIdtf(sysIdtf, type, outFiver);
  return outFiver.addr1;
}

bool ScMemoryContext::HelperResolveSystemIdtf(
    std::string const & sysIdtf,
    ScType const & type,
    ScSystemIdentifierFiver & outFiver)
{
  CHECK_CONTEXT;

  bool result = HelperFindBySystemIdtf(sysIdtf, outFiver);
  if (!result && !type.IsUnknown())
  {
    if (!type.IsNode())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified type must be sc-node type. You should provide any of ScType::Node... value as a type");

    ScAddr const & resultAddr = CreateNode(type);
    if (resultAddr.IsValid())
    {
      result = HelperSetSystemIdtf(sysIdtf, resultAddr, outFiver);
      if (!result)
      {
        EraseElement(resultAddr);
        outFiver = (ScSystemIdentifierFiver){ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScAddr::Empty};
      }
    }
  }

  return result;
}

bool ScMemoryContext::HelperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr)
{
  CHECK_CONTEXT;
  return sc_helper_set_system_identifier(m_context, *addr, sysIdtf.c_str(), (sc_uint32)sysIdtf.size()) == SC_RESULT_OK;
}

bool ScMemoryContext::HelperSetSystemIdtf(
    std::string const & sysIdtf,
    ScAddr const & addr,
    ScSystemIdentifierFiver & outFiver)
{
  CHECK_CONTEXT;

  sc_system_identifier_fiver fiver;
  bool status = sc_helper_set_system_identifier_ext(
                    m_context, *addr, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &fiver) == SC_RESULT_OK;
  outFiver = (ScSystemIdentifierFiver){
      ScAddr(fiver.addr1), ScAddr(fiver.addr2), ScAddr(fiver.addr3), ScAddr(fiver.addr4), ScAddr(fiver.addr5)};
  return status;
}

std::string ScMemoryContext::HelperGetSystemIdtf(ScAddr const & addr)
{
  CHECK_CONTEXT;

  ScAddr idtfLink;
  if (sc_helper_get_system_identifier_link(m_context, *addr, &idtfLink.m_realAddr) == SC_RESULT_OK)
  {
    if (idtfLink.IsValid())
    {
      ScStreamPtr stream = GetLinkContent(idtfLink);
      if (stream)
      {
        std::string result;
        if (ScStreamConverter::StreamToString(stream, result))
          return result;
      }
    }
  }

  return {};
}

bool ScMemoryContext::HelperCheckEdge(ScAddr const & begin, ScAddr end, ScType const & edgeType)
{
  CHECK_CONTEXT;
  return sc_helper_check_arc(m_context, *begin, *end, *edgeType) == SC_RESULT_OK;
}

bool ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf, ScAddr & outAddr)
{
  CHECK_CONTEXT;
  return sc_helper_find_element_by_system_identifier(
             m_context, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &outAddr.m_realAddr) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf)
{
  CHECK_CONTEXT;

  ScAddr result;
  sc_helper_find_element_by_system_identifier(
      m_context, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &result.m_realAddr);
  return result;
}

bool ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf, ScSystemIdentifierFiver & outFiver)
{
  CHECK_CONTEXT;

  sc_system_identifier_fiver fiver;
  bool status = sc_helper_find_element_by_system_identifier_ext(
                    m_context, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &fiver) == SC_RESULT_OK;
  outFiver = (ScSystemIdentifierFiver){
      ScAddr(fiver.addr1), ScAddr(fiver.addr2), ScAddr(fiver.addr3), ScAddr(fiver.addr4), ScAddr(fiver.addr5)};
  return status;
}

ScTemplate::Result ScMemoryContext::HelperGenTemplate(
    ScTemplate const & templ,
    ScTemplateGenResult & result,
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

ScTemplate::Result ScMemoryContext::HelperSearchTemplateInStruct(
    ScTemplate const & templ,
    ScAddr const & scStruct,
    ScTemplateSearchResult & result)
{
  CHECK_CONTEXT;
  return templ.SearchInStruct(*this, scStruct, result);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(
    ScTemplate & templ,
    ScAddr const & templAddr,
    ScTemplateParams const & params)
{
  CHECK_CONTEXT;
  return templ.FromScTemplate(*this, templAddr, params);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(ScTemplate & templ, std::string const & scsText)
{
  CHECK_CONTEXT;
  return templ.FromScs(*this, scsText);
}

ScMemoryContext::ScMemoryStatistics ScMemoryContext::CalculateStat() const
{
  CHECK_CONTEXT;

  sc_stat stat;
  sc_memory_stat(m_context, &stat);

  ScMemoryStatistics res{};
  res.m_edgesNum = uint32_t(stat.arc_count);
  res.m_linksNum = uint32_t(stat.link_count);
  res.m_nodesNum = uint32_t(stat.node_count);

  return res;
}
