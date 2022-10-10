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

extern "C"
{
#include <glib.h>
}

#define SC_BOOL(x) (x) ? SC_TRUE : SC_FALSE

namespace
{
GMutex gContextMutex;
struct ContextMutexLock
{
  ContextMutexLock()
  {
    g_mutex_lock(&gContextMutex);
  }
  ~ContextMutexLock()
  {
    g_mutex_unlock(&gContextMutex);
  }
};

bool gIsLogMuted = false;

void _logPrintHandler(gchar const * log_domain, GLogLevelFlags log_level, gchar const * message, gpointer user_data)
{
  if (gIsLogMuted)
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

unsigned int gContextCounter;

}  // namespace

// ------------------

sc_memory_context * ScMemory::ms_globalContext = nullptr;
ScMemory::MemoryContextList ScMemory::ms_contexts;

bool ScMemory::Initialize(sc_memory_params const & params)
{
  std::srand(unsigned(std::time(nullptr)));
  gContextCounter = 0;

  g_log_set_default_handler(_logPrintHandler, nullptr);

  ms_globalContext = sc_memory_initialize(&params);
  if (ms_globalContext == nullptr)
    return false;

  ScKeynodes::Init();
  ScAgentInit(true);

  SC_LOG_INFO("Memory initialized");

  utils::ScLog::SetUp(params.log_type, params.log_file, params.log_level);

  return (ms_globalContext != nullptr);
}

bool ScMemory::IsInitialized()
{
  return ms_globalContext != nullptr;
}

void ScMemory::Shutdown(bool saveState /* = true */)
{
  utils::ScLog::SetUp("Console", "", "Info");

  sc_memory_shutdown_ext();

  ScKeynodes::Shutdown();

  if (!ms_contexts.empty())
  {
    std::stringstream description;
    description << "There are " << ms_contexts.size() << " contexts, wasn't destroyed, before Memory::shutdown:";
    for (auto const * ctx : ms_contexts)
      description << "\t\n" << ctx->GetName();

    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, description.str());
  }

  sc_memory_shutdown(SC_BOOL(saveState));
  ms_globalContext = nullptr;

  g_log_set_default_handler(g_log_default_handler, nullptr);
}

void ScMemory::LogMute()
{
  gIsLogMuted = true;
  utils::ScLog::GetInstance()->SetMuted(true);
}

void ScMemory::LogUnmute()
{
  gIsLogMuted = false;
  utils::ScLog::GetInstance()->SetMuted(false);
}

void ScMemory::RegisterContext(ScMemoryContext const * ctx)
{
  assert(!HasMemoryContext(ctx));

  ContextMutexLock lock;
  ms_contexts.push_back(ctx);
}

void ScMemory::UnregisterContext(ScMemoryContext const * ctx)
{
  assert(HasMemoryContext(ctx));

  ContextMutexLock lock;
  for (auto it = ms_contexts.begin(); it != ms_contexts.end(); ++it)
  {
    if (*it == ctx)
    {
      ms_contexts.erase(it);
      return;
    }
  }
}

bool ScMemory::HasMemoryContext(ScMemoryContext const * ctx)
{
  ContextMutexLock lock;
  for (auto const it : ms_contexts)
  {
    if (it == ctx)
      return true;
  }
  return false;
}

// ---------------

ScMemoryContext::ScMemoryContext(sc_uint8 accessLevels, std::string const & name)
  : m_context(nullptr)
{
  m_context = sc_memory_context_new(accessLevels);
  if (name.empty())
  {
    std::stringstream ss;
    ss << "Context_" << gContextCounter;
    m_name = ss.str();
  }
  else
  {
    m_name = name;
  }

  ScMemory::RegisterContext(this);
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
    ScMemory::UnregisterContext(this);

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
  SC_ASSERT(IsValid(), ());
  return (sc_memory_is_element(m_context, *addr) == SC_TRUE);
}

