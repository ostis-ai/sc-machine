/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _translator_h_
#define _translator_h_

#include "types.h"
#include "exception.h"

extern "C"
{
#include "sc_memory_headers.h"
#include "sc_helper.h"
}

struct TranslatorParams
{
    //! Name of file to translate
    String fileName;
    //! Flag to generate format information based on file extensions
    bool autoFormatInfo;
};

/*! Interface for translators
 */
class iTranslator
{
public:


    typedef enum
    {
        IdtfSystem = 0,
        IdtfLocal = 1,
        IdtfGlobal = 2

    } eIdtfVisibility;

    explicit iTranslator(sc_memory_context *context);
    virtual ~iTranslator();

    /*! Translate specified file into memory
     * @param params Input parameters
     * @return If file translated without any errors, then returns true; otherwise returns false.
     */
    virtual bool translate(const TranslatorParams &params);

    //! Implementation of translate
    virtual bool translateImpl() = 0;

    //! Returns supported file extension
    virtual const std::string& getFileExt() const = 0;


protected:
    /*! Generates format relation in sc-memory by file extension
     * @param addr sc-addr of sc-link to create format relation
     * @param ext File extension
     */
    void generateFormatInfo(sc_addr addr, const String &ext);


    //! Determines visibility of identifier
    eIdtfVisibility _getIdentifierVisibility(const String &idtf) const;
    //! Appends sc-addr to specified map by it identifier
    void appendScAddr(sc_addr addr, const String &idtf);

protected:
    //! Translator parameters
    TranslatorParams mParams;

    typedef std::map<String, sc_addr> tStringAddrMap;
    //! Map that contains global identifiers
    static tStringAddrMap msGlobalIdtfAddrs;
    //! Map that contains system identifiers
    tStringAddrMap mSysIdtfAddrs;
    //! Map that contains local identifiers
    tStringAddrMap mLocalIdtfAddrs;
    //! Pointer to memory context
    sc_memory_context *mContext;
};

/*! Interface for translators factory
 */
class iTranslatorFactory
{
public:
    explicit iTranslatorFactory() {};
    virtual ~iTranslatorFactory() {};

    /*! Function to create translator instance
     */
    virtual iTranslator* createInstance(sc_memory_context *ctx) = 0;

    //! Returns supported file extension
    virtual const std::string& getFileExt() const = 0;
};

#endif // _translator_h_
