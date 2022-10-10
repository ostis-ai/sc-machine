/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-core/sc_memory_headers.h"
#include "sc-core/sc_helper.h"
}

#include "sc_addr.hpp"
#include "sc_event.hpp"
#include "sc_iterator.hpp"
#include "sc_stream.hpp"
#include "sc_template.hpp"
#include "sc_type.hpp"

class ScMemoryContext;

class ScMemory
{
  friend class ScMemoryContext;

public:
  //! Returns true, on memory initialized; otherwise returns false
  _SC_EXTERN static bool Initialize(sc_memory_params const & params);
  _SC_EXTERN static bool IsInitialized();
  _SC_EXTERN static void Shutdown(bool saveState = true);

  _SC_EXTERN static void LogMute();
  _SC_EXTERN static void LogUnmute();

protected:
  static void RegisterContext(ScMemoryContext const * ctx);
  static void UnregisterContext(ScMemoryContext const * ctx);

private:
  static bool HasMemoryContext(ScMemoryContext const * ctx);

private:
  static sc_memory_context * ms_globalContext;

  using MemoryContextList = std::list<ScMemoryContext const *>;
  static MemoryContextList ms_contexts;
};

//! Class used to work with memory. It provides functions to create/erase elements
class ScMemoryContext
{
public:
  struct Stat
  {
    uint32_t m_nodesNum;
    uint32_t m_linksNum;
    uint32_t m_edgesNum;

    uint32_t GetAllNum() const
    {
      return m_nodesNum + m_linksNum + m_edgesNum;
    }
  };

public:
  _SC_EXTERN explicit ScMemoryContext(sc_uint8 accessLevels, std::string const & name = "");
  _SC_EXTERN explicit ScMemoryContext(std::string const & name);
  _SC_EXTERN ~ScMemoryContext();

  // Disable object copying
  ScMemoryContext(ScMemoryContext const & other) = delete;
  ScMemoryContext & operator=(ScMemoryContext const & other) = delete;

  sc_memory_context const * operator*() const
  {
    return m_context;
  }
  sc_memory_context const * GetRealContext() const
  {
    return m_context;
  }

  //! Call this function, when you request to destroy real memory context, before destructor calls for this object
  _SC_EXTERN void Destroy();

  //! Begin events pending mode
  void BeingEventsPending();

  //! End events pending mode
  void EndEventsPending();

  // returns copy, because of Python wrapper
  std::string const & GetName() const
  {
    return m_name;
  }

  _SC_EXTERN bool IsValid() const;

  //! Check if element exists with specified addr
  _SC_EXTERN bool IsElement(ScAddr const & addr) const;
  //! Erase element from sc-memory and returns true on success; otherwise returns false.
  _SC_EXTERN bool EraseElement(ScAddr const & addr);

  _SC_EXTERN ScAddr CreateNode(ScType const & type);
  _SC_EXTERN ScAddr CreateLink(ScType const & type = ScType::LinkConst);

  SC_DEPRECATED(0.3.0, "Use ScMemoryContext::createEdge instead.")
  _SC_EXTERN ScAddr CreateArc(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd);

  _SC_EXTERN ScAddr CreateEdge(ScType const & type, ScAddr const & addrBeg, ScAddr const & addrEnd);

  //! Returns type of sc-element. If there are any error, then returns ScType::Unknown
  _SC_EXTERN ScType GetElementType(ScAddr const & addr) const;

  /*! Change subtype of sc-element.
   * Return true, if there are no any errors; otherwise return false.
   */
  _SC_EXTERN bool SetElementSubtype(ScAddr const & addr, sc_type subtype);

  _SC_EXTERN ScAddr GetEdgeSource(ScAddr const & edgeAddr) const;
  _SC_EXTERN ScAddr GetEdgeTarget(ScAddr const & edgeAddr) const;
  _SC_EXTERN bool GetEdgeInfo(ScAddr const & edgeAddr, ScAddr & outSourceAddr, ScAddr & outTargetAddr) const;

