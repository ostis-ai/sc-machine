/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/
#ifndef _gwf_translator_h_
#define _gwf_translator_h_

#include "translator.h"

class GwfTranslator : public iTranslator
{
    friend class GwfTranslatorFactory;

protected:
    explicit GwfTranslator();
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
    iTranslator* createInstance();

    //! @copydoc iTranslatorFactory::getFileExt
    const String& getFileExt() const;
};

#endif
