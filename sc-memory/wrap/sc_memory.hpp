/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc_memory.h"
}

#include "sc_types.hpp"
#include "sc_addr.hpp"

#include <list>
#include <string>

namespace sc
{

class MemoryContext;
class Stream;

class _SC_WRAP_EXTERN Memory
{
    friend class MemoryContext;

public:
    //! Returns true, on memory initialized; otherwise returns false
    static bool initialize(sc_memory_params const & params);
    static void shutdown(bool saveState = true);

protected:

    static void registerContext(MemoryContext const * ctx);
    static void unregisterContext(MemoryContext const * ctx);
private:
    static bool hasMemoryContext(MemoryContext const * ctx);

private:
    static sc_memory_context * msGlobalContext;

    typedef std::list<MemoryContext const *> tMemoryContextList;
    static tMemoryContextList msContexts;
};

//! Class used to work with memory. It provides functions to create/erase elements
class _SC_WRAP_EXTERN MemoryContext
{
public:
    explicit MemoryContext(sc_uint8 accessLevels = 0, std::string const & name = "");
    ~MemoryContext();

    sc_memory_context const * getRealContext() const { return mContext; }

    //! Call this function, when you request to destroy real memory context, before destructor calls for this object
    void destroy();

    std::string const & getName() const { return mName; }

    bool isValid() const;

    //! Check if element exists with specified addr
    bool isElement(Addr const & addr) const;
    //! Erase element from sc-memory and returns true on success; otherwise returns false.
    bool eraseElement(Addr const & addr);

    Addr createNode(sc_type type);
    Addr createLink();
    Addr createArc(sc_type type, Addr const & addrBeg, Addr const & addrEnd);

    //! Returns type of sc-element. If there are any error, then returns 0
    sc_type getElementType(Addr const & addr) const;
    /*! Change subtype of sc-element (subtype & sc_type_element_mask == 0).
     * Return true, if there are no any errors; otherwise return false.
     */
    bool setElementSubtype(Addr const & addr, sc_type subtype);

    Addr getArcBegin(Addr const & arcAddr) const;
    Addr getArcEnd(Addr const & arcAddr) const;

    bool setLinkContent(Addr const & addr, Stream const & stream);
    bool getLinkContent(Addr const & addr, Stream & stream);

    //! Returns true, if any links found
    bool findLinksByContent(Stream const & stream, tAddrList & found);

    //! Saves memory state
    bool save();

private:
    // Disable object copying
    MemoryContext(MemoryContext const & other) {}
	MemoryContext & operator = (MemoryContext const & other) { return *this; }

private:
    sc_memory_context * mContext;
    std::string mName;
};


}
