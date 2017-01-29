/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-memory/sc_memory_headers.h"
#include "sc-memory/sc_helper.h"
}

#include "sc_types.hpp"
#include "sc_addr.hpp"
#include "sc_iterator.hpp"
#include "sc_template.hpp"
#include "sc_event.hpp"

#include <list>
#include <string>

class ScMemoryContext;
class ScStream;

/// TODO: replace with ScType
#define SC_TYPE(__t__) ((sc_type)(__t__))

class ScMemory
{
  friend class ScMemoryContext;

public:
  //! Returns true, on memory initialized; otherwise returns false
  _SC_EXTERN static bool Initialize(sc_memory_params const & params);
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
  _SC_EXTERN explicit ScMemoryContext(sc_uint8 accessLevels = 0, std::string const & name = "");
  _SC_EXTERN ~ScMemoryContext();

  sc_memory_context const * operator * () const { return m_context; }
  sc_memory_context const * GetRealContext() const { return m_context; }

  //! Call this function, when you request to destroy real memory context, before destructor calls for this object
  _SC_EXTERN void Destroy();

  std::string const & GetName() const { return m_name; }

  _SC_EXTERN bool IsValid() const;

  //! Check if element exists with specified addr
  _SC_EXTERN bool IsElement(ScAddr const & addr) const;
  //! Erase element from sc-memory and returns true on success; otherwise returns false.
  _SC_EXTERN bool EraseElement(ScAddr const & addr);

  _SC_EXTERN ScAddr CreateNode(sc_type type);
  _SC_EXTERN ScAddr CreateLink();

  SC_DEPRECATED(0.3.0, "Use ScMemoryContext::createEdge instead.")
  _SC_EXTERN ScAddr CreateArc(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd);

  _SC_EXTERN ScAddr CreateEdge(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd);

  //! Returns type of sc-element. If there are any error, then returns 0
  _SC_EXTERN ScType GetElementType(ScAddr const & addr) const;

  /*! Change subtype of sc-element (subtype & sc_type_element_mask == 0).
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

  _SC_EXTERN bool SetLinkContent(ScAddr const & addr, ScStream const & stream);
  _SC_EXTERN bool GetLinkContent(ScAddr const & addr, ScStream & stream);

  //! Returns true, if any links found
  _SC_EXTERN bool FindLinksByContent(ScStream const & stream, ScAddrList & found);

  //! Saves memory state
  _SC_EXTERN bool Save();

  template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
  TIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5> * Iterator5(ParamType1 const & param1,
                                                                                     ParamType2 const & param2,
                                                                                     ParamType3 const & param3,
                                                                                     ParamType4 const & param4,
                                                                                     ParamType5 const & param5)
  {
    return new TIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>(*this, param1, param2, param3, param4, param5);
  }

  template <typename ParamType1, typename ParamType2, typename ParamType3>
  TIterator3<ParamType1, ParamType2, ParamType3> * Iterator3(ParamType1 const & param1,
                                                             ParamType2 const & param2,
                                                             ParamType3 const & param3)
  {
    return new TIterator3<ParamType1, ParamType2, ParamType3>(*this, param1, param2, param3);
  }


  /* Trying to reosolve ScAddr by it system identifier. If element with specified identifier doesn't exist
   * and type is not empty, then it would be created with specified type.
   * Look at type parameter as ForceCreate flag, that contains type.
   * Important: Type should be any of ScType::Node...
   */
  _SC_EXTERN bool HelperResolveSystemIdtf(std::string const & sysIdtf, ScAddr & outAddr, ScType const & type = ScType());
  _SC_EXTERN bool HelperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr);
  _SC_EXTERN std::string HelperGetSystemIdtf(ScAddr const & addr);

  SC_DEPRECATED(0.3.0, "Use ScMemoryContext::HelperCheckEdge instead.")
  _SC_EXTERN bool HelperCheckArc(ScAddr const & begin, ScAddr end, sc_type arcType);
  _SC_EXTERN bool HelperCheckEdge(ScAddr const & begin, ScAddr end, ScType const & edgeType);

  _SC_EXTERN bool HelperFindBySystemIdtf(std::string const & sysIdtf, ScAddr & outAddr);
  _SC_EXTERN bool HelperGenTemplate(ScTemplate const & templ, ScTemplateGenResult & result, ScTemplateGenParams const & params = ScTemplateGenParams::Empty, ScTemplateResultCode * resultCode = nullptr);
  _SC_EXTERN bool HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResult & result);
  _SC_EXTERN bool HelperSearchTemplateInStruct(ScTemplate const & templ, ScAddr const & scStruct, ScTemplateSearchResult & result);
  _SC_EXTERN bool HelperBuildTemplate(ScTemplate & templ, ScAddr const & templAddr);

private:
  // Disable object copying
  ScMemoryContext(ScMemoryContext const & other) {}
  ScMemoryContext & operator = (ScMemoryContext const & other) { return *this; }

private:
  sc_memory_context * m_context;
  std::string m_name;
};


