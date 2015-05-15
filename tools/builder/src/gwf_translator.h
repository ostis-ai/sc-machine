/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _gwf_translator_h_
#define _gwf_translator_h_

#include "translator.h"

class GwfTranslator : public iTranslator
{
    friend class GwfTranslatorFactory;

protected:
    explicit GwfTranslator(sc_memory_context *ctx);
    virtual ~GwfTranslator();


public:
    //! @copydoc iTranslator::translateImpl
    bool translateImpl();
    //! @copydoc iTranslator::getFileExt
    const String& getFileExt() const;

private:
    /*! Process xml data from string
     * @return If there are any errors, then returns false; otherwise returns true.
     */
    bool processString(const String &data);

    /*! Find sc-addr by specified identifier
     * @return If sc-add founded, then if stored in \p addr, and function returns true;
     * otherwise function returns false
     */
    bool getScAddr(const String &idtf, sc_addr &addr);

    //! Converts string type to sc-type
    sc_type convertType(const String &type);
};

class GwfTranslatorFactory : public iTranslatorFactory
{
public:
    explicit GwfTranslatorFactory();
    virtual ~GwfTranslatorFactory();

    static const String EXTENSION;

    //! @copydoc iTranslatorFactory::createInstance
    iTranslator* createInstance(sc_memory_context *ctx);

    //! @copydoc iTranslatorFactory::getFileExt
    const String& getFileExt() const;
};

#endif