  SC_DEPRECATED(0.3.0, "Use ScMemoryContext::getEdgeSource instead.")
  _SC_EXTERN ScAddr GetArcBegin(ScAddr const & arcAddr) const;
  SC_DEPRECATED(0.3.0, "Use ScMemoryContext::getEdgeTarget instead.")
  _SC_EXTERN ScAddr GetArcEnd(ScAddr const & arcAddr) const;

  _SC_EXTERN bool SetLinkContent(ScAddr const & addr, ScStreamPtr const & stream);
  template <typename TContentType>
  bool SetLinkContent(ScAddr const & addr, TContentType const & value)
  {
    return SetLinkContent(addr, ScStreamMakeRead(value));
  }

  _SC_EXTERN ScStreamPtr GetLinkContent(ScAddr const & addr);
  template <typename TContentType>
  bool GetLinkContent(ScAddr const & addr, TContentType & typedContent)
  {
    std::string content;
    ScStreamPtr const & ptr = GetLinkContent(addr);
    if (ptr != nullptr && ptr->IsValid() && ScStreamConverter::StreamToString(ptr, content))
    {
      std::stringstream streamString(content);
      streamString >> typedContent;

      return SC_TRUE;
    }

    return SC_FALSE;
  }

  //! Returns true, if any links found
  SC_DEPRECATED(0.6.0, "Use `ScAddrList FindLinksByContent(ScStreamPtr const & stream)` instead.")
  _SC_EXTERN bool FindLinksByContent(ScStreamPtr const & stream, ScAddrVector & found);
  _SC_EXTERN ScAddrVector FindLinksByContent(ScStreamPtr const & stream);
  template <typename TContentType>
  ScAddrVector FindLinksByContent(TContentType const & value)
  {
    return FindLinksByContent(ScStreamMakeRead(value));
  }

  template <typename TContentType>
  ScAddrVector FindLinksByContentSubstring(TContentType const & value, size_t maxLengthToSearchAsPrefix = 0)
  {
    return FindLinksByContentSubstring(ScStreamMakeRead(value), maxLengthToSearchAsPrefix);
  }
  _SC_EXTERN ScAddrVector FindLinksByContentSubstring(ScStreamPtr const & stream, size_t maxLengthToSearchAsPrefix = 0);

  template <typename TContentType>
  std::vector<std::string> FindLinksContentsByContentSubstring(
      TContentType const & value,
      size_t maxLengthToSearchAsPrefix = 0)
  {
    return FindLinksContentsByContentSubstring(ScStreamMakeRead(value), maxLengthToSearchAsPrefix);
  }
  _SC_EXTERN std::vector<std::string> FindLinksContentsByContentSubstring(
      ScStreamPtr const & stream,
      size_t maxLengthToSearchAsPrefix = 0);

  //! Saves memory state
  _SC_EXTERN bool Save();

  template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
  std::shared_ptr<TIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> Iterator5(
      ParamType1 const & param1,
      ParamType2 const & param2,
      ParamType3 const & param3,
      ParamType4 const & param4,
      ParamType5 const & param5)
  {
    return std::shared_ptr<TIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>>(
        new TIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>(
            *this, param1, param2, param3, param4, param5));
  }

  template <typename ParamType1, typename ParamType2, typename ParamType3>
  std::shared_ptr<TIterator3<ParamType1, ParamType2, ParamType3>> Iterator3(
      ParamType1 const & param1,
      ParamType2 const & param2,
      ParamType3 const & param3)
  {
    return std::shared_ptr<TIterator3<ParamType1, ParamType2, ParamType3>>(
        new TIterator3<ParamType1, ParamType2, ParamType3>(*this, param1, param2, param3));
  }

  /* Make iteration by triples, and call fn function for each result.
   * fn function should have 3 parameters (ScAddr const & source, ScAddr const & edge, ScAddr const & target)
   */
  template <typename ParamType1, typename ParamType2, typename ParamType3, typename FnT>
  void ForEachIter3(ParamType1 const & param1, ParamType2 const & param2, ParamType3 const & param3, FnT && fn)
  {
    ScIterator3Ptr it = Iterator3(param1, param2, param3);
    while (it->Next())
      fn(it->Get(0), it->Get(1), it->Get(2));
  }

