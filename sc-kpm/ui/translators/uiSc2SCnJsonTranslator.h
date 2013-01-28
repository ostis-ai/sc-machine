/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
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

#ifndef _uiSc2SCnJsonTranslator_h_
#define _uiSc2SCnJsonTranslator_h_

#include "uiTranslatorFromSc.h"

/*!
 * \brief Class that translates sc-construction into
 * SCn-code (json representation)
 */
class uiSc2SCnJsonTranslator : public uiTranslateFromSc
{
public:
    explicit uiSc2SCnJsonTranslator();
    virtual ~uiSc2SCnJsonTranslator();


    static sc_result ui_translate_sc2scn(sc_event *event, sc_addr arg);

protected:
    //! @copydoc uiTranslateFromSc::runImpl
    void runImpl();

    //! Translate one keyword semantic square
    void translateKeyword(sc_addr keyword_addr);

    //! Check if specified sc-element is translated
    bool isTranslated(sc_addr element) const;

protected:
    //! List of stranslated sc-elements
    tScAddrList mTranslatedAddrsList;
    //! List of keywords
    tScAddrList mKeywordsList;
};



#endif // _uiSc2SCnJsonTranslator_h_