bool ScMemoryContext::EraseElement(ScAddr const & addr)
{
  SC_ASSERT(IsValid(), ());
  return sc_memory_element_free(m_context, *addr) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::CreateNode(ScType const & type)
{
  SC_ASSERT(IsValid(), ());
  return ScAddr(sc_memory_node_new(m_context, *type));
}

ScAddr ScMemoryContext::CreateLink(ScType const & type /* = ScType::LinkConst */)
{
  SC_ASSERT(type == ScType::LinkConst || type == ScType::LinkVar, ());
  SC_ASSERT(IsValid(), ());
  return ScAddr(sc_memory_link_new2(m_context, type.IsConst()));
}

ScAddr ScMemoryContext::CreateArc(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
  return CreateEdge(ScType(type), addrBeg, addrEnd);
}

ScAddr ScMemoryContext::CreateEdge(ScType const & type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
  SC_ASSERT(IsValid(), ());
  return ScAddr(sc_memory_arc_new(m_context, *type, *addrBeg, *addrEnd));
}

ScType ScMemoryContext::GetElementType(ScAddr const & addr) const
{
  SC_ASSERT(IsValid(), ());
  sc_type type = 0;
  return (sc_memory_get_element_type(m_context, *addr, &type) == SC_RESULT_OK) ? ScType(type) : ScType(0);
}

bool ScMemoryContext::SetElementSubtype(ScAddr const & addr, sc_type subtype)
{
  SC_ASSERT(IsValid(), ());
  return sc_memory_change_element_subtype(m_context, *addr, subtype) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::GetEdgeSource(ScAddr const & edgeAddr) const
{
  SC_ASSERT(IsValid(), ());
  ScAddr addr;
  if (sc_memory_get_arc_begin(m_context, *edgeAddr, &addr.m_realAddr) != SC_RESULT_OK)
    addr.Reset();

  return addr;
}

ScAddr ScMemoryContext::GetEdgeTarget(ScAddr const & edgeAddr) const
{
  SC_ASSERT(IsValid(), ());
  ScAddr addr;
  if (sc_memory_get_arc_end(m_context, *edgeAddr, &addr.m_realAddr) != SC_RESULT_OK)
    addr.Reset();

  return addr;
}

bool ScMemoryContext::GetEdgeInfo(ScAddr const & edgeAddr, ScAddr & outSourceAddr, ScAddr & outTargetAddr) const
{
  SC_ASSERT(IsValid(), ());
  if (sc_memory_get_arc_info(m_context, *edgeAddr, &outSourceAddr.m_realAddr, &outTargetAddr.m_realAddr) !=
      SC_RESULT_OK)
  {
    outSourceAddr.Reset();
    outTargetAddr.Reset();

    return false;
  }

  return true;
}

ScAddr ScMemoryContext::GetArcBegin(ScAddr const & arcAddr) const
{
  return GetEdgeSource(arcAddr);
}

ScAddr ScMemoryContext::GetArcEnd(ScAddr const & arcAddr) const
{
  return GetEdgeTarget(arcAddr);
}

bool ScMemoryContext::SetLinkContent(ScAddr const & addr, ScStreamPtr const & stream)
{
  SC_ASSERT(IsValid(), ());
  SC_ASSERT(stream, ());
  return sc_memory_set_link_content(m_context, *addr, stream->m_stream) == SC_RESULT_OK;
}

ScStreamPtr ScMemoryContext::GetLinkContent(ScAddr const & addr)
{
  SC_ASSERT(IsValid(), ());

  sc_stream * s = nullptr;
  if (sc_memory_get_link_content(m_context, *addr, &s) != SC_RESULT_OK || s == nullptr)
    return {};

  return std::make_shared<ScStream>(s);
}

bool ScMemoryContext::FindLinksByContent(ScStreamPtr const & stream, ScAddrVector & found)
{
  found = FindLinksByContent(stream);
  return !found.empty();
}

ScAddrVector ScMemoryContext::FindLinksByContent(ScStreamPtr const & stream)
{
  SC_ASSERT(IsValid(), ());
  ScAddrVector contents;

  sc_addr * result = nullptr;
  sc_uint32 resultCount = 0;

  sc_stream * str = stream->m_stream;
  if (sc_memory_find_links_with_content(m_context, str, &result, &resultCount) == SC_RESULT_OK)
  {
    for (sc_uint32 i = 0; i < resultCount; ++i)
      contents.push_back(ScAddr(result[i]));

    if (result)
      sc_memory_free_buff(result);
  }

  return contents;
}

ScAddrVector ScMemoryContext::FindLinksByContentSubstring(ScStreamPtr const & stream, size_t maxLengthToSearchAsPrefix)
{
  SC_ASSERT(IsValid(), ());
  ScAddrVector contents;

  sc_addr * result = nullptr;
  sc_uint32 resultCount = 0;

  sc_stream * str = stream->m_stream;
  if (sc_memory_find_links_by_content_substring(m_context, str, &result, &resultCount, maxLengthToSearchAsPrefix) ==
      SC_RESULT_OK)
  {
    for (sc_uint32 i = 0; i < resultCount; ++i)
      contents.push_back(ScAddr(result[i]));

    if (result)
      sc_memory_free_buff(result);
  }

  return contents;
}

std::vector<std::string> ScMemoryContext::FindLinksContentsByContentSubstring(
    ScStreamPtr const & stream,
    size_t maxLengthToSearchAsPrefix)
{
  SC_ASSERT(IsValid(), ());
  std::vector<std::string> contents;

  sc_char ** result = nullptr;
  sc_uint32 resultCount = 0;

  sc_stream * str = stream->m_stream;
  if (sc_memory_find_links_contents_by_content_substring(
          m_context, str, &result, &resultCount, maxLengthToSearchAsPrefix) == SC_RESULT_OK)
  {
    for (sc_uint32 i = 0; i < resultCount; ++i)
    {
      contents.emplace_back(result[i]);
      sc_memory_free_buff(result[i]);
    }

    if (result)
      sc_memory_free_buff(result);
  }

  return contents;
}

bool ScMemoryContext::Save()
{
  SC_ASSERT(IsValid(), ());
  return (sc_memory_save(m_context) == SC_RESULT_OK);
}

bool ScMemoryContext::HelperResolveSystemIdtf(
    std::string const & sysIdtf,
    ScAddr & outAddr,
    ScType const & type /* = ScType()*/)
{
  outAddr = HelperResolveSystemIdtf(sysIdtf, type);
  return outAddr.IsValid();
}

ScAddr ScMemoryContext::HelperResolveSystemIdtf(std::string const & sysIdtf, ScType const & type /* = ScType()*/)
{
  SC_ASSERT(IsValid(), ());
  ScAddr resultAddr = HelperFindBySystemIdtf(sysIdtf);
  if (!resultAddr.IsValid() && !type.IsUnknown())
  {
    if (!type.IsNode())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You should provide any of ScType::Node... value as a type");
    }

    resultAddr = CreateNode(type);
    if (resultAddr.IsValid())
    {
      bool isSuccess = HelperSetSystemIdtf(sysIdtf, resultAddr);
      SC_ASSERT(isSuccess, ());
    }
  }
  return resultAddr;
}

bool ScMemoryContext::HelperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr)
{
  SC_ASSERT(IsValid(), ());
  return (
      sc_helper_set_system_identifier(m_context, *addr, sysIdtf.c_str(), (sc_uint32)sysIdtf.size()) == SC_RESULT_OK);
}