  /* Make iteration by 5-element constructions, and call fn function for each result.
   * fn function should have 5 parameters
   * (ScAddr const & source, ScAddr const & edge, ScAddr const & target, ScAddr const & attrEdge, ScAddr const & attr)
   */
  template <
      typename ParamType1,
      typename ParamType2,
      typename ParamType3,
      typename ParamType4,
      typename ParamType5,
      typename FnT>
  void ForEachIter5(
      ParamType1 const & param1,
      ParamType2 const & param2,
      ParamType3 const & param3,
      ParamType4 const & param4,
      ParamType5 const & param5,
      FnT && fn)
  {
    ScIterator5Ptr it = Iterator5(param1, param2, param3, param4, param5);
    while (it->Next())
      fn(it->Get(0), it->Get(1), it->Get(2), it->Get(3), it->Get(4));
  }

  /* Trying to resolve ScAddr by it system identifier. If element with specified identifier doesn't exist
   * and type is not empty, then it would be created with specified type.
   * Look at type parameter as ForceCreate flag, that contains type.
   * Important: Type should be any of ScType::Node...
   */
  SC_DEPRECATED(
      0.4.0,
      "Use should use ScMemoryContext::HelperResolveSystemIdtf(std::string const & sysIdtf, ScType const & type)")
  _SC_EXTERN bool HelperResolveSystemIdtf(
      std::string const & sysIdtf,
      ScAddr & outAddr,
      ScType const & type = ScType());
  _SC_EXTERN ScAddr HelperResolveSystemIdtf(std::string const & sysIdtf, ScType const & type = ScType());

  _SC_EXTERN bool HelperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr);
  _SC_EXTERN std::string HelperGetSystemIdtf(ScAddr const & addr);

  SC_DEPRECATED(0.3.0, "Use ScMemoryContext::HelperCheckEdge instead.")
  _SC_EXTERN bool HelperCheckArc(ScAddr const & begin, ScAddr end, sc_type arcType);
  _SC_EXTERN bool HelperCheckEdge(ScAddr const & begin, ScAddr end, ScType const & edgeType);

  SC_DEPRECATED(0.4.0, "Use ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf) instead.")
  _SC_EXTERN bool HelperFindBySystemIdtf(std::string const & sysIdtf, ScAddr & outAddr);
  _SC_EXTERN ScAddr HelperFindBySystemIdtf(std::string const & sysIdtf);

  _SC_EXTERN ScTemplate::Result HelperGenTemplate(
      ScTemplate const & templ,
      ScTemplateGenResult & result,
      ScTemplateParams const & params = ScTemplateParams::Empty,
      ScTemplateResultCode * resultCode = nullptr);
  _SC_EXTERN ScTemplate::Result HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResult & result);
  _SC_EXTERN ScTemplate::Result HelperSearchTemplateInStruct(
      ScTemplate const & templ,
      ScAddr const & scStruct,
      ScTemplateSearchResult & result);
  _SC_EXTERN ScTemplate::Result HelperBuildTemplate(
      ScTemplate & templ,
      ScAddr const & templAddr,
      const ScTemplateParams & params = ScTemplateParams());
  _SC_EXTERN ScTemplate::Result HelperBuildTemplate(ScTemplate & templ, std::string const & scsText);

  _SC_EXTERN Stat CalculateStat() const;

private:
  sc_memory_context * m_context;
  std::string m_name;
};

class ScMemoryContextEventsPendingGuard
{
public:
  _SC_EXTERN explicit ScMemoryContextEventsPendingGuard(ScMemoryContext & ctx)
    : m_ctx(ctx)
  {
    m_ctx.BeingEventsPending();
  }

  _SC_EXTERN ~ScMemoryContextEventsPendingGuard()
  {
    m_ctx.EndEventsPending();
  }

private:
  ScMemoryContext & m_ctx;
};