std::string ScMemoryContext::HelperGetSystemIdtf(ScAddr const & addr)
{
  SC_ASSERT(IsValid(), ());
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

bool ScMemoryContext::HelperCheckArc(ScAddr const & begin, ScAddr end, sc_type arcType)
{
  return HelperCheckEdge(begin, end, ScType(arcType));
}

bool ScMemoryContext::HelperCheckEdge(ScAddr const & begin, ScAddr end, ScType const & edgeType)
{
  SC_ASSERT(IsValid(), ());
  return (sc_helper_check_arc(m_context, *begin, *end, *edgeType) == SC_RESULT_OK);
}

bool ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf, ScAddr & outAddr)
{
  SC_ASSERT(IsValid(), ());
  return (
      sc_helper_find_element_by_system_identifier(
          m_context, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &outAddr.m_realAddr) == SC_RESULT_OK);
}

ScAddr ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf)
{
  ScAddr result;
  SC_ASSERT(IsValid(), ());
  sc_helper_find_element_by_system_identifier(
      m_context, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &result.m_realAddr);
  return result;
}

ScTemplate::Result ScMemoryContext::HelperGenTemplate(
    ScTemplate const & templ,
    ScTemplateGenResult & result,
    ScTemplateParams const & params,
    ScTemplateResultCode * resultCode)
{
  return templ.Generate(*this, result, params, resultCode);
}

ScTemplate::Result ScMemoryContext::HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResult & result)
{
  return templ.Search(*this, result);
}

ScTemplate::Result ScMemoryContext::HelperSearchTemplateInStruct(
    ScTemplate const & templ,
    ScAddr const & scStruct,
    ScTemplateSearchResult & result)
{
  return templ.SearchInStruct(*this, scStruct, result);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(
    ScTemplate & templ,
    ScAddr const & templAddr,
    ScTemplateParams const & params)
{
  return templ.FromScTemplate(*this, templAddr, params);
}

ScTemplate::Result ScMemoryContext::HelperBuildTemplate(ScTemplate & templ, std::string const & scsText)
{
  return templ.FromScs(*this, scsText);
}

ScMemoryContext::Stat ScMemoryContext::CalculateStat() const
{
  sc_stat stat;
  sc_memory_stat(m_context, &stat);

  Stat res{};
  res.m_edgesNum = uint32_t(stat.arc_count);
  res.m_linksNum = uint32_t(stat.link_count);
  res.m_nodesNum = uint32_t(stat.node_count);

  return res;
}
